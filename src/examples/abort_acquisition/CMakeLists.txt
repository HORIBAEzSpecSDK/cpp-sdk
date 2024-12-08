add_executable(abort_acquisition main.cpp)

target_link_libraries(
  abort_acquisition
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Boost::beast)

target_include_directories(abort_acquisition PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
