# Usage Examples

This folder contains various examples of how to use the `horiba_cpp_sdk` library.

## Building

In order to execute the examples, you need to clone and build the `cpp-sdk` repository. 

> [!NOTE]
> For a straightforward setup, we recommend under Windows to install the dependencies using chocolatey as explained in
> [Dependencies](../../README_dependencies.md), use Visual Studio 2022 as compiler and its corresponding CMake generator (see [Building](../../README_building.md)).
>
> This would correspond to the following steps:
> 1. In an admin PowerShell execute:
> ```powershell
> choco install -y git cmake ninja visualstudio2022community --package-parameters "add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended --includeOptional --passive --locale en-US"
> ```
> 2. Clone the `cpp-sdk` repository
> 3. Configure and build the sdk:
> ```powershell
> cd cpp-sdk
> cmake -B build -S . -G "Visual Studio 17 2022"
> cmake --build build
> ```
> 4. The examples' executables are then located under `.\build\src\examples\`.

## Examples

Acquisition examples:
- [`center_scan`](center_scan/main.cpp): This example demonstrates how to perform a center scan using the
  `horiba_cpp_sdk` library. Before running this example, set the environment variable `GNUTERM` to `qt` to avoid issues
  with the `gnuplot` library.
- [`range_scan`](range_scan/main.cpp): This example demonstrates how to perform a range scan using the
  `horiba_cpp_sdk` library including stitching of spectra. Before running this example, set the environment variable
  `GNUTERM` to `qt` to avoid issues with the `gnuplot` library.
- [`multiple_acquisitions`](multiple_acquisitions/main.cpp): This example shows how
  to perform multiple acquisitions.
- [`dark_count_subtraction`](dark_count_subtraction/main.cpp): This example demonstrates how to perform a
  dark count subtraction. Before running this example, set the environment variable
  `GNUTERM` to `qt` to avoid issues with the `gnuplot` library.
- [`abort_acquisition`](abort_acquisition/main.cpp): This example demonstrates how to abort a running acquisition.
- [`raman_shift`](raman_shift/main.cpp): This example demonstrates how to compute the raman shift of a spectrum.
- [`spectracq3`](spectracq3/main.cpp): This example demonstrates how to do an acquisition with the SpectrAcq3 device and
  monochromator.

Configuration examples:
- [`gain_speed_info`](gain_speed_info/main.cpp): This example demonstrates how to create your own gain and speed enums based on
  the device's capabilities.
- [`trigger`](trigger/main.cpp): This example demonstrates how to set a custom trigger.
- [`save_data_to_disk`](save_data_to_disk/main.cpp): This example demonstrates how to save aquisition data to disk.
