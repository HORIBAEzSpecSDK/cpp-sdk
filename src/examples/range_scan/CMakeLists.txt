set(SOURCES
    labspec6_spectra_stitch.cpp
    labspec6_spectra_stitch.h
    linear_spectra_stitch.cpp
    linear_spectra_stitch.h
    main.cpp
    simple_cut_spectra_stitch.cpp
    simple_cut_spectra_stitch.h
    spectra_stitch.h)

add_executable(range_scan ${SOURCES})

target_link_libraries(
  range_scan
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk
          nlohmann_json::nlohmann_json
          Matplot++::matplot
          Boost::beast)

target_include_directories(center_scan PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")
