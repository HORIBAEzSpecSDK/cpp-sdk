#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <chrono>
#include <cstdlib>
#include <thread>

#include "../../icl_exe.h"

// Warning about getenv being unsafe, we don't care about it here
#pragma warning(disable : 4996)

namespace horiba::test {

using Catch::Matchers::Equals;
using Catch::Matchers::WithinAbs;

using namespace horiba::devices::single_devices;
using namespace horiba::communication;

TEST_CASE_METHOD(ICLExe, "Mono test on HW", "[mono_hw]") {
  const char* has_hardware = std::getenv("HAS_HARDWARE");
  if (has_hardware == nullptr || std::string(has_hardware) == "0" ||
      std::string(has_hardware) == "false") {
    SUCCEED("Skipped: HAS_HARDWARE is not set");
    return;
  }

  // From the fixture ICLExe
  start();

  std::this_thread::sleep_for(std::chrono::seconds(4));

  // arrange
  auto websocket_communicator =
      std::make_shared<WebSocketCommunicator>("127.0.0.1", "25010");
  websocket_communicator->open();
  auto _ignored_response =
      websocket_communicator->request_with_response(Command("mono_discover"));
  auto mono = Monochromator(0, websocket_communicator);

  SECTION("Mono can be opened") {
    // act
    mono.open();
    auto mono_open = mono.is_open();

    // assert
    REQUIRE(mono_open);
  }

  SECTION("Mono can be closed") {
    // arrange
    mono.open();
    auto mono_open = mono.is_open();

    // act
    REQUIRE_NOTHROW(mono.close());
    auto mono_closed = mono.is_open();

    // assert
    REQUIRE(mono_open);
    REQUIRE_FALSE(mono_closed);
  }

  SECTION("When not used, Mono is not busy") {
    // arrange
    mono.open();
    const std::chrono::seconds max_timeout{2};
    mono.wait_until_ready(max_timeout);

    // act
    // assert
    REQUIRE_FALSE(mono.is_busy());
  }

  SECTION("Mono can be homed") {
    // arrange
    mono.open();
    const std::chrono::seconds max_timeout{360};

    // act
    mono.home();
    mono.wait_until_ready(max_timeout);

    // assert
    REQUIRE_FALSE(mono.is_busy());
  }

  SECTION("Mono config is not empty") {
    // arrange
    mono.open();

    // act
    auto config = mono.configuration();

    // assert
    REQUIRE_FALSE(config.empty());
  }

  SECTION("Mono can move to target wavelength") {
    // arrange
    mono.open();
    const auto target_wavelength = 125.0;
    const double tolerance = 0.1;
    const std::chrono::seconds max_timeout{360};
    mono.home();
    mono.wait_until_ready(max_timeout);

    // act
    mono.move_to_target_wavelength(target_wavelength);
    mono.wait_until_ready(max_timeout);
    auto wavelength = mono.get_current_wavelength();

    // assert
    REQUIRE_THAT(wavelength, WithinAbs(target_wavelength, tolerance));
  }

  SECTION("Mono wavelength can be set") {
    // arrange
    mono.open();
    // TODO: How to test this without messing up the monochromator?
    /* const auto target_wavelength = 125.0; */
    /* const int max_timeout_s = 30; */

    // act
    /* mono.calibrate_wavelength(target_wavelength); */
    /* int current_timeout_s = 0; */
    /* while (mono.is_busy() && current_timeout_s < max_timeout_s) { */
    /*   std::this_thread::sleep_for(std::chrono::seconds(1)); */
    /*   current_timeout_s++; */
    /* } */
    // assert
    REQUIRE_FALSE(mono.is_busy());
  }

  SECTION("Mono can change turret grating") {
    // arrange
    mono.open();
    auto expected_grating_before = Monochromator::Grating::FIRST;
    auto expected_grating_after = Monochromator::Grating::SECOND;
    const std::chrono::seconds max_timeout{180};

    // act
    mono.set_turret_grating(expected_grating_before);
    mono.wait_until_ready(max_timeout);
    auto turret_grating_before = mono.get_turret_grating();

    mono.set_turret_grating(expected_grating_after);
    mono.wait_until_ready(max_timeout);
    auto turret_grating_after = mono.get_turret_grating();

    // assert
    REQUIRE(turret_grating_before == expected_grating_before);
    REQUIRE(turret_grating_after == expected_grating_after);
  }

  // TODO: Function is not supported ATM
  SECTION("Mono can change filter wheel position") {
    // arrange
    mono.open();
    auto config = mono.configuration();
    if (config["filterWheels"].empty()) {
      SUCCEED("Skipped: Filter wheel is not supported by this mono");
      return;
    }

    const auto filter_wheel = Monochromator::FilterWheel::INTERNAL;
    const auto expected_filter_wheel_position_before =
        Monochromator::FilterWheelPosition::FIRST;
    const auto expected_filter_wheel_position_after =
        Monochromator::FilterWheelPosition::SECOND;
    const std::chrono::seconds max_timeout{180};

    // act
    mono.set_filter_wheel_position(filter_wheel,
                                   expected_filter_wheel_position_before);
    mono.wait_until_ready(max_timeout);
    auto filter_wheel_position_before =
        mono.get_filter_wheel_position(filter_wheel);

    mono.set_filter_wheel_position(filter_wheel,
                                   expected_filter_wheel_position_after);
    mono.wait_until_ready(max_timeout);
    auto filter_wheel_position_after =
        mono.get_filter_wheel_position(filter_wheel);

    // assert
    REQUIRE(filter_wheel_position_before ==
            expected_filter_wheel_position_before);
    REQUIRE(filter_wheel_position_after ==
            expected_filter_wheel_position_after);
  }

  SECTION("Mono can change mirror position") {
    // arrange
    mono.open();
    const auto mirror = Monochromator::Mirror::ENTRANCE;
    const auto expected_mirror_position_before =
        Monochromator::MirrorPosition::LATERAL;
    const auto expected_mirror_position_after =
        Monochromator::MirrorPosition::AXIAL;
    const std::chrono::seconds max_timeout{180};

    // act
    mono.set_mirror_position(mirror, expected_mirror_position_before);
    mono.wait_until_ready(max_timeout);
    auto mirror_position_before = mono.get_mirror_position(mirror);

    mono.set_mirror_position(mirror, expected_mirror_position_after);
    mono.wait_until_ready(max_timeout);
    auto mirror_position_after = mono.get_mirror_position(mirror);

    // assert
    REQUIRE(mirror_position_before == expected_mirror_position_before);
    REQUIRE(mirror_position_after == expected_mirror_position_after);
  }

  SECTION("Mono can change slit position by mm") {
    // arrange
    mono.open();
    const auto slit = Monochromator::Slit::A;
    const auto expected_slit_position_before_mm = 0.6;
    const auto expected_slit_position_after_mm = 4.0;
    const std::chrono::seconds max_timeout{180};
    const double tolerance = 0.1;

    // act
    mono.set_slit_position(slit, expected_slit_position_before_mm);
    mono.wait_until_ready(max_timeout);
    const auto slit_position_before_mm = mono.get_slit_position_in_mm(slit);

    mono.set_slit_position(slit, expected_slit_position_after_mm);
    mono.wait_until_ready(max_timeout);
    const auto slit_position_after_mm = mono.get_slit_position_in_mm(slit);

    // assert
    REQUIRE_THAT(slit_position_before_mm,
                 WithinAbs(expected_slit_position_before_mm, tolerance));
    REQUIRE_THAT(slit_position_after_mm,
                 WithinAbs(expected_slit_position_after_mm, tolerance));
  }

  SECTION("Mono can change slit by step position") {
    // arrange
    mono.open();
    const auto slit = Monochromator::Slit::A;
    const auto expected_slit_step_position_before = 200;
    const auto expected_slit_step_position_after = 300;
    const std::chrono::seconds max_timeout{180};

    // act
    mono.set_slit_step_position(slit, expected_slit_step_position_before);
    mono.wait_until_ready(max_timeout);
    const auto slit_step_position_before = mono.get_slit_step_position(slit);

    mono.set_slit_step_position(slit, expected_slit_step_position_after);
    mono.wait_until_ready(max_timeout);
    const auto slit_step_position_after = mono.get_slit_step_position(slit);

    // assert
    REQUIRE(slit_step_position_before == expected_slit_step_position_before);
    REQUIRE(slit_step_position_after == expected_slit_step_position_after);
  }

  SECTION("Mono can open and close shutter") {
    // arrange
    mono.open();
    const auto shutter = Monochromator::Shutter::FIRST;
    const auto expected_shutter_position_before =
        Monochromator::ShutterPosition::OPENED;
    const auto expected_shutter_position_after =
        Monochromator::ShutterPosition::CLOSED;
    const std::chrono::seconds max_timeout{180};

    // act
    mono.open_shutter();
    mono.wait_until_ready(max_timeout);
    const auto shutter_position_before = mono.get_shutter_position(shutter);

    mono.close_shutter();
    mono.wait_until_ready(max_timeout);
    const auto shutter_position_after = mono.get_shutter_position(shutter);

    // assert
    REQUIRE(shutter_position_before == expected_shutter_position_before);
    REQUIRE(shutter_position_after == expected_shutter_position_after);
  }

  if (mono.is_open()) {
    mono.close();
  }

  if (websocket_communicator->is_open()) {
    websocket_communicator->close();
  }

  stop();
}
}  // namespace horiba::test
