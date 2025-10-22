// Note: on Windows if you use scaling, add the environment variable
// GNUTERM="qt" to avoid strange rendering artifacts
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/core/stitching/simple_spectra_stitch.h>
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>
#include <horiba_cpp_sdk/os/process.h>
#include <matplot/matplot.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <nlohmann/json.hpp>
#include <thread>
#include <vector>

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
  using namespace horiba::core::stitching;
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
  if (ccds.empty()) {
    spdlog::error("No CCD devices found.");
    return 1;
  }
  const auto& ccd = ccds[0];

  const auto monos = icl_device_manager.monochromators();
  if (monos.empty()) {
    spdlog::error("No Monochromator devices found.");
    return 1;
  }
  const auto& mono = monos[0];
  const auto timeout = std::chrono::seconds(180);

  try {
    ccd->open();
    mono->open();
    mono->wait_until_ready(timeout);

    mono->home();
    mono->wait_until_ready(timeout);

    mono->set_turret_grating(Monochromator::Grating::SECOND);
    mono->wait_until_ready(timeout);
    mono->set_slit_position(Monochromator::Slit::A, 0.5);
    mono->wait_until_ready(timeout);
    mono->set_mirror_position(Monochromator::Mirror::ENTRANCE,
                              Monochromator::MirrorPosition::AXIAL);
    mono->wait_until_ready(timeout);
    auto wavelength = mono->get_current_wavelength();

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

    if (ccd->get_acquisition_ready()) {
      std::vector<std::vector<std::vector<double>>> spectra;

      const double start_wavelength = 400.0;
      const double end_wavelength = 600.0;
      const int pixel_overlap = 10;

      auto wavelengths = ccd->range_mode_center_wavelenghts(
          mono->device_id(), start_wavelength, end_wavelength, pixel_overlap);

      for (auto wavelength : wavelengths) {
        mono->move_to_target_wavelength(wavelength);
        mono->wait_until_ready(timeout);

        ccd->set_center_wavelength(mono->device_id(), wavelength);

        auto open_shutter = true;

        std::any data_return;

        ccd->set_acquisition_start(open_shutter);
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

        std::vector<double> x_data =
            raw_data[0]["roi"][0]["xData"].get<std::vector<double>>();
        std::ranges::reverse(x_data);
        std::vector<double> y_data =
            raw_data[0]["roi"][0]["yData"][0].get<std::vector<double>>();
        std::ranges::reverse(y_data);
        spectra.push_back({x_data, y_data});
      }

      std::ranges::sort(spectra, [](const auto& a, const auto& b) {
        return a[0][0] < b[0][0];
      });

      // Here are some examples of how to stitch the spectra together.
      // You can of course create your own stitching algorithm.
      auto spectra_stitch = make_unique<SimpleSpectraStitch>(spectra);
      auto stitched_spectra_data = spectra_stitch->stitched_spectra();

      plot(stitched_spectra_data[0], stitched_spectra_data[1]);
      title("Range Scan From " + to_string(start_wavelength) + "nm to " +
            to_string(end_wavelength) + "nm");
      xlabel("Wavelength [nm]");
      ylabel("Intensity");
      show();
    }

  } catch (const exception& e) {
    spdlog::error("An error occurred: {}", e.what());
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
    // we expect an exception when the socket gets closed by the remote
    spdlog::info("An error occurred while closing devices: {}", e.what());
  }

  return 0;
}
