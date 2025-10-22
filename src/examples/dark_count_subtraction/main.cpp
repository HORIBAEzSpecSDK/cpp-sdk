// Note: on Windows if you use scaling, add the environment variable
// GNUTERM="qt" to avoid strange rendering artifacts
#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>
#include <horiba_cpp_sdk/os/process.h>
#include <matplot/matplot.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

#ifdef _WIN32
#include <horiba_cpp_sdk/os/windows_process.h>
#endif

namespace horiba::os {
class FakeProcess : public Process {
 public:
  void start() override { this->is_running = true; }
  bool running() override { return this->is_running; }
  void stop() override { this->is_running = false; }

 private:
  bool is_running = false;
};
} /* namespace horiba::os */

auto main() -> int {
  using namespace nlohmann;
  using namespace horiba::devices;
  using namespace horiba::os;
  using namespace horiba::devices::single_devices;
  using namespace horiba::communication;
  using namespace matplot;
  using namespace std;

  spdlog::set_level(spdlog::level::debug);

#ifdef _WIN32
  auto icl_process = std::make_shared<WindowsProcess>(
      R"(C:\Program Files\HORIBA Scientific\SDK\)", R"(icl.exe)");
#else
  auto icl_process = std::make_shared<FakeProcess>();
#endif

  auto icl_device_manager = ICLDeviceManager(icl_process);

  icl_device_manager.start();
  icl_device_manager.discover_devices();

  const auto ccds = icl_device_manager.charge_coupled_devices();
  const auto& ccd = ccds[0];

  const auto monos = icl_device_manager.monochromators();
  const auto& mono = monos[0];
  cout << "Mono index: " << mono->device_id() << "\n";
  const auto timeout = chrono::seconds(180);

  try {
    ccd->open();
    mono->open();
    mono->wait_until_ready(timeout);

    mono->home();
    mono->wait_until_ready(timeout);

    mono->set_turret_grating(Monochromator::Grating::THIRD);
    mono->wait_until_ready(timeout);
    const auto target_wavelength = 123.0;
    mono->move_to_target_wavelength(target_wavelength);
    mono->wait_until_ready(timeout);
    mono->set_slit_position(Monochromator::Slit::A, 0);
    mono->set_mirror_position(Monochromator::Mirror::ENTRANCE,
                              Monochromator::MirrorPosition::AXIAL);
    mono->wait_until_ready(timeout);

    // ccd config

    auto config = ccd->get_configuration();
    int chip_x = config["chipWidth"];
    int chip_y = config["chipHeight"];

    ccd->set_acquisition_format(
        1, ChargeCoupledDevice::AcquisitionFormat::SPECTRA_IMAGE);

    ccd->set_region_of_interest(1, 0, 0, chip_x, chip_y, 1, chip_y);

    auto center_wavelength = mono->get_current_wavelength();
    ccd->set_center_wavelength(mono->device_id(), center_wavelength);

    ccd->set_x_axis_conversion_type(
        ChargeCoupledDevice::XAxisConversionType::FROM_ICL_SETTINGS_INI);

    ccd->set_timer_resolution(
        ChargeCoupledDevice::TimerResolution::THOUSAND_MICROSECONDS);

    constexpr int exposure_time = 1000;
    ccd->set_exposure_time(exposure_time);

    ccd->set_acquisition_count(1);

    std::vector<double> x_values;
    std::vector<int> y_values_shutter_closed;

    std::any data_return;

    if (ccd->get_acquisition_ready()) {
      const auto close_shutter = false;
      ccd->set_acquisition_start(close_shutter);
      // wait a short time for the acquisition to start
      int sleep_time = (exposure_time / 1000) * 2;
      while (true) {
        try {
          this_thread::sleep_for(std::chrono::seconds(sleep_time));
          ;
          cout << "Trying for data...\n";
          data_return = ccd->get_acquisition_data();
          break;
        } catch (const std::exception& e) {
          std::cout << "Data not ready yet...\n";
        }
      }

      const json& raw_data = std::any_cast<const json&>(data_return);

      x_values = raw_data[0]["roi"][0]["xData"].get<std::vector<double>>();
      y_values_shutter_closed =
          raw_data[0]["roi"][0]["yData"][0].get<std::vector<int>>();
    }

    std::vector<int> y_values_shutter_open;
    if (ccd->get_acquisition_ready()) {
      const auto open_shutter = true;
      ccd->set_acquisition_start(open_shutter);
      // wait a short time for the acquisition to start
      int sleep_time = (exposure_time / 1000) * 2;
      while (true) {
        try {
          this_thread::sleep_for(std::chrono::seconds(sleep_time));
          ;
          cout << "Trying for data...\n";
          data_return = ccd->get_acquisition_data();
          break;
        } catch (const std::exception& e) {
          std::cout << "Data not ready yet...\n";
        }
      }

      const json& raw_data = std::any_cast<const json&>(data_return);

      y_values_shutter_open =
          raw_data[0]["roi"][0]["yData"][0].get<std::vector<int>>();
    }

    if (y_values_shutter_closed.size() != y_values_shutter_open.size()) {
      throw std::invalid_argument(
          "The number of data points in the two "
          "acquisitions is not the same. " +
          to_string(y_values_shutter_closed.size()) +
          " != " + to_string(y_values_shutter_open.size()));
    }

    std::vector<double> y_values_noise_free;
    std::ranges::transform(y_values_shutter_open, y_values_shutter_closed,
                           std::back_inserter(y_values_noise_free),
                           std::minus<>());

    plot(x_values, y_values_noise_free);
    title("Noise free center Scan At Wavelength " +
          to_string(target_wavelength) + "nm");
    xlabel("Wavelength [nm]");
    ylabel("Intensity");
    show();

  } catch (const exception& e) {
    cout << e.what() << "\n";
    ccd->close();
    mono->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    ccd->close();
    mono->close();
    icl_device_manager.stop();
  } catch (const exception& e) {
    cout << e.what() << "\n";
    // we expect an exception when the socket gets closed by the remote
  }

  return 0;
}
