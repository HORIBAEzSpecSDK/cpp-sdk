#ifndef SPECTRA_STITCH_H
#define SPECTRA_STITCH_H

#include <memory>
#include <vector>

namespace horiba::examples {

class SpectraStitch {
 public:
  virtual std::vector<std::vector<double>> stitched_spectra() = 0;
  virtual std::unique_ptr<SpectraStitch> stitch_with(
      std::unique_ptr<SpectraStitch> other) = 0;
  virtual ~SpectraStitch() = default;
};
}  // namespace horiba::examples

#endif /* ifndef SPECTRA_STITCH_H */
