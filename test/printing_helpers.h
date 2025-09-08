#ifndef PRINTING_HELPERS_H
#define PRINTING_HELPERS_H

#include <spdlog/spdlog.h>

#include <string>
#include <vector>

namespace horiba::test {

inline void print_vector(const std::vector<double>& vec,
                         const std::string& name) {
  spdlog::debug("{}: ", name);

  std::ostringstream ss;
  for (const auto& val : vec) {
    ss << val << " ";
  }

  spdlog::debug("{}", ss.str());
}
}  // namespace horiba::test
   //
#endif /* ifndef PRINTING_HELPERS_H */
