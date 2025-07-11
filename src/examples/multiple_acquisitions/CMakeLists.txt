add_executable(multiple_acquisitions main.cpp)

target_link_libraries(
  multiple_acquisitions
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Boost::beast)

target_include_directories(multiple_acquisitions PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
