#ifndef LAB_SPEC_6_SPECTRA_STITCH_H
#define LAB_SPEC_6_SPECTRA_STITCH_H

#include "spectra_stitch.h"

namespace horiba::examples {

class LabSpec6SpectraStitch : public SpectraStitch {
 public:
  explicit LabSpec6SpectraStitch(
      const std::vector<std::vector<std::vector<double>>>& spectra_list);

  std::vector<std::vector<double>> stitched_spectra() override;

  std::unique_ptr<SpectraStitch> stitch_with(
      std::unique_ptr<SpectraStitch> other_stitch) override;

 private:
  std::vector<std::vector<double>> stitched_spectrum;

  std::vector<std::vector<double>> stitch_spectra(
      const std::vector<std::vector<double>>& spectrum1,
      const std::vector<std::vector<double>>& spectrum2);
  double interpolate(double x, const std::vector<double>& xs,
                     const std::vector<double>& ys);
};

} /* namespace horiba::examples */

#endif /* ifndef LAB_SPEC_6_SPECTRA_STITCH_H */
