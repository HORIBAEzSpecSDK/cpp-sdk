#ifndef SIMPLE_SPECTRA_STITCH_H
#define SIMPLE_SPECTRA_STITCH_H

#include <memory>
#include <vector>

#include "offset_spectra_stitch.h"
#include "spectra_stitch.h"

namespace horiba::core::stitching {

class SimpleSpectraStitch : public SpectraStitch {
 public:
  explicit SimpleSpectraStitch(
      const std::vector<std::vector<std::vector<double>>>& spectra_list);

  std::vector<std::vector<double>> stitched_spectra() override;

  std::unique_ptr<SpectraStitch> stitch_with(
      std::unique_ptr<SpectraStitch> other_stitch) override;

 private:
  std::unique_ptr<OffsetSpectraStitch> offset_stitch;
};

}  // namespace horiba::core::stitching

#endif /* ifndef SIMPLE_SPECTRA_STITCH_H */
