#include "labspec6_spectra_stitch.h"

#include <spdlog/spdlog.h>

#include <iostream>

namespace horiba::examples {

LabSpec6SpectraStitch::LabSpec6SpectraStitch(
    const std::vector<std::vector<std::vector<double>>>& spectra_list) {
  if (spectra_list.empty()) {
    spdlog::error("No spectra to stitch");
    throw std::runtime_error("No spectra to stitch");
  }

  this->stitched_spectrum = spectra_list[0];
  for (size_t i = 1; i < spectra_list.size(); i++) {
    this->stitched_spectrum =
        stitch_spectra(this->stitched_spectrum, spectra_list[i]);
  }
}
std::vector<std::vector<double>> LabSpec6SpectraStitch::stitched_spectra() {
  return this->stitched_spectrum;
}
std::unique_ptr<SpectraStitch> LabSpec6SpectraStitch::stitch_with(
    std::unique_ptr<SpectraStitch> other_stitch) {
  const std::vector<std::vector<std::vector<double>>> new_spectra_list = {
      this->stitched_spectra(), other_stitch->stitched_spectra()};
  return std::make_unique<LabSpec6SpectraStitch>(new_spectra_list);
}

double LabSpec6SpectraStitch::interpolate(double x,
                                          const std::vector<double>& xs,
                                          const std::vector<double>& ys) {
  auto it = std::lower_bound(xs.begin(), xs.end(), x);
  if (it == xs.end()) {
    return ys.back();
  }
  if (it == xs.begin()) {
    return ys.front();
  }
  size_t i = it - xs.begin();
  double x1 = xs[i - 1], x2 = xs[i];
  double y1 = ys[i - 1], y2 = ys[i];
  return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
}

std::vector<std::vector<double>> LabSpec6SpectraStitch::stitch_spectra(
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

  std::vector<double> x_overlap;
  std::vector<double> y1_overlap;
  std::vector<double> y2_overlap;
  std::vector<double> x_before;
  std::vector<double> y_before;
  std::vector<double> x_after;
  std::vector<double> y_after;

  for (size_t i = 0; i < fx1.size(); i++) {
    if (fx1[i] < overlap_start) {
      x_before.push_back(fx1[i]);
      y_before.push_back(fy1[i]);
    }
    if (fx1[i] >= overlap_start && fx1[i] <= overlap_end) {
      x_overlap.push_back(fx1[i]);
      y1_overlap.push_back(fy1[i]);
    }
  }

  for (size_t i = 0; i < fx2.size(); i++) {
    if (fx2[i] > overlap_end) {
      x_after.push_back(fx2[i]);
      y_after.push_back(fy2[i]);
    }
    if (fx2[i] >= overlap_start && fx2[i] <= overlap_end) {
      if (std::find(x_overlap.begin(), x_overlap.end(), fx2[i]) ==
          x_overlap.end()) {
        x_overlap.push_back(fx2[i]);
      }
    }
  }

  std::sort(x_overlap.begin(), x_overlap.end());
  for (const double x : x_overlap) {
    y1_overlap.push_back(interpolate(x, fx1, fy1));
    y2_overlap.push_back(interpolate(x, fx2, fy2));
  }

  std::vector<double> y_stitched(x_overlap.size());
  for (size_t i = 0; i < x_overlap.size(); i++) {
    const double A =
        (x_overlap[i] - overlap_start) / (overlap_end - overlap_start);
    const double B =
        (overlap_end - x_overlap[i]) / (overlap_end - overlap_start);
    y_stitched[i] = (y1_overlap[i] * A + y2_overlap[i] * B) / (A + B);
  }

  std::vector<double> x_stitched;
  std::vector<double> y_stitched_final;
  x_stitched.insert(x_stitched.end(), x_before.begin(), x_before.end());
  x_stitched.insert(x_stitched.end(), x_overlap.begin(), x_overlap.end());
  x_stitched.insert(x_stitched.end(), x_after.begin(), x_after.end());

  y_stitched_final.insert(y_stitched_final.end(), y_before.begin(),
                          y_before.end());
  y_stitched_final.insert(y_stitched_final.end(), y_stitched.begin(),
                          y_stitched.end());
  y_stitched_final.insert(y_stitched_final.end(), y_after.begin(),
                          y_after.end());

  return {x_stitched, y_stitched_final};
}

}  // namespace horiba::examples
