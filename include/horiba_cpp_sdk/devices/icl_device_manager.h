#ifndef ICL_DEVICE_MANAGER_H
#define ICL_DEVICE_MANAGER_H

#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/devices/device_manager.h>
#include <horiba_cpp_sdk/os/process.h>

#include <string>

#include "horiba_cpp_sdk/devices/single_devices/spectracq3.h"

namespace horiba::communication {
class Communicator;
} /* namespace horiba::communication */

namespace horiba::devices {

/**
 * @brief Device Manager using the ICL to communicate with connected Horiba
 * devices
 */
class ICLDeviceManager final : public DeviceManager {
 public:
  /**
   * @brief Creates a device manager that uses the ICL to communicate with the
   * devices.
   *
   * @param icl_process The process representing the icl.exe
   * @param websocket_ip The ip where the icl.exe is accepting connections
   * @param websocket_port The port where the icl.exe is accepting connections
   * @param manage_icl_lifetime Whether to start and stop the icl.exe
   * @param enable_binary_messages Whether to enable or not binary messages
   * comming from the ICL
   */
  explicit ICLDeviceManager(std::shared_ptr<horiba::os::Process> icl_process,
                            std::string websocket_ip = "127.0.0.1",
                            std::string websocket_port = "25010",
                            bool manage_icl_lifetime = true,
                            bool enable_binary_messages = false);

  /**
   * @brief Starts the ICL device manager. Also starts the icl.exe if managing
   * its lifecycle.
   */
  void start() override;

  /**
   * @brief Stops the ICL device manager. Also stops the icl.exe if managing its
   * lifecycle.
   */
  void stop() override;

  /**
   * @brief Discovers connected Horiba devices to the ICL
   *
   * @param error_on_no_device Whether to throw an exception if no devices have
   * been found.
   */
  void discover_devices(bool error_on_no_device = false) override;

  /**
   * @brief The connected monochromators
   *
   * @return connected monochromators
   */
  [[nodiscard]] std::vector<
      std::shared_ptr<horiba::devices::single_devices::Monochromator>>
  monochromators() const override;

  /**
   * @brief The connected charge coupled devices.
   *
   * @return connected ccds
   */
  [[nodiscard]] std::vector<
      std::shared_ptr<horiba::devices::single_devices::ChargeCoupledDevice>>
  charge_coupled_devices() const override;

  /**
   * @brief The connected SpectrAcq3s
   *
   * @return connected SpectrAcq3s
   */
  [[nodiscard]] std::vector<
      std::shared_ptr<horiba::devices::single_devices::SpectrAcq3>>
  spectracq3_devices() const override;

 private:
  std::shared_ptr<horiba::os::Process> icl_process;
  std::string websocket_ip;
  std::string websocket_port;
  bool manage_icl_lifetime;
  bool enable_binary_messages;
  std::shared_ptr<horiba::communication::Communicator> communicator;
  std::vector<std::shared_ptr<horiba::devices::single_devices::Monochromator>>
      monos;
  std::vector<
      std::shared_ptr<horiba::devices::single_devices::ChargeCoupledDevice>>
      ccds;
  std::vector<std::shared_ptr<horiba::devices::single_devices::SpectrAcq3>>
      spectracq3s;

  void enable_binary_messages_on_icl();
};
} /* namespace horiba::devices */

#endif /* ifndef ICL_DEVICE_MANAGER_H */
