#include "simple_cut_spectra_stitch.h"

#include <spdlog/spdlog.h>

namespace horiba::examples {

SimpleCutSpectraStitch::SimpleCutSpectraStitch(
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

std::vector<std::vector<double>> SimpleCutSpectraStitch::stitched_spectra() {
  return this->stitched_spectrum;
}

std::unique_ptr<SpectraStitch> SimpleCutSpectraStitch::stitch_with(
    std::unique_ptr<SpectraStitch> other_stitch) {
  const std::vector<std::vector<std::vector<double>>> new_spectra_list = {
      this->stitched_spectra(), other_stitch->stitched_spectra()};
  return std::make_unique<SimpleCutSpectraStitch>(new_spectra_list);
}

std::vector<std::vector<double>> SimpleCutSpectraStitch::stitch_spectra(
    const std::vector<std::vector<double>>& spectrum1,
    const std::vector<std::vector<double>>& spectrum2) {
  const std::vector<double>& fx1 = spectrum1[0];
  const std::vector<double>& fy1 = spectrum1[1];
  const std::vector<double>& fx2 = spectrum2[0];
  const std::vector<double>& fy2 = spectrum2[1];

  double overlap_start = std::max(fx1.front(), fx2.front());
  double overlap_end = std::min(fx1.back(), fx2.back());

  if (overlap_start >= overlap_end) {
    spdlog::error("No overlap between two spectra");
    throw std::runtime_error("No overlapping region between spectra");
  }

  std::vector<double> x2_overlap;
  std::vector<double> y2_overlap;
  std::vector<double> x1_before_overlap;
  std::vector<double> y1_before_overlap;
  std::vector<double> x2_after_overlap;
  std::vector<double> y2_after_overlap;

  for (size_t i = 0; i < fx2.size(); i++) {
    if (fx2[i] >= overlap_start && fx2[i] <= overlap_end) {
      x2_overlap.push_back(fx2[i]);
      y2_overlap.push_back(fy2[i]);
    }
    if (fx2[i] > overlap_end) {
      x2_after_overlap.push_back(fx2[i]);
      y2_after_overlap.push_back(fy2[i]);
    }
  }

  for (size_t i = 0; i < fx1.size(); i++) {
    if (fx1[i] < overlap_start) {
      x1_before_overlap.push_back(fx1[i]);
      y1_before_overlap.push_back(fy1[i]);
    }
  }

  std::vector<double> x_stitched;
  std::vector<double> y_stitched;
  x_stitched.insert(x_stitched.end(), x1_before_overlap.begin(),
                    x1_before_overlap.end());
  x_stitched.insert(x_stitched.end(), x2_overlap.begin(), x2_overlap.end());
  x_stitched.insert(x_stitched.end(), x2_after_overlap.begin(),
                    x2_after_overlap.end());

  y_stitched.insert(y_stitched.end(), y1_before_overlap.begin(),
                    y1_before_overlap.end());
  y_stitched.insert(y_stitched.end(), y2_overlap.begin(), y2_overlap.end());
  y_stitched.insert(y_stitched.end(), y2_after_overlap.begin(),
                    y2_after_overlap.end());

  return {x_stitched, y_stitched};
}
}  // namespace horiba::examples
