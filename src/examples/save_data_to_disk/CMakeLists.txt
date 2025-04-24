add_executable(save_data_to_disk main.cpp)

target_link_libraries(
  save_data_to_disk
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          )

target_include_directories(save_data_to_disk PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
