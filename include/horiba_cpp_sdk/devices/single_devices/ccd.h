#ifndef CCD_H
#define CCD_H

#include <horiba_cpp_sdk/communication/communicator.h>
#include <horiba_cpp_sdk/devices/single_devices/device.h>

#include <any>
#include <memory>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace horiba::devices::single_devices {
/**
 * @brief Represents a CCD camera
 *
 * This class should not be created directly by the end user. Instead a
 * DeviceManager should be used to access the detected CCDs on the system.
 *
 * TODO: write a basic description of the CCD
 *
 * The cells of the CCD can be binned and summed up.
 */
class ChargeCoupledDevice final : public Device {
 public:
  enum class AcquisitionFormat : int {
    SPECTRA = 0,
    IMAGE,
    CROP,
    FAST_KINETICS,
  };

  /**
   * @brief CCD X axis converstion type
   *
   * - None (default)
   * - CCD FIT parameters contained in the CCD firmware
   * - Mono Wavelength parameters contained in the icl_settings.ini file
   */
  enum class XAxisConversionType : int {
    NONE = 0,
    FROM_CCD_FIRMWARE,
    FROM_ICL_SETTINGS_INI
  };

  enum class CleanCountMode : int {
    NEVER = 0,
    FIRST_ONLY = 1,
    BETWEEN_ONLY = 2,
    EACH = 3,
    // TODO: clarify the meaning of the last mode
    MODE_UNKNOWN = 238,
  };

  enum class TimerResolution : int {
    THOUSAND_MICROSECONDS = 0,
    ONE_MICROSECOND
  };

  ChargeCoupledDevice(
      int id, std::shared_ptr<communication::Communicator> communicator);
  ~ChargeCoupledDevice() override = default;

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
   * @brief Checks if the connection to the CCD device is open.
   *
   * @return True if the connection is open, false otherwise.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  bool is_open() noexcept(false);

  /**
   * @brief Restarts the CCD.
   *
   * Note that this function only works if the camera has been opened before.
   * The connection to the camera stays open after the restart.
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void restart() noexcept(false);

  /**
   * @brief Returns the configuration of the CCD.
   *
   * @return Configuration of the CCD
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  nlohmann::json get_configuration();

  /**
   * @brief Returns the gain token of the CCD.
   *
   * Note: The CCD can have different sensors installed, which can have
   * different gain values. This is why only the token to the gain is returned.
   * You need to first check what gain values are available for the CCD using
   * the get_configuration function.
   *
   * @return Gain of the CCD
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  int get_gain_token() noexcept(false);

  /**
   * @brief Sets the gain of the CCD
   *
   * Note: The CCD can have different sensors installed, which can have
   * different gain values. Therefore you need to first check what gain values
   * are available for the CCD using the get_configuration function.
   *
   * @param gain_token Token of the gain to set
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_gain(int gain_token) noexcept(false);

  /**
   * @brief Returns the speed of the CCD
   *
   * Note: The CCD can have different sensors installed, which can have
   * different speed values. Therefore you need to first check what speed values
   * are available for the CCD using the get_configuration function.
   *
   * @return Speed token
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  int get_speed_token() noexcept(false);

  /**
   * @brief Sets the speed of the CCD
   *
   * Note: The CCD can have different sensors installed, which can have
   * different speed values. Therefore you need to first check what speed values
   * are available for the CCD using the get_configuration function.
   *
   * @param speed_token Token of the speed to set
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_speed(int speed_token) noexcept(false);

  /**
   * @brief Returns the parallel speed of the CCD
   *
   * Note: The CCD can have different sensors installed, which can have
   * different parallel speed values. Therefore you need to first check what
   * parallel speed values are available for the CCD using the get_configuration
   * function.
   *
   * @return Parallel speed token
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  int get_parallel_speed_token() noexcept(false);

  /**
   * @brief Sets the parallel speed of the CCD
   *
   * Note: The CCD can have different sensors installed, which can have
   * different parallel speed values. Therefore you need to first check what
   * parallel speed values are available for the CCD using the get_configuration
   * function.
   *
   * @param speed_token Token of the parallel speed to set
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_parallel_speed(int parallel_speed_token) noexcept(false);

  /**
   * @brief Returns the fit parameters of the CCD
   *
   * @return Fit parameters TODO: units and >details
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  std::vector<int> get_fit_parameters() noexcept(false);

  /**
   * @brief Returns the timer resolution of the CCD.
   *
   * @return Timer resolution
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  ChargeCoupledDevice::TimerResolution get_timer_resolution() noexcept(false);

  /**
   * @brief Sets the timer resolution of the CCD
   *
   * @param timer_resolution Timer resolution
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_timer_resolution(
      ChargeCoupledDevice::TimerResolution timer_resolution) noexcept(false);

  /**
   * @brief Sets the number of ROIs (Regions of Interest) or areas and the
   * acquisition format.
   *
   * After using this command to set the number of ROIs and format, the
   * ccd_setRoi command should be used to define each ROI.
   *
   * Note: The Crop (2) and Fast Kinetics (3) acquisition formats are not
   * supported by every CCD.
   *
   *
   * @param number_of_rois Number of regions of interest
   * @param acquisition_format Acquisition format
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_acquisition_format(
      int number_of_rois, AcquisitionFormat acquisition_format) noexcept(false);

  /**
   * @brief Gets the conversion type of the x axis.
   *
   * @return Conversion Type of the x axis
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  XAxisConversionType get_x_axis_conversion_type() noexcept(false);

  /**
   * @brief Sets the x-axis pixel conversion type to be used when retrieving
   * the acquisition data with thee.
   *
   * @param conversion_type Selected axis conversion type
   *
   * @throw std::runtime_error when an error occurred on the device side
   */
  void set_x_axis_conversion_type(XAxisConversionType conversion_type) noexcept(
      false);

