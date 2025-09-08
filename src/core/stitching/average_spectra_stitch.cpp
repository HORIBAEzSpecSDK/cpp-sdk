#include <horiba_cpp_sdk/core/stitching/average_spectra_stitch.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <iterator>
#include <stdexcept>

namespace horiba::core::stitching {

AverageSpectraStitch::AverageSpectraStitch(
    const std::vector<std::vector<std::vector<double>>>& spectra_list) {
  if (spectra_list.empty()) {
    spdlog::error("No spectra to stitch");
    throw std::runtime_error("No spectra to stitch");
  }

  this->stitched_spectrum = spectra_list[0];
  for (size_t i = 1; i < spectra_list.size(); i++) {
    this->stitched_spectrum =
        stitch_spectra(stitched_spectrum, spectra_list[i]);
  }
}

std::vector<std::vector<double>> AverageSpectraStitch::stitched_spectra() {
  return this->stitched_spectrum;
}

std::unique_ptr<SpectraStitch> AverageSpectraStitch::stitch_with(
    std::unique_ptr<SpectraStitch> other_stitch) {
  const std::vector<std::vector<std::vector<double>>> new_spectra_list = {
      this->stitched_spectra(), other_stitch->stitched_spectra()};
  return std::make_unique<AverageSpectraStitch>(new_spectra_list);
}

size_t AverageSpectraStitch::find_closest_index(const std::vector<double>& fx2,
                                                double x1) {
  auto it = std::lower_bound(fx2.begin(), fx2.end(), x1);
  if (it == fx2.end()) {
    return fx2.size() - 1;
  }
  size_t j = std::distance(fx2.begin(), it);
  if (j > 0 && std::abs(fx2[j - 1] - x1) < std::abs(fx2[j] - x1)) {
    return j - 1;
  }
  return j;
}

std::optional<double> AverageSpectraStitch::find_lower_bound_B(
    const std::vector<double>& x1, const std::vector<double>& x2) {
  auto it = std::find_if(x1.begin(), x1.end(),
                         [&](double v) { return v >= x2.front(); });
  if (it == x1.begin() || it == x1.end()) {
    spdlog::warn(
        "No valid lower bound B found: all x1 values are >= first x2 value");
    return std::nullopt;
  }
  it--;

  return *it;
}

std::optional<double> AverageSpectraStitch::find_upper_bound_C(
    const std::vector<double>& x1, const std::vector<double>& x2) {
  auto it = std::find_if(x2.begin(), x2.end(),
                         [&](double v) { return v > x1.back(); });
  if (it == x2.end()) {
    spdlog::warn(
        "No valid upper bound C found: all x2 values are <= last x1 value");
    return std::nullopt;
  }

  return *it;
}

double AverageSpectraStitch::average(double a, double b) {
  return (a + b) / 2.0;
}

void AverageSpectraStitch::append_average(
    size_t i, const std::vector<double>& x1, const std::vector<double>& y1,
    const std::vector<double>& x2, const std::vector<double>& y2,
    std::vector<double>& x_out, std::vector<double>& y_out) {
  size_t j = find_closest_index(x2, x1[i]);
  double avg_y = average(y1[i], y2[j]);
  x_out.push_back(x1[i]);
  y_out.push_back(avg_y);
}

std::vector<std::vector<double>> AverageSpectraStitch::stitch_spectra(
    const std::vector<std::vector<double>>& spectrum1,
    const std::vector<std::vector<double>>& spectrum2) {
  if (spectrum1.size() != 2 || spectrum2.size() != 2) {
    auto message = fmt::format(
        "Invalid spectra format: spectra must have x and y data. Got {} and {}",
        spectrum1.size(), spectrum2.size());
    spdlog::error(message);
    throw std::invalid_argument(message);
  }

  std::vector<double> x1 = spectrum1[0];
  std::vector<double> y1 = spectrum1[1];
  std::vector<double> x2 = spectrum2[0];
  std::vector<double> y2 = spectrum2[1];

  if (x1.front() > x2.front()) {
    spdlog::debug("Spectra swapped for stitching (ensure left-first)");
    return stitch_spectra(spectrum2, spectrum1);
  }

  sort_by_wavelength(x1, y1);
  remove_duplicates(x1, y1);
  sort_by_wavelength(x2, y2);
  remove_duplicates(x2, y2);

  // No overlap: just concatenate
  if (x1.back() < x2.front()) {
    std::vector<double> x(x1);
    std::vector<double> y(y1);
    x.insert(x.end(), x2.begin(), x2.end());
    y.insert(y.end(), y2.begin(), y2.end());
    return {x, y};
  }

  // Define overlap bounds B and C:
  // - define the lower bound B as the longest wavelength point in the left
  // spectrum that is less than the minimum wavelength of the right spectrum.
  // - define upper bound C as the shortest wavelength point in the right
  //  spectrum that is larger than the maximum wavelength of the left spectrum.
  spdlog::debug("Computing lower bound B and upper bound C of overlap");
  auto B = find_lower_bound_B(x1, x2);
  auto C = find_upper_bound_C(x1, x2);

  std::vector<double> x_out;
  std::vector<double> y_out;

  // 1) Left part (<= B) only if B exists
  if (B.has_value()) {
    for (size_t i = 0; i < x1.size(); ++i) {
      if (x1[i] <= B) {
        x_out.push_back(x1[i]);
        y_out.push_back(y1[i]);
      }
    }
  }

  // 2) Overlap zone: average
  // Four cases to consider:
  // - no B and no C: spectra fully overlap with start and end the same in both
  // - B exists and no C: right spectrum fully inside left spectrum with left
  // tail
  // - no B and C exists: left spectrum fully inside right spectrum with right
  // tail
  // - both B and C exist: partial overlap
  if (!B.has_value() && !C.has_value()) {
    for (size_t i = 0; i < x1.size(); ++i) {
      append_average(i, x1, y1, x2, y2, x_out, y_out);
    }
  } else if (B.has_value() && !C.has_value()) {
    for (size_t i = 0; i < x1.size(); ++i) {
      if (x1[i] > B && x1[i] <= x2.back()) {
        append_average(i, x1, y1, x2, y2, x_out, y_out);
      }
    }
  } else if (!B.has_value() && C.has_value()) {
    for (size_t i = 0; i < x1.size(); ++i) {
      if (x1[i] < C) {
        append_average(i, x1, y1, x2, y2, x_out, y_out);
      }
    }

  } else {
    for (size_t i = 0; i < x1.size(); ++i) {
      if (x1[i] > B && x1[i] < C) {
        append_average(i, x1, y1, x2, y2, x_out, y_out);
      }
    }
  }

  // 3) Right part (>= C) only if C exists
  if (C.has_value()) {
    for (size_t j = 0; j < x2.size(); ++j) {
      if (x2[j] >= C) {
        x_out.push_back(x2[j]);
        y_out.push_back(y2[j]);
      }
    }
  } else {
    // no C: append right tail of left spectrum
    for (size_t i = 0; i < x1.size(); ++i) {
      if (x1[i] > x2.back()) {
        x_out.push_back(x1[i]);
        y_out.push_back(y1[i]);
      }
    }
  }
  return {x_out, y_out};
}
}  // namespace horiba::core::stitching
