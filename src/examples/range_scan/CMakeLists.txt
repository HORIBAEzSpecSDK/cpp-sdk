set(SOURCES
    spectra_stitch.h
    labspec6_spectra_stitch.h
    labspec6_spectra_stitch.cpp
    linear_spectra_stitch.h
    linear_spectra_stitch.cpp
    simple_cut_spectra_stitch.h
    simple_cut_spectra_stitch.cpp
    main.cpp)

add_executable(range_scan ${SOURCES})

target_link_libraries(
  range_scan
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Matplot++::matplot
          Boost::beast)

target_include_directories(range_scan PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