  /**
   * @brief Gets the number of acquisitions to be performed.
   *
   * The acquisition count is used to perform multiple acquisitions in a row.
   *
   * @return int The acquisition count.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  int get_acquisition_count() noexcept(false);

  /**
   * @brief Sets the number of acquisitions to be performed. The acquisition
   * count is used to perform multiple acquisitions in a row.
   *
   * @param count Number of acquisitions to be performed.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_acquisition_count(int count) noexcept(false);

  /**
   * @brief Gets the number of cleans to be performed prior to measurement.
   *
   * @return std::pair<int, ChargeCoupledDevice::CleanCountMode> Clean count
   * information.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  std::pair<int, ChargeCoupledDevice::CleanCountMode>
  get_clean_count() noexcept(false);

  /**
   * @brief Sets the clean count mode of the CCD and the corresponding mode.
   *
   * @param count Number of acquisitions to be performed.
   * @param mode Clean count mode
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_clean_count(
      int count, ChargeCoupledDevice::CleanCountMode mode) noexcept(false);

  /**
   * @brief Returns the size of the data from the CCD.
   *
   * @return int Size of the data. TODO: In which unit?
   *
   * @throws std::exception When an error occurs on the device side.
   */
  int get_acquisition_data_size() noexcept(false);

  /**
   * @brief Returns the chip temperature of the CCD.
   *
   * @return double Chip temperature in degrees Celsius.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  double get_temperature() noexcept(false);

  /**
   * @brief Gets the chip resolution of the CCD.
   *
   * @return std::pair<int, int> Chip resolution (width, height).
   *
   * @throws std::exception When an error occurs on the device side.
   */
  std::pair<int, int> get_chip_size() noexcept(false);

