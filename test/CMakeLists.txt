cmake_minimum_required(VERSION 3.15...3.28)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(horiba_cpp_sdk CONFIG REQUIRED) # for intro, project_options, ...

  if(NOT TARGET horiba_cpp_sdk_options)
    message(FATAL_ERROR "Requiered config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

# Provide a simple smoke test to make sure that the CLI works and can display a --help message
# add_test(NAME cli.has_help COMMAND intro --help)

# Provide a test to verify that the version being reported from the application
# matches the version given to CMake. This will be important once you package
# your program. Real world shows that this is the kind of simple mistake that is easy
# to make, but also easy to test for.
# add_test(NAME cli.version_matches COMMAND intro --version)
# set_tests_properties(cli.version_matches PROPERTIES PASS_REGULAR_EXPRESSION "${PROJECT_VERSION}")

add_executable(
  tests
  tests.cpp
  communication/test_command.cpp
  # communication/test_response.cpp
  communication/test_websocket_communicator.cpp
  core/stitching/test_simple_spectra_stitch.cpp
  core/stitching/test_offset_spectra_stitch.cpp
  core/stitching/test_average_spectra_stitch.cpp
  devices/single_devices/test_ccd.cpp
  devices/single_devices/test_ccd_on_hw.cpp
  devices/single_devices/test_mono.cpp
  devices/single_devices/test_mono_on_hw.cpp
  devices/single_devices/test_spectracq3_on_hw.cpp
  devices/test_ccds_discovery.cpp
  devices/test_monos_discovery.cpp
  devices/test_icl_device_manager.cpp)
target_link_libraries(
  tests
  PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_warnings
          horiba_cpp_sdk::horiba_cpp_sdk_options
          horiba_cpp_sdk::horiba_cpp_sdk
          Catch2::Catch2
          Boost::beast
          nlohmann_json::nlohmann_json
          spdlog::spdlog)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS)
endif()

# Copy fake icl responses json files into build directory to support CICD pipeline
set(FAKE_ICL_RESPONSES_DIR_NAME "fake_icl_responses")
set(FAKE_ICL_RESPONSES_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${FAKE_ICL_RESPONSES_DIR_NAME}")
set(FAKE_ICL_RESPONSES_DEST_DIR "${CMAKE_BINARY_DIR}/test/")

# MSVC has a different directory structure for the build directory, with Debug/Release subdirectories
if(MSVC)
  set(FAKE_ICL_RESPONSES_DEST_DIR "${CMAKE_BINARY_DIR}/test/${CMAKE_BUILD_TYPE}/..")
endif()

message(STATUS "Copying fake icl responses from ${FAKE_ICL_RESPONSES_SOURCE_DIR} into ${FAKE_ICL_RESPONSES_DEST_DIR}")
file(
  COPY ${FAKE_ICL_RESPONSES_SOURCE_DIR}
  DESTINATION ${FAKE_ICL_RESPONSES_DEST_DIR}
  FILES_MATCHING
  PATTERN "*.json")

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

# # Add a file containing a set of constexpr tests
# add_executable(constexpr_tests constexpr_tests.cpp)
# target_link_libraries(
#   constexpr_tests
#   PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_warnings
#           horiba_cpp_sdk::horiba_cpp_sdk_options
#           horiba_cpp_sdk::sample_library
#           Catch2::Catch2WithMain)

# catch_discover_tests(
#   constexpr_tests
#   TEST_PREFIX
#   "constexpr."
#   REPORTER
#   XML
#   OUTPUT_DIR
#   .
#   OUTPUT_PREFIX
#   "constexpr."
#   OUTPUT_SUFFIX
#   .xml)

# # Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
# # things go wrong with the constexpr testing
# add_executable(relaxed_constexpr_tests constexpr_tests.cpp)
# target_link_libraries(
#   relaxed_constexpr_tests
#   PRIVATE horiba_cpp_sdk::horiba_cpp_sdk_warnings
#           horiba_cpp_sdk::horiba_cpp_sdk_options
#           horiba_cpp_sdk::sample_library
#           Catch2::Catch2WithMain)
# target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

# catch_discover_tests(
#   relaxed_constexpr_tests
#   TEST_PREFIX
#   "relaxed_constexpr."
#   REPORTER
#   XML
#   OUTPUT_DIR
#   .
#   OUTPUT_PREFIX
#   "relaxed_constexpr."
#   OUTPUT_SUFFIX
#   .xml)
