#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/response.h>
#include <horiba_cpp_sdk/devices/spectracq3s_discovery.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace horiba::devices {

SpectrAcq3sDiscovery::SpectrAcq3sDiscovery(
    std::shared_ptr<horiba::communication::Communicator> communicator)
    : communicator{std::move(communicator)} {}

void SpectrAcq3sDiscovery::execute(bool error_on_no_devices) {
  if (!this->communicator->is_open()) {
    this->communicator->open();
  }

  spdlog::debug("[SpectrAcq3sDiscovery] discover SpectrAcq3s");
  const auto _ignored_response = this->communicator->request_with_response(
      communication::Command("saq3_discover", {}));

  spdlog::debug("[SpectrAcq3sDiscovery] list SpectrAcq3s");
  const auto response = this->communicator->request_with_response(
      communication::Command("saq3_list", {}));

  spdlog::debug("[SpectrAcq3sDiscovery] response: {}",
                response.json_results().dump());

  if (response.json_results().empty() && error_on_no_devices) {
    throw std::runtime_error("No SpectrAcq3s connected");
  }

  auto raw_cdds = response.json_results();
  this->saq3s = this->parse_saq3s(raw_cdds);
}

std::vector<std::shared_ptr<single_devices::SpectrAcq3>>
SpectrAcq3sDiscovery::spectracq3s() const {
  return this->saq3s;
}

std::vector<std::shared_ptr<single_devices::SpectrAcq3>>
SpectrAcq3sDiscovery::parse_saq3s(nlohmann::json raw_saq3s) {
  spdlog::info("[SpectrAcq3sDiscovery] detected #{} SpectrAcq3S",
               raw_saq3s.size());
  std::vector<std::shared_ptr<single_devices::SpectrAcq3>> detected_saq3s;
  auto devices = raw_saq3s["devices"];
  for (auto& device : devices) {
    spdlog::info("[SpectrAcq3sDiscovery] SpectrAcq3: {}", device.dump());
    const int index = device["index"].get<int>();
    detected_saq3s.push_back(std::make_shared<single_devices::SpectrAcq3>(
        index, this->communicator));
  }
  return detected_saq3s;
}

} /* namespace horiba::devices */
