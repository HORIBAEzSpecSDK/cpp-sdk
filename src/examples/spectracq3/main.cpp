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
#include <string>
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

auto plot_spectral_data(const int start_wavelength, const int end_wavelength,
                        const std::vector<int> &x_data,
                        const std::vector<double> &y_data) -> void {
  using namespace matplot;

  auto fig = figure(true);
  fig->size(800, 900);

  plot(x_data, y_data, "b-");

  xlabel("wavelength (nm)");
  ylabel("current (nA)");
  auto plot_title = "Spectral data for range " +
                    std::to_string(start_wavelength) + "-" +
                    std::to_string(end_wavelength) + "[nm]";
  title(plot_title);
  grid(true);
  show();
}

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

  const auto spectracq3s = icl_device_manager.spectracq3_devices();
  if (spectracq3s.empty()) {
    cout << "No SpectrAcq3 devices found\n";
    icl_device_manager.stop();
    return 1;
  }
  const auto &spectracq3 = spectracq3s[0];

  const auto monos = icl_device_manager.monochromators();
  if (monos.empty()) {
    cout << "No monochromators found\n";
    icl_device_manager.stop();
    return 1;
  }
  const auto &mono = monos[0];

  try {
    mono->open();
    spectracq3->open();

    auto serial_number = spectracq3->get_serial_number();
    cout << "Serial number: " << serial_number << "\n";
    auto firmware_version = spectracq3->get_firmware_version();
    cout << "Firmware version: " << firmware_version << "\n";

    // do an acquisition
    constexpr auto start_wavelength = 490;
    constexpr auto end_wavelength = 520;
    constexpr auto increment_wavelength = 3;
    std::vector<int> wavelengths;
    for (int wavelength :
         std::views::iota(start_wavelength, end_wavelength + 1)) {
      if ((wavelength - start_wavelength) % increment_wavelength == 0) {
        wavelengths.push_back(wavelength);
      }
    }

    constexpr auto scan_count = 1;
    constexpr auto time_step = 0;
    constexpr auto integration_time = 1;
    constexpr auto external_param = 0;
    spectracq3->set_acquisition_set(scan_count, time_step, integration_time,
                                    external_param);

    std::vector<int> x_data(wavelengths.begin(), wavelengths.end());
    std::vector<double> y_data_current;
    std::vector<double> y_data_voltage;
    std::vector<double> y_data_counts;

    for (const auto &wavelength : wavelengths) {
      mono->move_to_target_wavelength(wavelength);
      while (mono->is_busy()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      cout << "Moved Mono to wavelength: " << wavelength << "\n";

      spectracq3->acquisition_start(1);
      std::this_thread::sleep_for(std::chrono::seconds(3));
      /* while (spectracq3->is_busy()) { */
      /*   std::this_thread::sleep_for(std::chrono::seconds(1)); */
      /*   cout << "Acquisition in progress...\n"; */
      /* } */
      if (!spectracq3->is_data_available()) {
        cout << "ERROR: No data available!\n";
        spectracq3->close();
        mono->close();
        icl_device_manager.stop();
        return 1;
      }
      auto data = spectracq3->get_acquisition_data();
      cout << "Acquisition completed for wavelength: " << wavelength << "nm, "
           << data << "\n";

      y_data_current.push_back(data[0]["currentSignal"]["value"]);
      y_data_voltage.push_back(data[0]["voltageSignal"]["value"]);
      y_data_counts.push_back(data[0]["ppdSignal"]["value"]);
    }

    plot_spectral_data(start_wavelength, end_wavelength, x_data,
                       y_data_current);

  } catch (const exception &e) {
    cout << e.what() << "\n";
    spectracq3->close();
    mono->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    spectracq3->close();
    mono->close();
    icl_device_manager.stop();
  } catch (const exception &e) {
    cout << e.what() << "\n";
    // we expect an exception when the socket gets closed by the remote
  }

  return 0;
}
