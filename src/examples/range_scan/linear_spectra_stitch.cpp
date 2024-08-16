#include "linear_spectra_stitch.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace horiba::examples {

LinearSpectraStitch::LinearSpectraStitch(
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

std::vector<std::vector<double>> LinearSpectraStitch::stitched_spectra() {
  return this->stitched_spectrum;
}

std::unique_ptr<SpectraStitch> LinearSpectraStitch::stitch_with(
    std::unique_ptr<SpectraStitch> other_stitch) {
  const std::vector<std::vector<std::vector<double>>> new_spectra_list = {
      this->stitched_spectra(), other_stitch->stitched_spectra()};
  return std::make_unique<LinearSpectraStitch>(new_spectra_list);
}

std::vector<std::vector<double>> LinearSpectraStitch::stitch_spectra(
    const std::vector<std::vector<double>>& spectrum1,
    const std::vector<std::vector<double>>& spectrum2) {
  const std::vector<double>& fx1 = spectrum1[0];
  const std::vector<double>& fy1 = spectrum1[1];
  const std::vector<double>& fx2 = spectrum2[0];
  const std::vector<double>& fy2 = spectrum2[1];

  const double overlap_start = std::max(fx1.front(), fx2.front());
  const double overlap_end = std::min(fx1.back(), fx2.back());

  if (overlap_start >= overlap_end) {
    spdlog::error("No overlap between two spectra");
    throw std::runtime_error("No overlapping region between spectra");
  }

  auto mask1_start = std::lower_bound(fx1.begin(), fx1.end(), overlap_start);
  auto mask1_end = std::upper_bound(fx1.begin(), fx1.end(), overlap_end);

  auto mask2_start = std::lower_bound(fx2.begin(), fx2.end(), overlap_start);
  auto mask2_end = std::upper_bound(fx2.begin(), fx2.end(), overlap_end);

  std::vector<double> x1_overlap(mask1_start, mask1_end);
  std::vector<double> y1_overlap(fy1.begin() + (mask1_start - fx1.begin()),
                                 fy1.begin() + (mask1_end - fx1.begin()));

  const std::vector<double> x2_overlap(mask2_start, mask2_end);
  std::vector<double> y2_interp;

  for (auto x : x1_overlap) {
    auto it = std::lower_bound(mask2_start, mask2_end, x);
    const size_t index = std::distance(mask2_start, it);
    y2_interp.push_back(fy2[index]);
  }

  std::vector<double> y_combined_overlap;
  for (size_t i = 0; i < y1_overlap.size(); ++i) {
    y_combined_overlap.push_back((y1_overlap[i] + y2_interp[i]) / 2.0);
  }

  std::vector<double> x_combined;
  x_combined.insert(x_combined.end(), fx1.begin(), mask1_start);
  x_combined.insert(x_combined.end(), x1_overlap.begin(), x1_overlap.end());
  x_combined.insert(x_combined.end(), mask2_end, fx2.end());

  std::vector<double> y_combined;
  y_combined.insert(y_combined.end(), fy1.begin(),
                    fy1.begin() + std::distance(fx1.begin(), mask1_start));
  y_combined.insert(y_combined.end(), y_combined_overlap.begin(),
                    y_combined_overlap.end());
  y_combined.insert(y_combined.end(),
                    fy2.begin() + std::distance(mask2_start, mask2_end),
                    fy2.end());

  std::vector<size_t> sort_indices(x_combined.size());
  std::iota(sort_indices.begin(), sort_indices.end(), 0);

  std::sort(sort_indices.begin(), sort_indices.end(),
            [&](size_t i, size_t j) { return x_combined[i] < x_combined[j]; });

  std::vector<double> x_sorted(x_combined.size());
  std::vector<double> y_sorted(y_combined.size());

  for (size_t i = 0; i < sort_indices.size(); ++i) {
    x_sorted[i] = x_combined[sort_indices[i]];
    y_sorted[i] = y_combined[sort_indices[i]];
  }

  return {x_sorted, y_sorted};
}
}  // namespace horiba::examples
