# Usage Examples

This folder contains various examples of how to use the `horiba_cpp_sdk` library.

## Examples

Acquisition examples:
- [`center_scan`](center_scan/main.cpp): This example demonstrates how to perform a center scan using the
  `horiba_cpp_sdk` library. Before running this example, set the environement variable `GNUTERM` to `qt` to avoid issues
  with the `gnuplot` library.
- [`range_scan`](range_scan/main.cpp): This example demonstrates how to perform a range scan using the
  `horiba_cpp_sdk` library including stitching of spectra. Before running this example, set the environement variable
  `GNUTERM` to `qt` to avoid issues with the `gnuplot` library.
- [`dark_count_subtraction`](dark_count_subtraction/main.cpp): This example demonstrates how to perform a
  dark count subtraction. Before running this example, set the environement variable
  `GNUTERM` to `qt` to avoid issues with the `gnuplot` library.
- [`abort_acquisition`](abort_acquisition/main.cpp): This example demonstrates how to abort a running acquisition.
- [`raman_shift`](raman_shift/main.cpp): This example demonstrates how to compute the raman shift of a spectrum.

Configuration examples:
- [`gain_speed_info`](gain_speed_info/main.cpp): This example demonstrates how to create your own gain and speed enums based on
  the device's capabilities.
- [`trigger`](trigger/main.cpp): This example demonstrates how to set a custom trigger.
