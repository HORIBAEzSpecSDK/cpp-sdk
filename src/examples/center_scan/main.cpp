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
  const auto &ccd = ccds[0];

  const auto monos = icl_device_manager.monochromators();
  const auto &mono = monos[0];
  cout << "Mono index: " << mono->device_id() << "\n";
  const auto timeout = std::chrono::seconds(180);

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

    // ccd configuration
    ccd->set_acquisition_count(1);
    ccd->set_center_wavelength(mono->device_id(), target_wavelength);
    constexpr auto exposure_time = chrono::milliseconds(1000);
    ccd->set_exposure_time(exposure_time.count());
    ccd->set_gain(0);   // Hight Light
    ccd->set_speed(2);  // 1 MHz Ultra
    ccd->set_timer_resolution(
        ChargeCoupledDevice::TimerResolution::THOUSAND_MICROSECONDS);
    ccd->set_acquisition_format(
        1, ChargeCoupledDevice::AcquisitionFormat::SPECTRA);
    ccd->set_region_of_interest();
    ccd->set_x_axis_conversion_type(
        ChargeCoupledDevice::XAxisConversionType::FROM_ICL_SETTINGS_INI);

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
      vector<int> y_values = raw_data[0]["roi"][0]["yData"][0];

      plot(x_values, y_values);
      title("Center Scan At Wavelength " + to_string(target_wavelength) + "nm");
      xlabel("Wavelength [nm]");
      ylabel("Intensity");
      show();
    }

  } catch (const exception &e) {
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
  } catch (const exception &e) {
    cout << e.what() << "\n";
    // we expect an exception when the socket gets closed by the remote
  }

  return 0;
}
