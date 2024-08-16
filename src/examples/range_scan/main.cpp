// Note: on Windows if you use scaling, add the environment variable
// GNUTERM="qt" to avoid strange rendering artifacts
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>
#include <horiba_cpp_sdk/os/process.h>
#include <matplot/matplot.h>

#include <chrono>
#include <cmath>
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

#include "labspec6_spectra_stitch.h"
#include "linear_spectra_stitch.h"
#include "simple_cut_spectra_stitch.h"

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
  using namespace horiba::examples;
  using namespace horiba::devices;
  using namespace horiba::os;
  using namespace horiba::devices::single_devices;
  using namespace horiba::communication;
  using namespace matplot;
  using namespace std;

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
  const auto ccd = ccds[0];

  const auto monos = icl_device_manager.monochromators();
  const auto mono = monos[0];
  const auto timeout = std::chrono::seconds(180);

  try {
    ccd->open();
    mono->open();
    mono->wait_until_ready(timeout);

    mono->home();
    mono->wait_until_ready(timeout);

    ccd->set_acquisition_format(
        1, ChargeCoupledDevice::AcquisitionFormat::SPECTRA);
    ccd->set_acquisition_count(1);
    ccd->set_x_axis_conversion_type(
        ChargeCoupledDevice::XAxisConversionType::FROM_ICL_SETTINGS_INI);
    ccd->set_timer_resolution(
        ChargeCoupledDevice::TimerResolution::THOUSAND_MICROSECONDS);
    ccd->set_exposure_time(2);
    ccd->set_region_of_interest();

    if (ccd->get_acquisition_ready()) {
      vector<vector<vector<double>>> spectra;

      const double start_wavelength = 200.0;
      const double end_wavelength = 600.0;
      const int pixel_overlap = 10;

      auto wavelengths = ccd->range_mode_center_wavelenghts(
          mono->device_id(), start_wavelength, end_wavelength, pixel_overlap);

      for (auto wavelength : wavelengths) {
        mono->move_to_target_wavelength(wavelength);
        mono->wait_until_ready(timeout);
        ccd->set_center_wavelength(mono->device_id(), wavelength);

        auto open_shutter = true;
        ccd->set_acquisition_start(open_shutter);
        // wait a short time for the acquisition to start
        this_thread::sleep_for(chrono::milliseconds(200));

        while (ccd->get_acquisition_busy()) {
          this_thread::sleep_for(chrono::milliseconds(500));
        }

        auto raw_data = any_cast<json>(ccd->get_acquisition_data());
        auto xy_data = raw_data[0]["roi"][0]["xyData"];

        auto x_data_view = xy_data | views::transform([](const auto &pair) {
                             return pair[0];
                           });
        auto y_data_view = xy_data | views::transform([](const auto &pair) {
                             return pair[1];
                           });

        const vector<double> x_data(x_data_view.begin(), x_data_view.end());
        const vector<double> y_data(y_data_view.begin(), y_data_view.end());
        spectra.push_back({x_data, y_data});
      }

      // Here are some examples of how to stitch the spectra together.
      // You can of course create your own stitching algorithm.
      /* auto stitched_spectra =
       * make_unique<SimpleCutSpectraStitch>(spectra_list); */
      /* auto stitched_spectra_data = stitched_spectra->stitched_spectra(); */
      /* auto stitched_spectra =
       * make_unique<LabSpec6SpectraStitch>(spectra_list); */
      /* auto stitched_spectra_data = stitched_spectra->stitched_spectra(); */
      auto stitched_spectra = make_unique<LinearSpectraStitch>(spectra);
      auto stitched_spectra_data = stitched_spectra->stitched_spectra();

      plot(stitched_spectra_data[0], stitched_spectra_data[1]);
      title("Range Scan From " + to_string(start_wavelength) + "nm to " +
            to_string(end_wavelength) + "nm");
      xlabel("Wavelength [nm]");
      ylabel("Intensity");
      show();
    }

  } catch (const exception &e) {
    cout << e.what() << endl;
    ccd->close();
    mono->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    ccd->close();
    mono->close();
    icl_device_manager.stop();
  } catch (const exception &e) {
    cout << e.what() << endl;
    // we expect an exception when the socket gets closed by the remote
  }

  return 0;
}