  /**
   * @brief Returns the exposure time in milliseconds.
   *
   * @return int Exposure time in milliseconds.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  int get_exposure_time() noexcept(false);

  /**
   * @brief Sets the exposure time in milliseconds.
   *
   * @param exposure_time_ms Exposure time in milliseconds.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_exposure_time(int exposure_time_ms) noexcept(false);

  /**
   * @brief This command is used to get the current setting of the input
   * trigger.
   *
   * The address, event, and signalType parameters are used to define the input
   * trigger based on the supported options of that particular CCD.
   *
   * The supported trigger options are retrieved using the get_configuration
   * function, and begin with the “Triggers” string contained in the
   * configuration.
   *
   * @return std::tuple<bool, int, int, int> The input trigger settings.
   *            enabled (bool) Specifies if the signal is enabled (e.g. False =
   *                           Disabled),
   *            address (int) used to specify where the trigger is located.
   *                          (e.g. 0 = Trigger Input). Note: Value of -1
   *                          indicates that the input trigger is disabled,
   *            event (int) used to specify when the trigger event should occur.
   *                        (e.g. 0 = Once - Start All) Note: Value of -1
   *                        indicates that the input trigger is disabled,
   *            signal type (int) used to specify how the signal will cause the
   *                              input trigger. (e.g. 0 = TTL Falling Edge)
   *                              Note: Value of -1 indicates that the input
   *                              trigger is disabled,
   *
   * @throws std::exception When an error occurs on the device side.
   */
  std::tuple<bool, int, int, int> get_trigger_input() noexcept(false);

  /**
   * @brief This command is used to enable or disable the trigger input.
   *
   * When enabling the trigger input, the address, event, and signalType
   * parameters are used to define the input trigger based on the
   * supported options of that particular CCD.
   *
   * The supported trigger options are retrieved using the get_configuration
   * function, and begin with the “Triggers” string contained in the
   * configuration.
   *
   * @param enabled Enable or disable the trigger input. Note : When disabling
   * the input trigger, the address, event, and signalType parameters are
   * ignored.
   * @param address Used to specify where the trigger is located (e.g. 0 =
   * Trigger Input)
   * @param event Used to specify when the trigger event should occur. (e.g .0 =
   * Once - Start All)
   * @param signal_type Used to specify how the signal will cause the input
   * trigger. (e.g .0 = TTL Falling Edge)
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_trigger_input(bool enabled, int address, int event,
                         int signal_type) noexcept(false);

  /**
   * @brief This command is used to get the current setting of the signal
   * output.
   *
   * The address, event, and signalType parameters are used to define the signal
   * based on the supported options of that particular CCD.
   *
   * The supported signal options are retrieved using the get_configuration
   * command, and begin with the “Signals” string contained in the
   * configuration.
   *
   * @return std::tuple<bool, int, int, int> The signal output settings.
   *         enabled (bool): Specifies if the signal is enabled (e.g. False =
   *                         Disabled),
   *         address (int): Used to specify where the signal is located (e.g. 0
   *                        = Signal Output), Note: Value of -1 indicates that
   *                        the signal output is disabled,
   *         event (int): Used to specify when the signal event should occur.
   *                     (e.g. 3 = Shutter Open) Note: Value of -1 indicates
   *                     that the signal output is disabled,
   *         signal type (int): how the signal will cause the event. (e.g. 0 =
   *                            TTL Active High) Note: Value of -1 indicates
   *                            that the signal output is disabled,
   *
   * @throws std::exception When an error occurs on the device side.
   */
  std::tuple<bool, int, int, int> get_signal_output() noexcept(false);

