// Note: on Windows if you use scaling, add the environment variable
// GNUTERM="qt" to avoid strange rendering artifacts
#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/communication/websocket_communicator.h>
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/devices/single_devices/mono.h>
#include <horiba_cpp_sdk/os/process.h>
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
  spdlog::info("Discovered devices: {}", ccds.size());
  const auto& ccd = ccds[0];

  try {
    ccd->open();

    // ccd config

    auto config = ccd->get_configuration();
    int chip_x = config["chipWidth"];
    int chip_y = config["chipHeight"];

    ccd->set_acquisition_format(
        1, ChargeCoupledDevice::AcquisitionFormat::SPECTRA_IMAGE);

    ccd->set_region_of_interest(1, 0, 0, chip_x, chip_y, 1, chip_y);

    ccd->set_x_axis_conversion_type(
        ChargeCoupledDevice::XAxisConversionType::NONE);

    ccd->set_timer_resolution(
        ChargeCoupledDevice::TimerResolution::THOUSAND_MICROSECONDS);

    constexpr int exposure_time = 1000;
    ccd->set_exposure_time(exposure_time);

    ccd->set_acquisition_count(5);

    std::any data_return;

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
      spdlog::info("Acquisition data size: {}", raw_data.size());
      spdlog::info("Acquisition data: {}", raw_data.dump());
    }

  } catch (const exception& e) {
    spdlog::error("An error occurred: {}", e.what());
    ccd->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    ccd->close();
    icl_device_manager.stop();
  } catch (const exception& e) {
    // we expect an exception when the socket gets closed by the remote
    spdlog::info("An error occurred while closing the device: {}", e.what());
  }

  return 0;
}
