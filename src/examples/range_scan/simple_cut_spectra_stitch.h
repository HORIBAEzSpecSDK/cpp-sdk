#ifndef SIMPLE_CUT_SPECTRA_STITCH_H
#define SIMPLE_CUT_SPECTRA_STITCH_H

#include <memory>
#include <vector>

#include "spectra_stitch.h"

namespace horiba::examples {

class SimpleCutSpectraStitch : public SpectraStitch {
 public:
  explicit SimpleCutSpectraStitch(
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

#endif /* ifndef SIMPLE_CUT_SPECTRA_STITCH_H */