  /**
   * @brief This command is used to enable or disable the signal output.
   *
   * When enabling the signal output, the address, event, and signalType
   * parameters are used to define the signal based on the supported options of
   * that particular CCD.
   *
   * The supported signal options are retrieved using the ccd_getConfig command,
   * and begin with the “Signals” string contained in the configuration.
   *
   *  @param enabled Enable or disable the signal output. Note: When disabling
   * the signal output, the address, event, and signal_type parameters are
   * ignored.
   *  @param address Used to specify where the signal is located (e.g. 0 =
   * Signal Output)
   *  @param event Used to specify when the signal event should occur. (e.g. 3 =
   * Shutter Open)
   *  @param signal_type How the signal will cause the event. (e.g. 0 = TTL
   * Active High)
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_signal_output(bool enabled, int address, int event,
                         int signal_type) noexcept(false);

  /**
   * @brief Returns true if the CCD is ready to acquire.
   *
   * @return bool True if the CCD is ready to acquire.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  bool get_acquisition_ready() noexcept(false);

  /**
   * @brief Starts an acquisition that has been set up according to the
   * previously defined acquisition parameters.
   *
   * Note: To specify the acquisiton parameters please see
   * set_region_of_interest, set_x_axis_conversion_type. If there are no
   * acquisition parameters set at the time of acquisition it may result in no
   * data being generated.
   *
   * @param open_shutter Whether the shutter of the camera should be open during
   * the acquisition.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_acquisition_start(bool open_shutter) noexcept(false);

  /**
   * @brief Sets the region of interest (ROI) of the CCD.
   *
   * @param roi_index Index of the region of interest. Defaults to 1.
   * @param x_origin X origin of the region of interest. Defaults to 0.
   * @param y_origin Y origin of the region of interest. Defaults to 0.
   * @param x_size X size of the region of interest. Defaults to 1024.
   * @param y_size Y size of the region of interest. Defaults to 256.
   * @param x_bin X bin of the region of interest. Defaults to 1.
   * @param y_bin Y bin of the region of interest. Defaults to 256.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_region_of_interest(int roi_index = 1, int x_origin = 0,
                              int y_origin = 0, int x_size = 1024,
                              int y_size = 256, int x_bin = 1,
                              int y_bin = 256) noexcept(false);

  /**
   * @brief Returns the acquisition data of the CCD.
   *
   * The acquisition data is a dictionary with the following keys:
   * - acqIndex: Acquisition number
   * - roi:
   *   - roiIndex: Region of Interest number
   *   - xOrigin: ROI’s X Origin
   *   - yOrigin: ROI’s Y Origin
   *   - xSize: ROI’s X Size
   *   - ySize: ROI’s Y Size
   *   - xBinning: ROI’s X Bin
   *   - yBinning: ROI’s Y Bin
   *   - xData: X data
   *   - yData: Y data
   * - Timestamp: This is a timestamp that relates to the time when the all the
   * programmed acquisitions have completed. The data from all programmed
   * acquisitions are retrieve from the CCD after all acquisitions have
   * completed, therefore the same timestamp is used for all acquisitions.
   *
   * @return std::any Acquisition data.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  std::any get_acquisition_data() noexcept(false);

  /**
   * @brief Returns true if the CCD is busy with the acquisition.
   *
   * @return bool True if the CCD is busy.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  bool get_acquisition_busy() noexcept(false);

  /**
   * @brief Stops the acquisition of the CCD.
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void abort_acquisition() noexcept(false);

  /**
   * @brief Sets the center wavelength value to be used in the grating equation.
   *
   * Used when X axis conversion is @ref
   * XAxisConversionType::FROM_ICL_SETTINGS_INI
   *
   * @param monochromator_id Monochromator ID that is used with this CCD
   * @param wavelength Center wavelength value in nm
   *
   * @throws std::exception When an error occurs on the device side.
   */
  void set_center_wavelength(int monochromator_id,
                             double wavelength) noexcept(false);

  /**
   * @brief Finds the center wavelength positions based on the input range and
   * pixel overlap.
   *
   * The following commands are prerequisites and should be called prior to
   * using this command:
   *    - @ref set_x_axis_conversion_type,
   *    - @ref set_acquisition_format,
   *    - @ref set_region_of_interest
   *
   * @param monochromator_id Monochromator ID that is used with this CCD
   * @param start_wavelength Start wavelength
   * @param end_wavelength End wavelength
   * @param pixel_overlap Overlap in pixels
   *
   * @return std::vector<double> Center wavelengths
   *
   * @throws std::exception When an error occurs on the device side.
   */
  std::vector<double> range_mode_center_wavelenghts(
      int monochromator_id, double start_wavelength, double end_wavelength,
      double pixel_overlap) noexcept(false);
};
} /* namespace horiba::devices::single_devices */
#endif /* ifndef CCD_H */
