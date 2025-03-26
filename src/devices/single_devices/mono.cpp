#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>

#include <thread>

namespace horiba::devices::single_devices {
using namespace nlohmann;
Monochromator::Monochromator(
    int id, std::shared_ptr<communication::Communicator> communicator)
    : Device(id, std::move(communicator)) {}

void Monochromator::open() {
  Device::open();
  auto _ignored_response = Device::execute_command(
      communication::Command("mono_open", {{"index", Device::device_id()}}));
}

void Monochromator::close() {
  auto _ignored_response = Device::execute_command(
      communication::Command("mono_close", {{"index", Device::device_id()}}));
}

bool Monochromator::is_open() {
  auto response = Device::execute_command(
      communication::Command("mono_isOpen", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return json_results.at("open").get<bool>();
}

bool Monochromator::is_busy() {
  auto response = Device::execute_command(
      communication::Command("mono_isBusy", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return json_results.at("busy").get<bool>();
}

void Monochromator::home(bool force_homing) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_init", {{"index", Device::device_id()}, {"force", force_homing}}));
}

bool Monochromator::initialized() {
  auto response = Device::execute_command(communication::Command(
      "mono_isInitialized", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return json_results.at("initialized").get<bool>();
}

nlohmann::json Monochromator::configuration() {
  auto response = Device::execute_command(communication::Command(
      "mono_getConfig", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return json_results["configuration"];
}

double Monochromator::get_current_wavelength() {
  auto response = Device::execute_command(communication::Command(
      "mono_getPosition", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return json_results.at("wavelength").get<double>();
}

void Monochromator::calibrate_wavelength(double wavelength) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_setPosition",
      {{"index", Device::device_id()}, {"wavelength", wavelength}}));
}

void Monochromator::move_to_target_wavelength(double wavelength) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_moveToPosition",
      {{"index", Device::device_id()}, {"wavelength", wavelength}}));
}

Monochromator::Grating Monochromator::get_turret_grating() {
  auto response = Device::execute_command(communication::Command(
      "mono_getGratingPosition", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  auto grating = json_results.at("position").get<int>();

  return static_cast<Monochromator::Grating>(grating);
}

void Monochromator::set_turret_grating(Grating grating) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_moveGrating", {{"index", Device::device_id()},
                           {"position", static_cast<int>(grating)}}));
}

Monochromator::FilterWheelPosition Monochromator::get_filter_wheel_position(
    FilterWheel filter_wheel) {
  auto response = Device::execute_command(
      communication::Command("mono_getFilterWheelPosition",
                             {{"index", Device::device_id()},
                              {"locationId", static_cast<int>(filter_wheel)}}));
  auto json_results = response.json_results();
  auto position = json_results.at("position").get<int>();

  return static_cast<Monochromator::FilterWheelPosition>(position);
}

void Monochromator::set_filter_wheel_position(FilterWheel filter_wheel,
                                              FilterWheelPosition position) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_moveFilterWheel", {{"index", Device::device_id()},
                               {"locationId", static_cast<int>(filter_wheel)},
                               {"position", static_cast<int>(position)}}));
}

Monochromator::MirrorPosition Monochromator::get_mirror_position(
    Mirror mirror) {
  auto response = Device::execute_command(communication::Command(
      "mono_getMirrorPosition", {{"index", Device::device_id()},
                                 {"locationId", static_cast<int>(mirror)}}));
  auto json_results = response.json_results();
  auto position = json_results.at("position").get<int>();

  return static_cast<Monochromator::MirrorPosition>(position);
}

void Monochromator::set_mirror_position(Mirror mirror,
                                        MirrorPosition position) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_moveMirror", {{"index", Device::device_id()},
                          {"locationId", static_cast<int>(mirror)},
                          {"position", static_cast<int>(position)}}));
}

double Monochromator::get_slit_position_in_mm(Slit slit) {
  auto response = Device::execute_command(communication::Command(
      "mono_getSlitPositionInMM", {{"index", Device::device_id()},
                                   {"locationId", static_cast<int>(slit)}}));
  auto json_results = response.json_results();
  auto position = json_results.at("position").get<double>();

  return position;
}

void Monochromator::set_slit_position(Slit slit, double position_in_mm) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_moveSlitMM", {{"index", Device::device_id()},
                          {"locationId", static_cast<int>(slit)},
                          {"position", position_in_mm}}));
}

int Monochromator::get_slit_step_position(Slit slit) {
  auto response = Device::execute_command(communication::Command(
      "mono_getSlitStepPosition", {{"index", Device::device_id()},
                                   {"locationId", static_cast<int>(slit)}}));
  auto json_results = response.json_results();
  auto position = json_results.at("position").get<int>();

  return position;
}

void Monochromator::set_slit_step_position(Slit slit, int step_position) {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_moveSlit", {{"index", Device::device_id()},
                        {"locationId", static_cast<int>(slit)},
                        {"position", step_position}}));
}

void Monochromator::open_shutter() {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_shutterOpen", {{"index", Device::device_id()}}));
}

void Monochromator::close_shutter() {
  auto _ignored_response = Device::execute_command(communication::Command(
      "mono_shutterClose", {{"index", Device::device_id()}}));
}

Monochromator::ShutterPosition Monochromator::get_shutter_position(
    Shutter shutter) {
  auto response = Device::execute_command(communication::Command(
      "mono_getShutterStatus", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  ShutterPosition position = ShutterPosition::CLOSED;
  if (shutter == Shutter::FIRST) {
    position = json_results.at("shutter 1").get<ShutterPosition>();
  } else if (shutter == Shutter::SECOND) {
    position = json_results.at("shutter 2").get<ShutterPosition>();
  }

  return position;
}

void Monochromator::wait_until_ready(std::chrono::seconds timeout) {
  std::this_thread::sleep_for(std::chrono::seconds(1));

  int current_timeout_s = 0;
  while (this->is_busy() && current_timeout_s < timeout.count()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    current_timeout_s++;
  }

  if (this->is_busy()) {
    throw std::runtime_error(
        "timeout reached while waiting for monochromator to be ready");
  }
}

} /* namespace horiba::devices::single_devices */
