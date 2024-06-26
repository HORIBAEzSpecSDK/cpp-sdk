#include "horiba_cpp_sdk/communication/response.h"

#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

namespace horiba::communication {
Response::Response(unsigned long long int id, std::string command,
                   nlohmann::json::object_t results,
                   std::vector<std::string> errors)
    : id{id},
      command{std::move(command)},
      results{std::move(results)},
      icl_errors{std::move(errors)} {}

nlohmann::json Response::json_results() const { return this->results; }

std::vector<std::string> Response::errors() const { return this->icl_errors; }
} /* namespace horiba::communication */
