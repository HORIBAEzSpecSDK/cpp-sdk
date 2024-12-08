set(SOURCES raman_shift.h raman_shift.cpp main.cpp)

add_executable(raman_shift ${SOURCES})

target_link_libraries(
  raman_shift
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Matplot++::matplot
          Boost::beast)

target_include_directories(raman_shift PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
