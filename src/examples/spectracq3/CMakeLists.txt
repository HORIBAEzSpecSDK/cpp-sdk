add_executable(spectracq3 main.cpp)

target_link_libraries(
  spectracq3
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Matplot++::matplot
          Boost::beast)

target_include_directories(spectracq3 PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
