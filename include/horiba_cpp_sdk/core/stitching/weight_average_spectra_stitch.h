#ifndef WEIGHT_AVERAGE_SPECTRA_STITCH_H
#define WEIGHT_AVERAGE_SPECTRA_STITCH_H

#include <memory>
#include <vector>

#include "spectra_stitch.h"

namespace horiba::core::stitching {

class WeightAverageSpectraStitch : public SpectraStitch {
 public:
  explicit WeightAverageSpectraStitch(
      const std::vector<std::vector<std::vector<double>>>& spectra_list);

  std::vector<std::vector<double>> stitched_spectra() override;

  std::unique_ptr<SpectraStitch> stitch_with(
      std::unique_ptr<SpectraStitch> other_stitch) override;

 private:
  std::vector<std::vector<double>> stitched_spectrum;

  std::vector<std::vector<double>> stitch_spectra(
      const std::vector<std::vector<double>>& spectrum1,
      const std::vector<std::vector<double>>& spectrum2);

  size_t find_closest_index(const std::vector<double>& fx2, double x1);
  std::optional<double> find_lower_bound_B(const std::vector<double>& x1,
                                           const std::vector<double>& x2);
  std::optional<double> find_upper_bound_C(const std::vector<double>& x1,
                                           const std::vector<double>& x2);
  double weight_average(double x1, double y1, double x2, double y2, double B,
                        double C);
  void append_weight_average(size_t i, double B, double C,
                             const std::vector<double>& x1,
                             const std::vector<double>& y1,
                             const std::vector<double>& x2,
                             const std::vector<double>& y2,
                             std::vector<double>& x_out,
                             std::vector<double>& y_out);
};

}  // namespace horiba::core::stitching

#endif /* ifndef WEIGHT_AVERAGE_SPECTRA_STITCH_H */
