#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/spectracq3.h>

#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include <thread>
#include <tuple>

#include "../../icl_exe.h"

// Warning about getenv being unsafe, we don't care about it here
#pragma warning(disable : 4996)

namespace horiba::test {

using namespace horiba::devices::single_devices;
using namespace horiba::communication;
TEST_CASE_METHOD(ICLExe, "SpectrAcq3 test on HW", "[spectracq3_hw]") {
  const char* has_hardware = std::getenv("HAS_HARDWARE");
  if (has_hardware == nullptr || std::string(has_hardware) == "0" ||
      std::string(has_hardware) == "false") {
    SUCCEED("Skipped: HAS_HARDWARE is not set");
    return;
  }

  start();

  // arrange
  auto websocket_communicator =
      std::make_shared<WebSocketCommunicator>("127.0.0.1", "25010");
  websocket_communicator->open();
  auto _ignored_response =
      websocket_communicator->request_with_response(Command("ccd_discover"));
  auto spectracq3 = SpectrAcq3(0, websocket_communicator);

  SECTION("SpectrAcq3 can be opened") {
    // act
    spectracq3.open();
    auto spectracq3_open = spectracq3.is_open();

    // assert
    REQUIRE(spectracq3_open);
  }

  SECTION("SpectrAcq3 can be closed") {
    // arrange
    spectracq3.open();
    auto spectracq3_open_before_close = spectracq3.is_open();

    // act
    REQUIRE_NOTHROW(spectracq3.close());

    auto spectracq3_open_after_close = spectracq3.is_open();

    // assert
    REQUIRE(spectracq3_open_before_close);
    REQUIRE_FALSE(spectracq3_open_after_close);
  }

  SECTION("SpectrAcq3 firmware version") {
    // arrange
    spectracq3.open();

    // act
    auto firmware_version = spectracq3.get_firmware_version();

    // assert
    REQUIRE_FALSE(firmware_version.empty());
  }

  SECTION("SpectrAcq3 FPGA version") {
    // arrange
    spectracq3.open();

    // act
    auto fpga_version = spectracq3.get_fpga_version();

    // assert
    REQUIRE_FALSE(fpga_version.empty());
  }

  SECTION("SpectrAcq3 board revision") {
    // arrange
    spectracq3.open();

    // act
    auto board_revision = spectracq3.get_board_revision();

    // assert
    REQUIRE_FALSE(board_revision.empty());
  }

  SECTION("SpectrAcq3 serial number") {
    // arrange
    spectracq3.open();

    // act
    auto serial_number = spectracq3.get_serial_number();

    // assert
    REQUIRE_FALSE(serial_number.empty());
  }

  SECTION("SpectrAcq3 HV bias voltage") {
    // arrange
    spectracq3.open();
    auto expected_bias_voltage_before = 50;
    auto expected_bias_voltage_after = 51;
    REQUIRE_NOTHROW(
        spectracq3.set_hv_bias_voltage(expected_bias_voltage_before));
    auto actual_bias_voltage_before = spectracq3.get_hv_bias_voltage();

    // act
    REQUIRE_NOTHROW(
        spectracq3.set_hv_bias_voltage(expected_bias_voltage_after));
    auto actual_bias_voltage_after = spectracq3.get_hv_bias_voltage();

    // assert
    REQUIRE(actual_bias_voltage_before == expected_bias_voltage_before);
    REQUIRE(actual_bias_voltage_after == expected_bias_voltage_after);
  }

  SECTION("SpectrAcq3 max HV voltage allowed") {
    // arrange
    spectracq3.open();

    // act
    auto max_hv_voltage_allowed = spectracq3.get_max_hv_voltage_allowed();

    // assert
    REQUIRE(max_hv_voltage_allowed > 0);
  }

  SECTION("SpectrAcq3 acquisition set") {
    // arrange
    spectracq3.open();
    auto scan_count = 10;
    auto time_step = 0;
    auto integration_time = 1;
    auto external_param = 0;

    // act
    REQUIRE_NOTHROW(spectracq3.set_acquisition_set(
        scan_count, time_step, integration_time, external_param));
    REQUIRE_NOTHROW(spectracq3.acquisition_start(1));
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // assert
    REQUIRE(spectracq3.is_data_available());
  }

  SECTION("SpectrAcq3 acquisition data") {
    // arrange
    spectracq3.open();
    auto scan_count = 10;
    auto time_step = 0;
    auto integration_time = 1;
    auto external_param = 0;
    REQUIRE_NOTHROW(spectracq3.set_acquisition_set(
        scan_count, time_step, integration_time, external_param));
    REQUIRE_NOTHROW(spectracq3.acquisition_start(1));
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // act
    auto data = spectracq3.get_acquisition_data();

    // assert
    REQUIRE_FALSE(data.empty());
  }

  SECTION("SpectrAcq3 acquisition busy") {
    // arrange
    spectracq3.open();
    auto scan_count = 10;
    auto time_step = 0;
    auto integration_time = 1;
    auto external_param = 0;
    REQUIRE_NOTHROW(spectracq3.set_acquisition_set(
        scan_count, time_step, integration_time, external_param));

    // act
    auto acquisition_busy_before_start = spectracq3.is_busy();
    REQUIRE_NOTHROW(spectracq3.acquisition_start(1));
    auto acquisition_busy_after_start = spectracq3.is_busy();
    std::this_thread::sleep_for(std::chrono::seconds(15));
    auto acquisition_busy_after_end = spectracq3.is_busy();

    // assert
    REQUIRE_FALSE(acquisition_busy_before_start);
    REQUIRE(acquisition_busy_after_start);
    REQUIRE_FALSE(acquisition_busy_after_end);
  }

  SECTION("SpectrAcq3 acquisition pause and continue") {
    // arrange
    spectracq3.open();
    auto scan_count = 10;
    auto time_step = 0;
    auto integration_time = 1;
    auto external_param = 0;
    REQUIRE_NOTHROW(spectracq3.set_acquisition_set(
        scan_count, time_step, integration_time, external_param));
    REQUIRE_NOTHROW(spectracq3.acquisition_start(1));
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // act
    REQUIRE_NOTHROW(spectracq3.acquisition_pause());
    auto acquisition_busy_after_pause = spectracq3.is_busy();
    REQUIRE_NOTHROW(spectracq3.acquisition_continue());
    auto acquisition_busy_after_continue = spectracq3.is_busy();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    auto acquisition_busy_after_end = spectracq3.is_busy();

    // assert
    REQUIRE(acquisition_busy_after_pause);
    REQUIRE_FALSE(acquisition_busy_after_continue);
    REQUIRE_FALSE(acquisition_busy_after_end);
  }

  SECTION("SpectrAcq3 acquisition stop") {
    // arrange
    spectracq3.open();
    auto scan_count = 10;
    auto time_step = 0;
    auto integration_time = 1;
    auto external_param = 0;
    REQUIRE_NOTHROW(spectracq3.set_acquisition_set(
        scan_count, time_step, integration_time, external_param));
    REQUIRE_NOTHROW(spectracq3.acquisition_start(1));
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // act
    REQUIRE_NOTHROW(spectracq3.acquisition_stop());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto acquisition_busy_after_stop = spectracq3.is_busy();

    // assert
    REQUIRE_FALSE(acquisition_busy_after_stop);
  }

  SECTION("SpectrAcq3 trigger IN polarity") {
    // arrange
    spectracq3.open();
    auto expected_trigger_in_polarity_before = 0;
    auto expected_trigger_in_polarity_after = 1;

    // act
    REQUIRE_NOTHROW(spectracq3.set_trigger_in_polarity(
        expected_trigger_in_polarity_before));
    auto actual_trigger_in_polarity_before =
        spectracq3.get_trigger_in_polarity();
    REQUIRE_NOTHROW(
        spectracq3.set_trigger_in_polarity(expected_trigger_in_polarity_after));
    auto actual_trigger_in_polarity_after =
        spectracq3.get_trigger_in_polarity();

    // assert
    REQUIRE(actual_trigger_in_polarity_before ==
            expected_trigger_in_polarity_before);
    REQUIRE(actual_trigger_in_polarity_after ==
            expected_trigger_in_polarity_after);
  }

  SECTION("SpectrAcq3 trigger IN mode") {
    // arrange
    spectracq3.open();
    auto expected_trigger_in_mode_before = 1;
    auto expected_trigger_in_mode_after = 2;

    // act
    REQUIRE_NOTHROW(
        spectracq3.set_in_trigger_mode(expected_trigger_in_mode_before));
    auto actual_trigger_in_mode_before = spectracq3.get_in_trigger_mode().first;

    REQUIRE_NOTHROW(
        spectracq3.set_in_trigger_mode(expected_trigger_in_mode_after));
    auto actual_trigger_in_mode_after = spectracq3.get_in_trigger_mode().first;

    // assert
    REQUIRE(actual_trigger_in_mode_before == expected_trigger_in_mode_before);
    REQUIRE(actual_trigger_in_mode_after == expected_trigger_in_mode_after);
  }

  if (websocket_communicator->is_open()) {
    websocket_communicator->close();
  }

  stop();
}
}  // namespace horiba::test
