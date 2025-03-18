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
      communication::Command("saq3_is_busy", {{"id", Device::device_id()}}));
  return response.json_results().at("is_busy").get<bool>();
}

std::string SpectrAcq3::get_firmware_version() {
  auto response = Device::execute_command(communication::Command(
      "saq3_get_firmware_version", {{"id", Device::device_id()}}));
  return response.json_results().at("firmware_version").get<std::string>();
}

std::string SpectrAcq3::get_fpga_version() {
  auto response = Device::execute_command(communication::Command(
      "saq3_get_fpga_version", {{"id", Device::device_id()}}));
  return response.json_results().at("FPGA_version").get<std::string>();
}

std::string SpectrAcq3::get_board_revision() {
  auto response = Device::execute_command(communication::Command(
      "saq3_get_board_revision", {{"id", Device::device_id()}}));
  return response.json_results().at("board_revision").get<std::string>();
}

std::string SpectrAcq3::get_serial_number() {
  auto response = Device::execute_command(communication::Command(
      "saq3_get_serial_number", {{"id", Device::device_id()}}));
  return response.json_results().at("serial_number").get<std::string>();
}

void SpectrAcq3::set_integration_time(std::chrono::seconds integration_time) {
  [[maybe_unused]] auto ignored_response = Device::execute_command(
      communication::Command("saq3_setIntegrationTime",
                             {{"id", Device::device_id()},
                              {"integration_time", integration_time.count()}}));
}

std::chrono::seconds SpectrAcq3::get_integration_time() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getIntegrationTime", {{"id", Device::device_id()}}));
  return std::chrono::seconds(
      response.json_results().at("integration_time").get<int>());
}

void SpectrAcq3::set_hv_bias_voltage(int bias_voltage) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_setHVBiasVoltage",
          {{"id", Device::device_id()}, {"bias_voltage", bias_voltage}}));
}

int SpectrAcq3::get_hv_bias_voltage() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getHVBiasVoltage", {{"id", Device::device_id()}}));
  return response.json_results().at("bias_voltage").get<int>();
}

int SpectrAcq3::get_max_hv_voltage_allowed() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getMaxHVVoltageAllowed", {{"id", Device::device_id()}}));
  return response.json_results().at("bias_voltage").get<int>();
}

void SpectrAcq3::define_acquisition_set(int scan_count, int time_step,
                                        int integration_time,
                                        int external_param) {
  [[maybe_unused]] auto ignored_response =
      Device::execute_command(communication::Command(
          "saq3_defineAcqSet", {{"id", Device::device_id()},
                                {"scan_count", scan_count},
                                {"time_step", time_step},
                                {"integration_time", integration_time},
                                {"external_param", external_param}}));
}

AcquisitionSetParameters SpectrAcq3::get_acquisition_set() {
  auto response = Device::execute_command(
      communication::Command("saq3_getAcqSet", {{"id", Device::device_id()}}));
  auto json_results = response.json_results();
  return AcquisitionSetParameters{
      json_results.at("scan_count").get<int>(),
      std::chrono::seconds(json_results.at("time_step").get<int>()),
      std::chrono::seconds(json_results.at("integration_time").get<int>()),
      json_results.at("external_param").get<int>()};
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

nlohmann::json SpectrAcq3::get_trigger_mode() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getTriggerMode", {{"id", Device::device_id()}}));
  return response.json_results();
}

std::string SpectrAcq3::get_last_error() {
  auto response = Device::execute_command(communication::Command(
      "saq3_getLastError", {{"id", Device::device_id()}}));
  return response.json_results().at("last_error").get<std::string>();
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
