#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/device.h>
#include <horiba_cpp_sdk/devices/single_devices/spectracq3.h>

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

namespace horiba::devices::single_devices {

using namespace nlohmann;

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
      communication::Command("saq3_is_open", {{"id", Device::device_id()}}));
  return response.json_results().at("is_open").get<bool>();
}

bool SpectrAcq3::is_busy() {
  auto response = Device::execute_command(
      communication::Command("saq3_isBusy", {{"id", Device::device_id()}}));
  return response.json_results().at("isBusy").get<bool>();
}

std::string SpectrAcq3::get_firmware_version() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getFirmwareVersion", {{"id", Device::device_id()}}));
  return response.json_results().at("firmwareVersion").get<std::string>();
}

std::string SpectrAcq3::get_fpga_version() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getFpgaVersion", {{"id", Device::device_id()}}));
  return response.json_results().at("FpgaVersion").get<std::string>();
}

std::string SpectrAcq3::get_board_revision() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getBoardRevision", {{"id", Device::device_id()}}));
  return response.json_results().at("boardRevision").get<std::string>();
}

std::string SpectrAcq3::get_serial_number() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getSerialNumber", {{"id", Device::device_id()}}));
  return response.json_results().at("serialNumber").get<std::string>();
}

void SpectrAcq3::set_hv_bias_voltage(int bias_voltage) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_setHVBiasVoltage",
          {{"id", Device::device_id()}, {"biasVoltage", bias_voltage}}));
}

int SpectrAcq3::get_hv_bias_voltage() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getHVBiasVoltage", {{"id", Device::device_id()}}));
  return response.json_results().at("biasVoltage").get<int>();
}

int SpectrAcq3::get_max_hv_voltage_allowed() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getMaxHVVoltageAllowed", {{"id", Device::device_id()}}));
  return response.json_results().at("biasVoltage").get<int>();
}

void SpectrAcq3::set_acquisition_set(int scan_count, int time_step,
                                     int integration_time, int external_param) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_setAcqSet", {{"id", Device::device_id()},
                             {"scanCount", scan_count},
                             {"timeStep", time_step},
                             {"integrationTime", integration_time},
                             {"externalParam", external_param}}));
}

AcquisitionSetParameters SpectrAcq3::get_acquisition_set() {
  auto response = Device::execute_command(
      communication::Command("saq3_getAcqSet", {{"id", Device::device_id()}}));
  auto json_results = response.json_results();
  return AcquisitionSetParameters{
      json_results.at("scanCount").get<int>(),
      std::chrono::seconds(json_results.at("timeStep").get<int>()),
      std::chrono::seconds(json_results.at("integrationTime").get<int>()),
      json_results.at("externalParam").get<int>()};
}

void SpectrAcq3::acquisition_start(int trigger) {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_acqStart", {{"id", Device::device_id()},
                                               {"trigger", trigger}}));
}

void SpectrAcq3::acquisition_stop() {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_acqStop", {{"id", Device::device_id()}}));
}

void SpectrAcq3::acquisition_pause() {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_acqPause", {{"id", Device::device_id()}}));
}

void SpectrAcq3::acquisition_continue() {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_acqContinue", {{"id", Device::device_id()}}));
}

bool SpectrAcq3::is_data_available() {
  auto response = Device::execute_command(communication::Command(
      "saq3_isDataAvailable", {{"id", Device::device_id()}}));
  return response.json_results().at("isDataAvailable").get<bool>();
}

std::vector<nlohmann::json> SpectrAcq3::get_available_data() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getAvailableData", {{"id", Device::device_id()}}));
  std::vector<nlohmann::json> data;
  for (const auto& record : response.json_results().at("data")) {
    data.push_back(record);
  }
  return data;
}

void SpectrAcq3::force_trigger() {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_forceTrigger", {{"id", Device::device_id()}}));
}

void SpectrAcq3::set_in_trigger_mode(int mode) {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_setInTriggerMode",
                             {{"id", Device::device_id()}, {"mode", mode}}));
}

std::pair<int, int> SpectrAcq3::get_in_trigger_mode() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getInTriggerMode", {{"id", Device::device_id()}}));
  return {response.json_results().at("inputTriggerMode").get<int>(),
          response.json_results().at("scanStartMode").get<int>()};
}

void SpectrAcq3::set_trigger_in_polarity(int polarity) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_setTriggerInPolarity",
          {{"id", Device::device_id()}, {"polarity", polarity}}));
}

int SpectrAcq3::get_trigger_in_polarity() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getTriggerInPolarity", {{"id", Device::device_id()}}));
  return response.json_results().at("polarity").get<int>();
}

std::string SpectrAcq3::get_last_error() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getLastError", {{"id", Device::device_id()}}));
  return response.json_results().at("error").get<std::string>();
}

std::string SpectrAcq3::get_error_log() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getErrorLog", {{"id", Device::device_id()}}));
  return response.json_results().at("errors").get<std::string>();
}

void SpectrAcq3::clear_error_log() {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_clearErrorLog", {{"id", Device::device_id()}}));
}
}  // namespace horiba::devices::single_devices
