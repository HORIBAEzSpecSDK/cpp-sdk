#include "raman_shift.h"

#include <algorithm>
#include <iterator>
#include <vector>

namespace horiba::examples {
RamanShift::RamanShift(const std::vector<double>& wavelengths,
                       double exicitation_wavelength)
    : wavelengths{wavelengths},
      excitation_wavelength{exicitation_wavelength},
      raman_shift{} {}

std::vector<double> RamanShift::compute() {
  if (!raman_shift.empty()) {
    return raman_shift;
  }

  std::ranges::transform(
      wavelengths, std::back_inserter(raman_shift), [this](double wavelength) {
        return ((1.0 / excitation_wavelength) - (1.0 / wavelength)) * 1e7;
      });

  return raman_shift;
}

}  // namespace horiba::examples
