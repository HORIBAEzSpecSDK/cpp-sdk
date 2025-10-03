#ifndef MONO_H
#define MONO_H

#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/device.h>

#include <nlohmann/json.hpp>
#include <string>

namespace horiba::devices::single_devices {

/**
 * @brief Represents a Monochromator
 *
 * This class should not be created directly by the end user. Instead a
 * DeviceManager should be used to access the detected Monochromators on the
 * system.
 *
 */
class Monochromator final : public Device {
 public:
  Monochromator(int id,
                std::shared_ptr<communication::Communicator> communicator);
  ~Monochromator() override = default;

  /**
   * @brief Shutters installed in the monochromator. Depending on the model, not
   * all shutters may be available.
   */
  enum class Shutter : int { FIRST, SECOND };

  /**
   * @brief Position of the shutter
   */
  enum class ShutterPosition : int { CLOSED, OPENED };

  /**
   * @brief Gratings installed in the monochromator. Depending on the model, not
   * all gratings may be available.
   */
  enum class Grating : int { FIRST, SECOND, THIRD };

  /**
   * @brief Filter wheels installed in the monochromator. Depending on the
   * model, not all filter wheels may be available.
   */
  enum class FilterWheel : int { FIRST, SECOND };

  /**
   * @brief Positions of the filter wheel installed in the monochromator.
   * Depending on the model, not all positions may be available.
   */
  enum class FilterWheelPosition : int { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH };

  /**
   * @brief Mirrors installed in the monochromator. Depending on the model, not
   * all mirrors may be available.
   */
  enum class Mirror : int { ENTRANCE, EXIT };

  /**
   * @brief Possible positions of the mirrors
   */
  enum class MirrorPosition : int { AXIAL, LATERAL };

  /**
   * @brief Slits available on the monochromator. Depending on the model, not
   * all slits may be available.
   */
  enum class Slit : int { A = 1, B = 2, C = 3, D = 4 };

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
   * @brief Checks if the connection to the monochromator is open.
   *
   * @return True if the connection is open, false otherwise
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  bool is_open() noexcept(false);

  /**
   * @brief Checks if the monochromator is busy.
   *
   * @return True if busy, false otherwise
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  bool is_busy() noexcept(false);

  /**
   * @brief Starts the monochromator initialization process called "homing".
   *
   * Use is_busy() to know if the operation is still taking place.
   *
   * @param force_homing Force starts the initialization process.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void home(bool force_homing = false) noexcept(false);

  /**
   * @brief Checks if the monochromator is initialized.
   *
   * Note: This command may also return false when the mono is busy with another
   * command.
   *
   * @return True if initialized, false otherwise
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  bool initialized() noexcept(false);

  /**
   * @brief Returns the configuration of the monochromator.
   *
   * @return Configuration of the monochromator
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  nlohmann::json configuration() noexcept(false);

  /**
   * @brief Current wavelength of the monochromator's position in nm.
   *
   * @return The current wavelength in nm
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  double get_current_wavelength() noexcept(false);

  /**
   * @brief This command sets the wavelength value of the current grating
   * position of the monochromator.
   *
   * Warning! This could potentially un-calibrate the monochromator and report
   * an incorrect wavelength compared to the actual output wavelength.
   *
   * @param wavelength Wavelength in nm
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void calibrate_wavelength(double wavelength) noexcept(false);

  /**
   * @brief Orders the monochromator to move to the requested wavelength.
   *
   * Use is_busy() to know if the operation is still taking place.
   *
   * @param wavelength Wavelength in nm
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void move_to_target_wavelength(double wavelength) noexcept(false);

  /**
   * @brief Current grating of the turret
   *
   * @return Current grating of turret, see Monochromator::Grating enum for
   * possible values
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  Grating get_turret_grating() noexcept(false);

  /**
   * @brief Select turret grating
   *
   * @param grating New grating of the turret. See Monochromator::Grating enum
   * for possible values.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_turret_grating(Grating grating) noexcept(false);

  /**
   * @brief Current position of the filter wheel.
   *
   * @param filter_wheel Desired filter wheel to get the position from
   *
   * @return current position of the filter wheel. See
   * Monochromator::FilterWheelPosition enum for possible values.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  FilterWheelPosition get_filter_wheel_position(
      FilterWheel filter_wheel) noexcept(false);

  /**
   * @brief Sets the current position of the filter wheel.
   *
   * @param filter_wheel Desired filter wheel to set the position
   * @param position New position of the filter wheel. See
   * Monochromator::FilterWheelPosition enum for possible values.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_filter_wheel_position(FilterWheel filter_wheel,
                                 FilterWheelPosition position) noexcept(false);

  /**
   * @brief Position of the selected mirror
   *
   * @param mirror Desired mirror to get the position from. See
   * Monochromator::Mirror for possible values.
   *
   * @return Current mirror position. See Monochromator::MirrorPosition for
   * possible values
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  MirrorPosition get_mirror_position(Mirror mirror) noexcept(false);

  /**
   * @brief Sets the position of the selected mirror
   *
   * @param mirror Desired mirror to set the position. See Monochromator::Mirror
   * @param position Position to set. See Monochromator::MirrorPosition
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_mirror_position(Mirror mirror,
                           MirrorPosition position) noexcept(false);

  /**
   * @brief Returns the position in millimeters[mm] of the selected slit.
   *
   * @param slit Desired slit to get the position from. See Monochromator::Slit
   * for possible values
   *
   * @return Position in mm
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  double get_slit_position_in_mm(Slit slit) noexcept(false);

  /**
   * @brief Sets the position of the selected slit.
   *
   * @param slit Desired slit to set the position. See Monochromator::Slit for
   * possible values. position Position to set in millimeters[mm].
   * @param position_in_mm Position in mm
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_slit_position(Slit slit, double position_in_mm) noexcept(false);

  /**
   * @brief Returns the step position of the selected slit.
   *
   * @param slit Desired slit to get the position from.
   *
   * @return Returns the step position.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  int get_slit_step_position(Slit slit) noexcept(false);

  /**
   * @brief Sets the step position of the selected slit.
   *
   * @param slit Desired slit to set the step position. See Monochromator::Slit
   * for possible values.
   * @param step_position The step position.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_slit_step_position(Slit slit, int step_position) noexcept(false);

  /**
   * @brief Opens the shutter.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void open_shutter() noexcept(false);

  /**
   * @brief Closes the shutter.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void close_shutter() noexcept(false);

  /**
   * @brief Returns the shutter position.
   *
   * @param shutter Desired shutter to get the position from. See
   * Monochromator::Shutter.
   *
   * @return ShutterPosition::OPEN or ShutterPosition::CLOSED
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  ShutterPosition get_shutter_position(Shutter shutter) noexcept(false);

  /**
   * @brief Blocking waits until the monochromator is ready.
   *
   * @param timeout Maximum time, in seconds [s], to wait for the monochromator
   * to be ready.
   *
   * @throw std::runtime_error when the timeout is reached
   */
  void wait_until_ready(std::chrono::seconds timeout) noexcept(false);
};
}  // namespace horiba::devices::single_devices
#endif /* ifndef MONO_H */
