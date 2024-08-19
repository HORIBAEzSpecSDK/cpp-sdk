add_executable(trigger main.cpp)

target_link_libraries(
  trigger
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Boost::beast
          spdlog::spdlog)

target_include_directories(gain_speed PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
