#ifndef SPECTRAC3_H
#define SPECTRAC3_H

#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/device.h>

#include <chrono>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace horiba::devices::single_devices {

class AcquisitionSetParameters final {
 public:
  explicit AcquisitionSetParameters(int scan_count,
                                    std::chrono::seconds time_step,
                                    std::chrono::seconds integration_time,
                                    int external_param)
      : _scan_count(scan_count),
        _time_step(time_step),
        _integration_time(integration_time),
        _external_param(external_param) {}

  /**
   * @brief Number of acquisition to perform
   *
   * @return Number of acquisition to perform
   */
  [[nodiscard]] int scan_count() const { return _scan_count; }

  /**
   * @brief Interval between successive scans for time based scan in seconds. If
   * 0 or not defined, the scans take place as fast as possible (limited by
   * integration time and monochromator move if applicable)
   *
   * @return Interval between successive scans for time based scan in seconds.
   */
  [[nodiscard]] std::chrono::seconds time_step() const { return _time_step; }

  /**
   * @brief Integration time in seconds
   *
   * @return Integration time in seconds
   */
  [[nodiscard]] std::chrono::seconds integration_time() const {
    return _integration_time;
  }

  /**
   * @brief User defined value
   *
   * @return User defined value
   */
  [[nodiscard]] int external_param() const { return _external_param; }

 private:
  int _scan_count;
  std::chrono::seconds _time_step;
  std::chrono::seconds _integration_time;
  int _external_param;
};

class SpectrAcq3 final : public Device {
 public:
  enum class TriggerMode : int {
    START_AND_INTERVAL = 1,
    TRIGGER_AND_INTERVAL = 2,
    WAIT_FOR_TRIGGER = 3,
  };

  SpectrAcq3(int id, std::shared_ptr<communication::Communicator> communicator);
  ~SpectrAcq3() override = default;

  /**
   * @brief Opens the device.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void open() noexcept(false) override;

  /**
   * @brief Closes the device.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void close() noexcept(false) override;

  /**
   * @brief Checks if the connection to the SpectrAcq3 device is open.
   *
   * @return True if the connection is open, false otherwise.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  bool is_open() noexcept(false);

  /**
   * @brief Checks whether the instrument is busy (e.g., performing
   * initialization or data acquisition).
   *
   * @return True if the instrument is busy, false otherwise.
   */
  bool is_busy() noexcept(false);

  /**
   * @brief Get the fimrware version of the device for the given index.
   *
   * @return Firmware version of the device
   */
  std::string get_firmware_version() noexcept(false);

  /**
   * @brief Get the FPGA version of the device.
   *
   * @return FPGA version of the device
   */
  std::string get_fpga_version() noexcept(false);

  /**
   * @brief Get the board revision of the device.
   *
   * @return Board revision of the device
   */
  std::string get_board_revision() noexcept(false);

  /**
   * @brief Get the serial number of the device.
   *
   * @return Serial number of the device
   */
  std::string get_serial_number() noexcept(false);

  /**
   * @brief Sets the integration time for the acquisition. If not set, the
   * default value will be used.
   *
   * @note This parameter can be set using @see @ref define_acquisition_set()
   *
   * @param integration_time
   */
  void set_integration_time(std::chrono::seconds integration_time) noexcept(
      false);

  /**
   * @brief Gets the integration time that was previously set. If no integration
   * time has been explicitly set, the default value is returned.
   *
   * @return Integration time in seconds
   */
  std::chrono::seconds get_integration_time() noexcept(false);

  /**
   * @brief Set the high bias voltage in Volts.
   *
   * TODO: check how and which default value
   * If not set then default value will be used.
   *
   * @param bias_voltage High bias voltage in Volts
   */
  void set_hv_bias_voltage(int bias_voltage) noexcept(false);

  /**
   * @brief Gets the bias voltage that was previously set. If no bias voltage
   * has been explicitly set, the default value is returned.
   *
   * @return
   */
  int get_hv_bias_voltage() noexcept(false);

  /**
   * @brief Gets the maximum bias high voltage allowed in Volts
   *
   * @return Maximum bias high voltage allowed in Volts
   */
  int get_max_hv_voltage_allowed() noexcept(false);

  /**
   * @brief Defines and sends the parameters for the acquisition set to perform
   * the acquisition.
   *
   * If the acquisition set is not defined, a single-point scan with default
   * settings is performed. Parameters that are not explicitly defined are set
   * to their default values. Parameters to define for the acquisition
   *
   * @param scan_count Number of acquisitions to perform
   * @param time_step Time interval in seconds between acquisitions
   * @param integration_time
   * @param external_paramu ser defined parameter
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void define_acquisition_set(int scan_count, int time_step,
                              int integration_time,
                              int external_param) noexcept(false);

  /**
   * @brief Get the acquisition set parameters.
   *
   * @return the acquisition set parameters.
   */
  AcquisitionSetParameters get_acquisition_set() noexcept(false);

  /**
   * @brief
   *
   * @note Define acquisition sets before starting an acquisition. Ensure
   * acquisition preparation is completed successfully. <br>Starting an
   * acquisition will return an error if:
   * - An acquisition is already running, or
   * - Acquisition preparation has not been completed.
   * - In the event of errors in the defined parameters, the result will include
   *   an `error_count` field indicating the number of errors detected.
   *   @see @ref get_error_log() to get the detailed error.
   *
   * @param trigger TODO create enum
   */
  void acquisition_start(int trigger) noexcept(false);

  /**
   * @brief Stops the current acquisition. The current data point is discarded.
   * The acquisition process must be checked and restarted if needed.
   */
  void acquisition_stop() noexcept(false);

  /**
   * @brief Pause active Acquisition. Current point is completed. Can be
   * continued. Needs to be Stopped to start a new Acquisition.
   *
   * An error will be returned if a pause is received while an acquisition is
   * not running.
   */
  void acquisition_pause() noexcept(false);

  /**
   * @brief Restart a paused acquisition.
   * An error will be returned if continue is received when not paused.
   */
  void acquisition_continue() noexcept(false);

  /**
   * @brief Check whether the acquired data is available.
   *
   * @return True if data is available, false otherwise.
   */
  bool is_data_available() noexcept(false);

  /**
   * @brief Retrieve the acquired data that is available so far.
   *
   * Note: Once the acquired data is read, it will be removed from the
   * device/software's data buffer. Ensure that you save the data to a local
   * buffer or storage before reading to prevent data loss.
   *
   * @return TODO create class or return json?
   */
  std::vector<nlohmann::json> get_available_data() noexcept(false);

  /**
   * @brief Software Trigger, treated the same as Hardware Trigger (IN).
   *
   * If no acquisition is in progress, the trigger will be ignored.
   */
  void force_trigger() noexcept(false);

  /**
   * @brief Tell the device how Hardware Trigger pin is used. Returns Error if
   * Acquisition is in Progress.
   *
   * @param mode TODO: add int enum
   */
  void set_in_trigger_mode(int mode) noexcept(false);

  /* [saq3\_setTriggerInPolarity](#saq3_setTriggerInPolarity) */
  /* [saq3\_getTriggerInPolarity](#saq3_getTriggerInPolarity) */

  /**
   * TODO: Needs clarification
   * @brief Read the trigger mode
   *
   * @return
   */
  nlohmann::json get_trigger_mode() noexcept(false);
  std::string get_last_error() noexcept(false);
  std::string get_error_log() noexcept(false);
  void clear_error_log() noexcept(false);
};
}  // namespace horiba::devices::single_devices
#endif /* ifndef SPECTRAC3_H */
