#include "horiba_cpp_sdk/communication/command.h"

#include <atomic>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>

namespace horiba::communication {

std::atomic<unsigned long long int> Command::next_id{0};

Command::Command(std::string command, nlohmann::json parameters)
    : id{this->next_id++},
      command{std::move(command)},
      // we cannot use braces {} here, as the library transforms the json into a
      // list. see:
      // https://json.nlohmann.me/home/faq/#brace-initialization-yields-arrays
      parameters(std::move(parameters)) {}

nlohmann::json Command::json() const {
  return {{"id", this->id},
          {"command", this->command},
          {"parameters", this->parameters}};
}
} /* namespace horiba::communication */
