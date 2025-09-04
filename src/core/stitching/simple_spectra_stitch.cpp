#include <horiba_cpp_sdk/core/stitching/simple_spectra_stitch.h>
#include <spdlog/spdlog.h>

namespace horiba::core::stitching {

SimpleSpectraStitch::SimpleSpectraStitch(
    const std::vector<std::vector<std::vector<double>>>& spectra_list) {
  offset_stitch =
      std::make_unique<OffsetSpectraStitch>(spectra_list, std::nullopt);
}

std::vector<std::vector<double>> SimpleSpectraStitch::stitched_spectra() {
  return this->offset_stitch->stitched_spectra();
}

std::unique_ptr<SpectraStitch> SimpleSpectraStitch::stitch_with(
    std::unique_ptr<SpectraStitch> other_stitch) {
  auto stitched = offset_stitch->stitch_with(std::move(other_stitch));
  return std::make_unique<SimpleSpectraStitch>(
      std::vector<std::vector<std::vector<double>>>{
          this->stitched_spectra(), stitched->stitched_spectra()});
}

}  // namespace horiba::core::stitching
