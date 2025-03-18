#ifndef SPECTRACQ3_DEVICES_DISCOVERY_H
#define SPECTRACQ3_DEVICES_DISCOVERY_H

#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/devices/device_discovery.h>
#include <horiba_cpp_sdk/devices/single_devices/spectracq3.h>

#include <memory>
#include <nlohmann/json.hpp>

namespace horiba::devices {
/**
 * @brief Represents a discovery of SpectrAcq cameras on the ICL
 */
class SpectrAcq3sDiscovery : public DeviceDiscovery {
 public:
  /**
   * @brief Builds a SpectrAcq3 discovery
   *
   * @param communicator The communicator that will talk to the ICL
   */
  explicit SpectrAcq3sDiscovery(
      std::shared_ptr<horiba::communication::Communicator> communicator);
  ~SpectrAcq3sDiscovery() override = default;

  /**
   * @brief Executes the SpectrAcq3 discovery
   *
   * @param error_on_no_devices Whether to throw an exception or not if no
   * devices are detected
   */
  void execute(bool error_on_no_devices) noexcept(false) override;

  /**
   * @brief SpectrAcq3s that have been discovered after calling the execute()
   * function
   *
   * @return The detected SpectrAcqs
   */
  [[nodiscard]] std::vector<std::shared_ptr<single_devices::SpectrAcq3>>
  spectracq3s() const;

 private:
  std::shared_ptr<horiba::communication::Communicator> communicator;
  std::vector<std::shared_ptr<single_devices::SpectrAcq3>> saq3s;

  std::vector<std::shared_ptr<single_devices::SpectrAcq3>> parse_saq3s(
      nlohmann::json raw_saq3s);
};
} /* namespace horiba::devices */
#endif /* ifndef SPECTRACQ3_DEVICES_DISCOVERY_H */
