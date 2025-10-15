#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/device.h>
#include <horiba_cpp_sdk/devices/single_devices/spectracq3.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>

namespace horiba::devices::single_devices {

using namespace nlohmann;

const SpectrAcq3::Channel SpectrAcq3::Channel::Current{"current"};
const SpectrAcq3::Channel SpectrAcq3::Channel::Voltage{"voltage"};
const SpectrAcq3::Channel SpectrAcq3::Channel::Ppd{"ppd"};
const SpectrAcq3::Channel SpectrAcq3::Channel::Photon{"photon"};

const std::unordered_set<SpectrAcq3::Channel, SpectrAcq3::Channel::Hash>
    SpectrAcq3::Channel::all_existing_channels{Current, Voltage, Ppd, Photon};

SpectrAcq3::SpectrAcq3(
    int id, std::shared_ptr<communication::Communicator> communicator)
    : Device(id, communicator) {}

void SpectrAcq3::open() {
  Device::open();
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_open", {{"index", Device::device_id()}}));
}

void SpectrAcq3::close() {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_close", {{"index", Device::device_id()}}));
}

bool SpectrAcq3::is_open() {
  auto response = Device::execute_command(
      communication::Command("saq3_isOpen", {{"index", Device::device_id()}}));
  return response.json_results().at("open").get<bool>();
}

bool SpectrAcq3::is_busy() {
  auto response = Device::execute_command(
      communication::Command("saq3_isBusy", {{"index", Device::device_id()}}));
  return response.json_results().at("isBusy").get<bool>();
}

std::string SpectrAcq3::get_firmware_version() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getFirmwareVersion", {{"index", Device::device_id()}}));
  return response.json_results().at("firmwareVersion").get<std::string>();
}

std::string SpectrAcq3::get_fpga_version() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getFPGAVersion", {{"index", Device::device_id()}}));
  return response.json_results().at("FpgaVersion").get<std::string>();
}

std::string SpectrAcq3::get_board_revision() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getBoardRevision", {{"index", Device::device_id()}}));
  return response.json_results().at("boardRevision").get<std::string>();
}

std::string SpectrAcq3::get_serial_number() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getSerialNumber", {{"index", Device::device_id()}}));
  return response.json_results().at("serialNumber").get<std::string>();
}

void SpectrAcq3::set_hv_bias_voltage(int bias_voltage) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_setHVBiasVoltage",
          {{"index", Device::device_id()}, {"biasVoltage", bias_voltage}}));
}

int SpectrAcq3::get_hv_bias_voltage() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getHVBiasVoltage", {{"index", Device::device_id()}}));
  return response.json_results().at("biasVoltage").get<int>();
}

int SpectrAcq3::get_max_hv_voltage_allowed() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getMaxHVVoltageAllowed", {{"index", Device::device_id()}}));
  return response.json_results().at("biasVoltage").get<int>();
}

void SpectrAcq3::set_acquisition_set(int scan_count, double time_step,
                                     double integration_time,
                                     int external_param) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_setAcqSet", {{"index", Device::device_id()},
                             {"scanCount", scan_count},
                             {"timeStep", time_step},
                             {"integrationTime", integration_time},
                             {"externalParam", external_param}}));
}

AcquisitionSetParameters SpectrAcq3::get_acquisition_set() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getAcqSet", {{"index", Device::device_id()}}));
  auto json_results = response.json_results();
  return AcquisitionSetParameters{
      json_results.at("scanCount").get<int>(),
      json_results.at("timeStep").get<double>(),
      json_results.at("integrationTime").get<double>(),
      json_results.at("externalParam").get<int>()};
}

void SpectrAcq3::acquisition_start(SpectrAcq3::TriggerMode trigger) {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_acqStart", {{"index", Device::device_id()},
                                               {"trigger", trigger}}));
}

void SpectrAcq3::acquisition_stop() {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_acqStop", {{"index", Device::device_id()}}));
}

void SpectrAcq3::acquisition_pause() {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_acqPause", {{"index", Device::device_id()}}));
}

void SpectrAcq3::acquisition_continue() {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_acqContinue", {{"index", Device::device_id()}}));
}

bool SpectrAcq3::is_data_available() {
  auto response = Device::execute_command(communication::Command(
      "saq3_isDataAvailable", {{"index", Device::device_id()}}));
  return response.json_results().at("isDataAvailable").get<bool>();
}

nlohmann::json SpectrAcq3::get_acquisition_data(
    std::unordered_set<Channel, Channel::Hash> channels) {
  if (channels.empty()) {
    spdlog::error("At least one channel must be requested");
    throw std::invalid_argument("At least one channel must be requested");
  }
  bool all_channel_exist = std::ranges::all_of(channels, [&](const auto& e) {
    return Channel::all_existing_channels.contains(e);
  });
  if (!all_channel_exist) {
    spdlog::error("One or more requested channels do not exist:");
    for (const auto& channel : channels) {
      if (!Channel::all_existing_channels.contains(channel)) {
        spdlog::error(" - {} is inexistant", channel.json_name());
      }
    }
    throw std::invalid_argument("One or more requested channels do not exist");
  }

  std::vector<std::string> channels_json_names;
  channels_json_names.reserve(channels.size());

  for (auto&& channel_json_name :
       channels | std::views::transform(
                      [](const Channel& c) { return c.json_name(); })) {
    channels_json_names.emplace_back(channel_json_name);
  }

  auto response = Device::execute_command(communication::Command(
      "saq3_getAvailableData",
      {{"index", Device::device_id()}, {"channels", channels_json_names}}));

  return response.json_results()["data"];
}

void SpectrAcq3::force_trigger() {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_forceTrigger", {{"index", Device::device_id()}}));
}

void SpectrAcq3::set_in_trigger_mode(HardwareTriggerPinMode mode) {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_setInTriggerMode",
                             {{"index", Device::device_id()}, {"mode", mode}}));
}

std::pair<SpectrAcq3::TriggerMode, SpectrAcq3::HardwareTriggerPinMode>
SpectrAcq3::get_in_trigger_mode() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getInTriggerMode", {{"index", Device::device_id()}}));
  return {static_cast<SpectrAcq3::TriggerMode>(
              response.json_results().at("inputTriggerMode").get<int>()),
          static_cast<SpectrAcq3::HardwareTriggerPinMode>(
              response.json_results().at("scanStartMode").get<int>())};
}

void SpectrAcq3::set_trigger_in_polarity(
    SpectrAcq3::TriggerInPolarity polarity) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_setTriggerInPolarity",
          {{"index", Device::device_id()}, {"polarity", polarity}}));
}

SpectrAcq3::TriggerInPolarity SpectrAcq3::get_trigger_in_polarity() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getTriggerInPolarity", {{"index", Device::device_id()}}));
  return static_cast<SpectrAcq3::TriggerInPolarity>(
      response.json_results().at("polarity").get<int>());
}

std::string SpectrAcq3::get_last_error() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getLastError", {{"index", Device::device_id()}}));
  return response.json_results().at("error").get<std::string>();
}

std::string SpectrAcq3::get_error_log() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getErrorLog", {{"index", Device::device_id()}}));
  return response.json_results().at("errors").get<std::string>();
}

void SpectrAcq3::clear_error_log() {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_clearErrorLog", {{"index", Device::device_id()}}));
}
}  // namespace horiba::devices::single_devices
