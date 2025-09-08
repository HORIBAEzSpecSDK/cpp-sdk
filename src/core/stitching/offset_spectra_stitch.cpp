#include <horiba_cpp_sdk/core/stitching/offset_spectra_stitch.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <stdexcept>

namespace horiba::core::stitching {

OffsetSpectraStitch::OffsetSpectraStitch(
    const std::vector<std::vector<std::vector<double>>>& spectra_list,
    std::optional<double> offset)
    : spectrum_offset(offset) {
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

std::vector<std::vector<double>> OffsetSpectraStitch::stitched_spectra() {
  return this->stitched_spectrum;
}

std::unique_ptr<SpectraStitch> OffsetSpectraStitch::stitch_with(
    std::unique_ptr<SpectraStitch> other_stitch) {
  const std::vector<std::vector<std::vector<double>>> new_spectra_list = {
      this->stitched_spectra(), other_stitch->stitched_spectra()};
  return std::make_unique<OffsetSpectraStitch>(new_spectra_list);
}

std::vector<std::vector<double>> OffsetSpectraStitch::stitch_spectra(
    const std::vector<std::vector<double>>& spectrum1,
    const std::vector<std::vector<double>>& spectrum2) {
  if (spectrum1.size() != 2 || spectrum2.size() != 2) {
    auto message = fmt::format(
        "Invalid spectra format: spectra must have x an y data. Got {} and {}",
        spectrum1.size(), spectrum2.size());
    spdlog::error(message);
    throw std::invalid_argument(message);
  }

  std::vector<double> fx1 = spectrum1[0];
  std::vector<double> fy1 = spectrum1[1];
  std::vector<double> fx2 = spectrum2[0];
  std::vector<double> fy2 = spectrum2[1];

  // handle if spectras are in the wrong order
  if (fx1.back() > fx2.back() && fx1.front() > fx2.front()) {
    spdlog::debug(
        "Spectra are in the wrong order, swapping them for stitching");
    return stitch_spectra(spectrum2, spectrum1);
  }

  sort_by_wavelength(fx1, fy1);
  remove_duplicates(fx1, fy1);

  sort_by_wavelength(fx2, fy2);
  remove_duplicates(fx2, fy2);

  if (spectrum_offset.has_value()) {
    double offset = spectrum_offset.value();
    spdlog::debug("Applying offset of {} to second spectrum", offset);
    std::for_each(fy2.begin(), fy2.end(),
                  [offset](double& y_intensity) { y_intensity += offset; });
  }

  // check if overlap exists, if not just append and return
  if (fx1.back() < fx2.front()) {
    std::vector<double> x_combined;
    std::vector<double> y_combined;
    x_combined.reserve(fx1.size() + fx2.size());
    y_combined.reserve(fy1.size() + fy2.size());
    x_combined.insert(x_combined.end(), fx1.begin(), fx1.end());
    x_combined.insert(x_combined.end(), fx2.begin(), fx2.end());
    y_combined.insert(y_combined.end(), fy1.begin(), fy1.end());
    y_combined.insert(y_combined.end(), fy2.begin(), fy2.end());
    return {x_combined, y_combined};
  }

  // compute upper bound C
  auto upper_bound_C = std::upper_bound(fx2.begin(), fx2.end(), fx1.back());
  if (upper_bound_C == fx2.end()) {
    return {fx1, fy1};
  }

  // append to combined first spectrum then second spectrum from C to end
  auto remaining_amount_in_spectrum2 = std::distance(upper_bound_C, fx2.end());

  std::vector<double> x_combined;
  x_combined.reserve(fx1.size() + remaining_amount_in_spectrum2);
  std::vector<double> y_combined;
  y_combined.reserve(fy1.size() + remaining_amount_in_spectrum2);

  x_combined.insert(x_combined.end(), fx1.begin(), fx1.end());
  y_combined.insert(y_combined.end(), fy1.begin(), fy1.end());
  x_combined.insert(x_combined.end(), upper_bound_C, fx2.end());
  y_combined.insert(y_combined.end(),
                    fy2.begin() + (fy2.size() - remaining_amount_in_spectrum2),
                    fy2.end());

  return {x_combined, y_combined};
}
}  // namespace horiba::core::stitching
