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

#include "raman_shift.h"

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
  using namespace horiba::examples;
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
  if (ccds.empty()) {
    cout << "No CCDs found\n";
    icl_device_manager.stop();
    return 1;
  }
  const auto &ccd = ccds[0];

  try {
    ccd->open();
    ccd->set_acquisition_count(1);
    ccd->set_x_axis_conversion_type(
        ChargeCoupledDevice::XAxisConversionType::FROM_CCD_FIRMWARE);
    ccd->set_acquisition_format(
        1, ChargeCoupledDevice::AcquisitionFormat::SPECTRA);
    constexpr auto exposure_time = chrono::milliseconds(5);
    ccd->set_exposure_time(exposure_time.count());
    ccd->set_timer_resolution(
        ChargeCoupledDevice::TimerResolution::THOUSAND_MICROSECONDS);
    ccd->set_region_of_interest();

    if (ccd->get_acquisition_ready()) {
      const auto open_shutter = true;
      ccd->set_acquisition_start(open_shutter);
      // wait a short time for the acquisition to start
      this_thread::sleep_for(chrono::seconds(1));

      constexpr auto sleep_time = chrono::milliseconds(500);
      while (ccd->get_acquisition_busy()) {
        this_thread::sleep_for(sleep_time);
      }

      auto raw_data = any_cast<json>(ccd->get_acquisition_data());
      vector<double> x_values = raw_data[0]["roi"][0]["xData"];
      vector<double> y_values = raw_data[0]["roi"][0]["yData"][0];
      const double excitation_wavelength = 520.0;
      auto raman_shift = RamanShift(x_values, excitation_wavelength);
      auto x_values_raman_shift = raman_shift.compute();

      plot(x_values_raman_shift, y_values);
      title("Raman Shift Spectrum With Excitation At " +
            to_string(excitation_wavelength) + "nm");
      xlabel("Raman Shift (cm^-1)");
      ylabel("Intensity");
      show();
    }

  } catch (const exception &e) {
    cout << e.what() << "\n";
    ccd->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    ccd->close();
    icl_device_manager.stop();
  } catch (const exception &e) {
    cout << e.what() << "\n";
    // we expect an exception when the socket gets closed by the remote
  }

  return 0;
}
