#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/response.h>
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/ccds_discovery.h>
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/monos_discovery.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "horiba_cpp_sdk/devices/spectracq3s_discovery.h"

namespace horiba::devices {
ICLDeviceManager::ICLDeviceManager(
    std::shared_ptr<horiba::os::Process> icl_process, std::string websocket_ip,
    std::string websocket_port, bool manage_icl_lifetime,
    bool enable_binary_messages)
    : icl_process{std::move(icl_process)},
      websocket_ip{std::move(websocket_ip)},
      websocket_port{std::move(websocket_port)},
      manage_icl_lifetime{manage_icl_lifetime},
      enable_binary_messages{enable_binary_messages},
      communicator{
          std::make_shared<horiba::communication::WebSocketCommunicator>(
              this->websocket_ip, this->websocket_port)} {}

void ICLDeviceManager::start() {
  spdlog::debug("[ICLDeviceManager] managing ICL lifetime: {}",
                this->manage_icl_lifetime);
  if (this->manage_icl_lifetime && !this->icl_process->running()) {
    this->icl_process->start();
    std::this_thread::sleep_for(std::chrono::seconds(4));
  }
  spdlog::debug("[ICLDeviceManager] ICL started");

  this->communicator->open();

  const communication::Response response =
      this->communicator->request_with_response(
          communication::Command("icl_info", {}));
  spdlog::debug("[ICLDeviceManager] ICL info: {}",
                response.json_results().dump());

  if (this->enable_binary_messages) {
    this->enable_binary_messages_on_icl();
  }

  this->discover_devices();
}

void ICLDeviceManager::stop() {
  if (this->manage_icl_lifetime && !this->communicator->is_open()) {
    this->communicator->open();
  }

  if (this->manage_icl_lifetime) {
    const auto _ignored_response = this->communicator->request_with_response(
        communication::Command("icl_shutdown", {}));
  }

  if (this->communicator->is_open()) {
    this->communicator->close();
  }

  spdlog::debug("[ICLDeviceManager] ICL stopped");
}

void ICLDeviceManager::discover_devices(bool error_on_no_device) {
  ChargeCoupledDevicesDiscovery ccds_discovery =
      ChargeCoupledDevicesDiscovery(this->communicator);

  ccds_discovery.execute(error_on_no_device);
  this->ccds = ccds_discovery.charge_coupled_devices();

  MonochromatorsDiscovery monochromators_discovery =
      MonochromatorsDiscovery(this->communicator);

  monochromators_discovery.execute(error_on_no_device);
  this->monos = monochromators_discovery.monochromators();

  SpectrAcq3sDiscovery spectracq3s_discovery =
      SpectrAcq3sDiscovery(this->communicator);

  spectracq3s_discovery.execute(error_on_no_device);
  this->spectracq3s = spectracq3s_discovery.spectracq3s();
}

std::vector<std::shared_ptr<horiba::devices::single_devices::Monochromator>>
ICLDeviceManager::monochromators() const {
  return this->monos;
}

std::vector<
    std::shared_ptr<horiba::devices::single_devices::ChargeCoupledDevice>>
ICLDeviceManager::charge_coupled_devices() const {
  return this->ccds;
}

std::vector<std::shared_ptr<horiba::devices::single_devices::SpectrAcq3>>
ICLDeviceManager::spectracq3_devices() const {
  return this->spectracq3s;
}

void ICLDeviceManager::enable_binary_messages_on_icl() {
  spdlog::debug("[ICLDeviceManager] enable binary messages on the ICL");

  const communication::Response response =
      this->communicator->request_with_response(
          communication::Command("icl_binMode", {{"mode", "all"}}));

  if (!response.errors().empty()) {
    for (auto& error : response.errors()) {
      spdlog::error(error);
    }
    // TODO: add DB Error
  }
}

} /* namespace horiba::devices */
