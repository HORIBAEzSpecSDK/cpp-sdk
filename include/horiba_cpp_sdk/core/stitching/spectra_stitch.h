#ifndef SPECTRA_STITCH_H
#define SPECTRA_STITCH_H

#include <spdlog/spdlog.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

namespace horiba::core::stitching {

class SpectraStitch {
 public:
  virtual std::vector<std::vector<double>> stitched_spectra() = 0;
  virtual std::unique_ptr<SpectraStitch> stitch_with(
      std::unique_ptr<SpectraStitch> other) = 0;
  virtual ~SpectraStitch() = default;

  void sort_by_wavelength(std::vector<double>& x_wavelength,
                          std::vector<double>& y_intensity) {
    if (x_wavelength.size() != y_intensity.size()) {
      auto message = fmt::format(
          "Invalid spectra format: spectra must have x an y data of the same "
          "size. Got {} and {}",
          x_wavelength.size(), y_intensity.size());
      spdlog::error(message);
      throw std::invalid_argument(message);
    }

    std::vector<std::pair<double, double>> xy(x_wavelength.size());
    for (size_t i = 0; i < x_wavelength.size(); ++i)
      xy[i] = {x_wavelength[i], y_intensity[i]};

    std::sort(xy.begin(), xy.end(),
              [](auto& a, auto& b) { return a.first < b.first; });

    for (size_t i = 0; i < x_wavelength.size(); ++i) {
      x_wavelength[i] = xy[i].first;
      y_intensity[i] = xy[i].second;
    }
  }

  void remove_duplicates(std::vector<double>& x_wavelength,
                         std::vector<double>& y_intensity) {
    if (x_wavelength.size() != y_intensity.size()) {
      auto message = fmt::format(
          "Invalid spectra format: spectra must have x an y data of the same "
          "size. Got {} and {}",
          x_wavelength.size(), y_intensity.size());
      spdlog::error(message);
      throw std::invalid_argument(message);
    }
    if (x_wavelength.empty()) {
      auto message = "Spectra are empty";
      spdlog::warn(message);
      return;
    }

    size_t write_idx = 0;  // index to write next unique element

    for (size_t read_idx = 1; read_idx < x_wavelength.size(); ++read_idx) {
      if (x_wavelength[read_idx] != x_wavelength[write_idx]) {
        ++write_idx;
        x_wavelength[write_idx] = x_wavelength[read_idx];
        y_intensity[write_idx] = y_intensity[read_idx];
      }
    }

    // Resize vectors to remove leftover duplicates
    x_wavelength.resize(write_idx + 1);
    y_intensity.resize(write_idx + 1);
  }
};
}  // namespace horiba::core::stitching

#endif /* ifndef SPECTRA_STITCH_H */
