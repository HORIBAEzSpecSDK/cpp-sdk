#ifndef RAMAN_SHIFT_H
#define RAMAN_SHIFT_H

#include <memory>
#include <vector>

namespace horiba::examples {

/**
 * @brief Computes the Raman shift for a given set of wavelengths and an
 * excitation wavelength.
 */
class RamanShift {
 public:
  explicit RamanShift(const std::vector<double>& wavelengths,
                      double exicitation_wavelength);

  /**
   * @brief Returns the Raman shift for the given wavelengths in cm^-1.
   *
   * @return raman shift in cm^-1
   */
  std::vector<double> compute();

 private:
  std::vector<double> wavelengths;
  double excitation_wavelength;
  std::vector<double> raman_shift;
};

} /* namespace horiba::examples */

#endif /* ifndef RAMAN_SHIFT_H */
