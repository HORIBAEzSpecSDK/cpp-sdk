#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "../../fake_icl_server.h"

namespace horiba::test {

using Catch::Matchers::Equals;
using Catch::Matchers::WithinAbs;

using namespace horiba::devices::single_devices;
using namespace horiba::communication;

TEST_CASE("Mono test with fake ICL", "[mono_no_hw]") {
  // arrange
  auto websocket_communicator = std::make_shared<WebSocketCommunicator>(
      FakeICLServer::FAKE_ICL_ADDRESS,
      std::to_string(FakeICLServer::FAKE_ICL_PORT));
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
    // we do not check if it is closed as the fake answer from the ICL always
    // returns true

    // assert
    REQUIRE(mono_open);
  }

  SECTION("Mono is busy") {
    // arrange
    mono.open();

    // act
    // assert
    REQUIRE_FALSE(mono.is_busy());
  }

  SECTION("Mono can be homed") {
    // arrange
    mono.open();

    // act
    // assert
    REQUIRE_NOTHROW(mono.home());
  }

  SECTION("Mono config") {
    // arrange
    mono.open();

    // act
    auto config = mono.configuration();

    // assert
    REQUIRE_FALSE(config.empty());
  }

  SECTION("Mono get current wavelength") {
    // arrange
    mono.open();
    const auto expected_wavelength = 320.0;
    const auto tolerance = 0.001;

    // act
    auto actual_wavelength = mono.get_current_wavelength();

    // assert
    REQUIRE_THAT(actual_wavelength, WithinAbs(expected_wavelength, tolerance));
  }

  SECTION("Mono wavelenght can be set") {
    // arrange
    mono.open();
    const auto wavelength = 120.0;

    // act
    // assert
    REQUIRE_NOTHROW(mono.calibrate_wavelength(wavelength));
    // we do not check if the new wavelength is set, as the fake answer from the
    // ICL always returns the same value
  }

  SECTION("Mono move to target wavelength") {
    // arrange
    mono.open();
    const auto wavelength = 125.0;

    // act
    // assert
    REQUIRE_NOTHROW(mono.move_to_target_wavelength(wavelength));
    // we do not check if the new wavelength is set, as the fake answer from the
    // ICL always returns the same value
  }

  SECTION("Mono get turret grating") {
    // arrange
    mono.open();

    // act
    auto turret_grating = mono.get_turret_grating();

    // assert
    REQUIRE(turret_grating == Monochromator::Grating::SECOND);
  }

  SECTION("Mono can set turret grating") {
    // arrange
    mono.open();

    // act
    // assert
    REQUIRE_NOTHROW(mono.set_turret_grating(Monochromator::Grating::FIRST));
    // we do not check if the new grating params are set, as the fake answer
    // from the ICL always returns the same value
  }

  SECTION("Mono get filter wheel position") {
    // arrange
    mono.open();
    const auto filter_wheel = Monochromator::FilterWheel::INTERNAL;
    const auto expected_filter_wheel_position =
        Monochromator::FilterWheelPosition::FIRST;

    // act
    auto filter_wheel_position = mono.get_filter_wheel_position(filter_wheel);

    // assert
    REQUIRE(filter_wheel_position == expected_filter_wheel_position);
  }

  SECTION("Mono set filter wheel position") {
    // arrange
    mono.open();
    const auto filter_wheel = Monochromator::FilterWheel::INTERNAL;
    const auto filter_wheel_position =
        Monochromator::FilterWheelPosition::SECOND;

    // act
    // assert
    REQUIRE_NOTHROW(
        mono.set_filter_wheel_position(filter_wheel, filter_wheel_position));
    // we do not check if the new filter wheel position is set, as the fake
    // answer from the ICL always returns the same value
  }

  SECTION("Mono get mirror position") {
    // arrange
    mono.open();
    const auto mirror = Monochromator::Mirror::ENTRANCE;
    const auto expected_mirror_position = Monochromator::MirrorPosition::AXIAL;

    // act
    auto mirror_position = mono.get_mirror_position(mirror);

    // assert
    REQUIRE(mirror_position == expected_mirror_position);
  }

  SECTION("Mono mirror position can be set") {
    // arrange
    mono.open();
    const auto mirror = Monochromator::Mirror::ENTRANCE;
    const auto mirror_position = Monochromator::MirrorPosition::LATERAL;

    // act
    // assert
    REQUIRE_NOTHROW(mono.set_mirror_position(mirror, mirror_position));
    // we do not check if the new position is set, as the fake answer
    // from the ICL always returns the same value
  }

  SECTION("Mono get slit position") {
    // arrange
    mono.open();
    const auto tolerance = 0.001;
    const auto slit = Monochromator::Slit::A;

    // act
    auto slit_position = mono.get_slit_position_in_mm(slit);

    // assert
    REQUIRE_THAT(slit_position, WithinAbs(1.5, tolerance));
  }

  SECTION("Mono slit position can be set") {
    // arrange
    mono.open();

    // act
    // assert
    REQUIRE_NOTHROW(mono.set_slit_position(Monochromator::Slit::A, 2.2));
    // we do not check if the new slit position is set, as the fake
    // answer from the ICL always returns the same value
  }

  SECTION("Mono get slit step position") {
    // arrange
    mono.open();
    const auto slit = Monochromator::Slit::A;
    const auto expected_slit_step_position = 0;

    // act
    auto slit_step_position = mono.get_slit_step_position(slit);

    // assert
    REQUIRE(slit_step_position == expected_slit_step_position);
  }

  SECTION("Mono slit step position can be set") {
    // arrange
    mono.open();
    const auto slit = Monochromator::Slit::A;
    const auto slit_step_position = 200;

    // act
    // assert
    REQUIRE_NOTHROW(mono.set_slit_step_position(slit, slit_step_position));
    // we do not check if the new slit step position is set, as the fake answer
    // from the ICL always returns the same value
  }

  SECTION("Mono can open shutter") {
    // arrange
    mono.open();

    // act
    // assert
    REQUIRE_NOTHROW(mono.open_shutter());
    // we do not check if the shutter is open, as the fake answer from the ICL
    // always returns the same value
  }

  SECTION("Mono can close shutter") {
    // arrange
    mono.open();

    // act
    // assert
    REQUIRE_NOTHROW(mono.close_shutter());
    // we do not check if the shutter is open, as the fake answer from the ICL
    // always returns the same value
  }

  SECTION("Mono get shutter position") {
    // arrange
    mono.open();
    const auto shutter = Monochromator::Shutter::FIRST;
    const auto expected_shutter_position =
        Monochromator::ShutterPosition::CLOSED;

    // act
    auto shutter_position = mono.get_shutter_position(shutter);

    // assert
    REQUIRE(shutter_position == expected_shutter_position);
  }

  if (websocket_communicator->is_open()) {
    websocket_communicator->close();
  }
}
}  // namespace horiba::test
