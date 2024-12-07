#ifndef DEVICE_H
#define DEVICE_H

#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/communication/response.h>

namespace horiba::devices::single_devices {
/**
 * @brief Interface for devices connected to the ICL
 */
class Device {
 public:
  Device(int id, std::shared_ptr<communication::Communicator> communicator);
  virtual ~Device() = default;

  /**
   * @brief Opens the device.
   */
  virtual void open();

  /**
   * @brief Closes the device.
   */
  virtual void close() = 0;

  [[nodiscard]] int device_id() const;

 protected:
  communication::Response execute_command(
      const communication::Command& command);

 private:
  int id;
  std::shared_ptr<communication::Communicator> communicator;

  void handle_errors(const std::vector<std::string>& errors);
};
} /* namespace horiba::devices::single_devices */
#endif /* ifndef DEVICE_H */
