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

auto plot_spectral_data(const std::vector<double> &time_stamps,
                        const std::vector<double> &current_signals,
                        const std::vector<double> &pmt_signals,
                        const std::vector<double> &voltage_signals) -> void {
  using namespace matplot;

  auto fig = figure(true);
  fig->size(800, 900);

  subplot(3, 1, 1);
  plot(time_stamps, current_signals, "b-");
  xlabel("Elapsed Time (ms)");
  ylabel("Current (uAmps)");
  title("Current Signal vs Elapsed Time");

  subplot(3, 1, 2);
  plot(time_stamps, pmt_signals, "g-");
  xlabel("Elapsed Time (ms)");
  ylabel("PMT Signal (Counts/sec)");
  title("PMT Signal vs Elapsed Time");

  subplot(3, 1, 3);
  plot(time_stamps, voltage_signals, "r-");
  xlabel("Elapsed Time (ms)");
  ylabel("Voltage (Volts)");
  title("Voltage Signal vs Elapsed Time");

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

  try {
    spectracq3->open();

    auto serial_number = spectracq3->get_serial_number();
    cout << "Serial number: " << serial_number << "\n";
    auto firmware_version = spectracq3->get_firmware_version();
    cout << "Firmware version: " << firmware_version << "\n";

    // do and acquisition
    constexpr auto scan_count = 10;
    constexpr auto time_step = 0;
    constexpr auto integration_time = 1;
    constexpr auto external_param = 0;

    spectracq3->set_acquisition_set(scan_count, time_step, integration_time,
                                    external_param);
    spectracq3->acquisition_start(1);
    // TODO: uncomment as soon as saq3_isBusy works as expected
    /* while (spectracq3->is_busy()) { */
    /*   std::this_thread::sleep_for(std::chrono::seconds(1)); */
    /* } */
    std::this_thread::sleep_for(std::chrono::seconds(15));

    if (!spectracq3->is_data_available()) {
      cout << "ERROR: No data available!\n";
      spectracq3->close();
      icl_device_manager.stop();
      return 1;
    }

    auto data = spectracq3->get_acquisition_data();

    auto time_stamps = std::vector<double>();
    auto current_signals = std::vector<double>();
    auto pmt_signals = std::vector<double>();
    auto voltage_signals = std::vector<double>();

    for (const auto &record : data) {
      time_stamps.push_back(record["elapsedTime"].get<double>());
      current_signals.push_back(record["currentSignal"]["value"].get<double>());
      pmt_signals.push_back(record["pmtSignal"]["value"].get<double>());
      voltage_signals.push_back(record["voltageSignal"]["value"].get<double>());
    }

    plot_spectral_data(time_stamps, current_signals, pmt_signals,
                       voltage_signals);

  } catch (const exception &e) {
    cout << e.what() << "\n";
    spectracq3->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    spectracq3->close();
    icl_device_manager.stop();
  } catch (const exception &e) {
    cout << e.what() << "\n";
    // we expect an exception when the socket gets closed by the remote
  }

  return 0;
}
