#ifndef LINEAR_SPECTRA_STITCH_H
#define LINEAR_SPECTRA_STITCH_H

#include "spectra_stitch.h"

namespace horiba::examples {

class LinearSpectraStitch : public SpectraStitch {
 public:
  explicit LinearSpectraStitch(
      const std::vector<std::vector<std::vector<double>>>& spectra_list);

  std::vector<std::vector<double>> stitched_spectra() override;

  std::unique_ptr<SpectraStitch> stitch_with(
      std::unique_ptr<SpectraStitch> other_stitch) override;

 private:
  std::vector<std::vector<double>> stitched_spectrum;

  std::vector<std::vector<double>> stitch_spectra(
      const std::vector<std::vector<double>>& spectrum1,
      const std::vector<std::vector<double>>& spectrum2);
};

} /* namespace horiba::examples */

#endif /* ifndef LINEAR_SPECTRA_STITCH_H */
