// Note: on Windows if you use scaling, add the environment variable
// GNUTERM="qt" to avoid strange rendering artifacts
#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>
#include <horiba_cpp_sdk/devices/single_devices/spectracq3.h>
#include <horiba_cpp_sdk/os/process.h>
#include <matplot/matplot.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <memory>

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
                        const std::vector<double>& x_data,
                        const std::vector<double>& y_data) -> void {
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
  using Channel = horiba::devices::single_devices::SpectrAcq3::Channel;


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
    spdlog::error("No SpectrAcq3 devices found.");
    icl_device_manager.stop();
    return 1;
  }
  const auto& spectracq3 = spectracq3s[0];

  const auto monos = icl_device_manager.monochromators();
  if (monos.empty()) {
    spdlog::error("No Monochromator devices found.");
    icl_device_manager.stop();
    return 1;
  }
  const auto& mono = monos[0];

  try {
    mono->open();
    spectracq3->open();

    auto serial_number = spectracq3->get_serial_number();
    spdlog::info("Serial number: {}", serial_number);
    auto firmware_version = spectracq3->get_firmware_version();
    spdlog::info("Firmware version: {}", firmware_version);

    // do an acquisition
    constexpr auto start_wavelength = 400;
    constexpr auto end_wavelength = 700;
    constexpr auto increment_wavelength = 1;
    std::vector<double> wavelengths;

    for (double wavelength = start_wavelength; wavelength <= end_wavelength + 1e-9; wavelength += increment_wavelength) {
        wavelengths.push_back(wavelength);
    }

    constexpr auto scan_count = 1;
    constexpr auto time_step = 0;
    constexpr auto integration_time = 0.1;
    constexpr auto external_param = 0;

    std::vector<double> x_data(wavelengths.begin(), wavelengths.end());
    std::vector<double> y_data_current;
    std::vector<double> y_data_voltage;
    std::vector<double> y_data_counts;

    for (const auto& wavelength : wavelengths) {
      mono->move_to_target_wavelength(wavelength);
      while (mono->is_busy()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      spdlog::info("Mono moved to wavelength: {}", wavelength);

      while (spectracq3->is_busy()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        spdlog::info("Spectracq3 is busy, waiting...");
      }
      spectracq3->set_acquisition_set(scan_count, time_step, integration_time,
                                      external_param);
      spectracq3->acquisition_start(SpectrAcq3::TriggerMode::START_AND_INTERVAL);
      while (spectracq3->is_busy()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        spdlog::info("Spectracq3 is busy, waiting...");
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(150));
      if (!spectracq3->is_data_available()) {
        spdlog::error("No data available for wavelength: {}", wavelength);
        spectracq3->close();
        mono->close();
        icl_device_manager.stop();
        return 1;
      }

      std::unordered_set<Channel, Channel::Hash> channels = {
        Channel::Voltage,
        Channel::Current,
        Channel::Photon,          // include only the ones you need
        //Channel:Ppd
      };

      auto data = spectracq3->get_acquisition_data(channels);
      spdlog::info("Acquisition data for wavelength {}nm: {}", wavelength,
                   data.dump());

      y_data_current.push_back(data[0]["currentSignal"]["value"]);
      y_data_voltage.push_back(data[0]["voltageSignal"]["value"]);
      y_data_counts.push_back(data[0]["pmtSignal"]["value"]);
    }

    plot_spectral_data(start_wavelength, end_wavelength, x_data,
                       y_data_counts);

  } catch (const exception& e) {
    spdlog::error("An error occurred: {}", e.what());
    spectracq3->close();
    mono->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    spectracq3->close();
    mono->close();
    icl_device_manager.stop();
  } catch (const exception& e) {
    spdlog::info("An error occurred while closing devices: {}", e.what());
    // we expect an exception when the socket gets closed by the remote
  }

  return 0;
}
