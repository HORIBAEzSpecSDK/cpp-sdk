#include <horiba_cpp_sdk/communication/command.h>
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/os/process.h>
#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

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
  spdlog::info("running on windows");
  auto icl_process = make_shared<WindowsProcess>(
      R"(C:\Program Files\HORIBA Scientific\SDK\)", R"(icl.exe)");
#else
  spdlog::info("running on linux or macOS");
  auto icl_process = make_shared<FakeProcess>();
#endif
  spdlog::info("creating icl device manager...");
  auto icl_device_manager = ICLDeviceManager(icl_process);

  spdlog::info("starting icl device manager...");
  icl_device_manager.start();
  spdlog::info("discovering devices...");
  icl_device_manager.discover_devices();

  const auto ccds = icl_device_manager.charge_coupled_devices();
  if (ccds.empty()) {
    spdlog::error("No CCDs found");
    return 1;
  }

  spdlog::info("found {} CCDs", ccds.size());
  const auto& ccd = ccds[0];

  try {
    ccd->open();

    auto config = ccd->get_configuration();

    spdlog::info("------ Configuration ------");
    spdlog::info("Gains: {}", config["gains"].dump(2));
    spdlog::info("Speeds: {}", config["speeds"].dump(2));
    ccd->set_acquisition_format(1,
                                ChargeCoupledDevice::AcquisitionFormat::IMAGE);

  } catch (const exception& e) {
    spdlog::error("Exception: {}", e.what());
    ccd->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    ccd->close();
    icl_device_manager.stop();
  } catch (const exception& e) {
    spdlog::error("Exception: {}", e.what());
    // we expect an exception when the socket gets closed by the remote
  }
  return 0;
}
