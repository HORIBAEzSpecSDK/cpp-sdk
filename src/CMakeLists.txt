include(GenerateExportHeader)

set(HORIBA_CPP_LIB_SOURCES
    communication/command.cpp
    communication/response.cpp
    communication/websocket_communicator.cpp
    devices/ccds_discovery.cpp
    devices/icl_device_manager.cpp
    devices/monos_discovery.cpp
    devices/single_devices/ccd.cpp
    devices/single_devices/device.cpp
    devices/single_devices/mono.cpp
    devices/single_devices/spectracq3.cpp
    devices/spectracq3s_discovery.cpp)

set(HORIBA_CPP_LIB_HEADERS
    include/horiba_cpp_sdk/communication/command.h
    include/horiba_cpp_sdk/communication/communicator.h
    include/horiba_cpp_sdk/communication/response.h
    include/horiba_cpp_sdk/communication/websocket_communicator.h
    include/horiba_cpp_sdk/devices/ccds_discovery.h
    include/horiba_cpp_sdk/devices/device_discovery.h
    include/horiba_cpp_sdk/devices/device_manager.h
    include/horiba_cpp_sdk/devices/icl_device_manager.h
    include/horiba_cpp_sdk/devices/monos_discovery.h
    include/horiba_cpp_sdk/devices/spectracq3s_discovery.h
    include/horiba_cpp_sdk/devices/single_devices/ccd.h
    include/horiba_cpp_sdk/devices/single_devices/device.h
    include/horiba_cpp_sdk/devices/single_devices/mono.h
    include/horiba_cpp_sdk/devices/single_devices/spectracq3.h
    include/horiba_cpp_sdk/os/process.h)

# Platform specific code
if(WIN32)
  list(APPEND HORIBA_CPP_LIB_SOURCES os/windows_process.cpp)
  list(APPEND HORIBA_CPP_LIB_HEADERS include/horiba_cpp_sdk/os/windows_process.h)
endif()

list(TRANSFORM HORIBA_CPP_LIB_HEADERS PREPEND "${PROJECT_SOURCE_DIR}/")

add_library(horiba_cpp_sdk ${HORIBA_CPP_LIB_HEADERS} ${HORIBA_CPP_LIB_SOURCES})

add_library(horiba_cpp_sdk::horiba_cpp_sdk ALIAS horiba_cpp_sdk)

target_link_libraries(
  horiba_cpp_sdk
  PRIVATE horiba_cpp_sdk_options horiba_cpp_sdk_warnings
  PUBLIC Boost::beast nlohmann_json::nlohmann_json spdlog::spdlog)

target_include_directories(horiba_cpp_sdk ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(horiba_cpp_sdk PUBLIC cxx_std_20)

set_target_properties(
  horiba_cpp_sdk
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(horiba_cpp_sdk EXPORT_FILE_NAME
                       ${PROJECT_BINARY_DIR}/include/horiba_cpp_sdk/horiba_cpp_sdk_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(horiba_cpp_sdk PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()

add_subdirectory(examples)
