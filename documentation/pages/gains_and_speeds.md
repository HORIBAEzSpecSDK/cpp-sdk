# Gain and Speed

The gain and speed of the CCD depend which sensor model is installed in the camera. In essence, only the camera itself
knows which speed and gain settings it supports.

In order to determine the available gains and speeds of the CCD, the config of the ccd has to be retrieved:

```cpp
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/os/windows_process.h>
#include <horiba_cpp_sdk/communication/command.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

auto main(int argc, char *argv[]) -> int {
  using namespace nlohmann;
  using namespace horiba::devices;
  using namespace horiba::os;
  using namespace horiba::devices::single_devices;
  using namespace horiba::communication;
  using namespace std;

  spdlog::set_level(spdlog::level::debug);

  spdlog::info("running on windows");
  auto icl_process = make_shared<WindowsProcess>(R"(C:\Program Files\HORIBA Scientific\SDK\)", R"(icl.exe)");

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
  const auto ccd = ccds[0];

  try {
    ccd->open();

    auto config = ccd->get_configuration();

    spdlog::info("------ Configuration ------");
    spdlog::info("Gains: {}", config["gains"].dump(2));
    spdlog::info("Speeds: {}", config["speeds"].dump(2));
    ccd->set_acquisition_format(1, ChargeCoupledDevice::AcquisitionFormat::SPECTRA_IMAGE);

  } catch (const exception &e) {
    spdlog::error("Exception: {}", e.what());
    ccd->close();
    icl_device_manager.stop();
    return 1;
  }

  try {
    ccd->close();
    icl_device_manager.stop();
  } catch (const exception &e) {
    spdlog::error("Exception: {}", e.what());
  }
  return 0;
}
```

Note: You can also find this example in the file `examples/gain_speed_info/`

Once you run the code above, you will get the following information:

```txt
   [...]

   ------ Configuration -----
   Gains: [{"info": "High Sensitivity", "token": 2 }, { "info": "High Light", "token": 0 }, { "info": "Best Dynamic Range", "token": 1 }],
   Speeds: [{ "info": " 1 MHz Ultra ", "token": 2 }, { "info": "500 kHz Wrap", "token": 127 }, { "info": "45 kHz       ", "token": 0 }, { "info": " 1 MHz       ", "token": 1 }],
```

> [!WARNING]
> Remember those are example values!

Based on this information you can create your own gain and speed classes as follows:

```cpp
namespace horiba::devices::syncerity_oe {
  enum class Gain: int {
    HIGH_LIGHT = 0,
    BEST_DYNAMIC_RANGE = 1,
    HIGH_SENSITIVITY = 2
  };

  enum class Speed {
    _45_KHZ = 0
    _1_MHZ = 1
    _1_MHZ_ULTRA = 2
    _500_KHZ_WRAP = 127
  };
}
```

> [!IMPORTANT]
> You need to give the correct values to the enums based on the information retrieved from the config of the CCD.

You can then call the speed and gain functions as follows:

```cpp
#include <horiba_cpp_sdk/devices/icl_device_manager.h>
#include <horiba_cpp_sdk/devices/single_devices/ccd.h>
#include <horiba_cpp_sdk/os/windows_process.h>
// assuming you have the Gain and Speed classes defined as above in a file called gain_speed.h
#include "gain_speed.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

auto main(int argc, char *argv[]) -> int {
  using namespace nlohmann;
  using namespace horiba::devices;
  using namespace horiba::os;
  using namespace horiba::devices::single_devices;
  using namespace horiba::communication;
  using namespace std;

  spdlog::set_level(spdlog::level::debug);
  
  auto icl_process = make_shared<windowsprocess>(
      r"(c:\program files\horiba scientific\sdk\)", r"(icl.exe)");
  auto icl_device_manager = icldevicemanager(icl_process);
  
  icl_device_manager.start();
  icl_device_manager.discover_devices();
  
  const auto ccds = icl_device_manager.charge_coupled_devices();
  if (ccds.empty()) {
    spdlog::error("no ccds found");
    return 1;
  }
  
  const auto ccd = ccds[0];
  
  try {
    ccd->open();
    ccd->set_gain(syncerity_oe::Gain::HIGH_SENSITIVITY);
    ccd->set_speed(syncerity_oe::Speed::_1_MHZ);
    // ... etc
  } catch (const exception &e) {
    spdlog::error("exception: {}", e.what());
    ccd->close();
    icl_device_manager.stop();
    return 1;
  }
  
  try {
    ccd->close();
    icl_device_manager.stop();
  } catch (const exception &e) {
    spdlog::error("exception: {}", e.what());
  }
  
  return 0;
}
```
