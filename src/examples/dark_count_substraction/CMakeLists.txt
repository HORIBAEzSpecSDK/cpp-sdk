add_executable(dark_count_substraction main.cpp)

target_link_libraries(
  dark_count_substraction
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Matplot++::matplot
          Boost::beast)

target_include_directories(dark_count_substraction PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
