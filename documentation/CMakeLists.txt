cmake_minimum_required(VERSION 3.21)

project(horiba_cpp_sdk_docs)

# ---- Dependencies ----

include(../cmake/CPM.cmake)

CPMAddPackage("gh:jothepro/doxygen-awesome-css#9760c30014131f4eacb8e96f15f3869c7bc5dd8c")

# ---- Doxygen variables ----

# set Doxyfile variables
set(DOXYGEN_PROJECT_NAME "HORIBA C++ SDK")
set(DOXYGEN_PROJECT_BRIEF "Library for HORIBA devices")
set(DOXYGEN_PROJECT_VERSION ${horiba_cpp_sdk_VERSION})
set(DOXYGEN_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYGEN_SOURCE_IMAGE_PATH "${DOXYGEN_PROJECT_ROOT}/documentation/images")
set(DOXYGEN_PROJECT_LOGO "${DOXYGEN_PROJECT_ROOT}/documentation/images/horiba_logo.svg")
set(DOXYGEN_HTML_HEADER "${DOXYGEN_PROJECT_ROOT}/documentation/doxygen-custom/header.html")

set(DOXYGEN_AWESOME_CSS
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css"
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-sidebar-only.css"
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css"
    "${DOXYGEN_PROJECT_ROOT}/documentation/doxygen-custom/custom.css"
    "${DOXYGEN_PROJECT_ROOT}/documentation/doxygen-custom/custom-alternative.css")
string(
  REPLACE ";"
          " \\\n"
          DOXYGEN_HTML_EXTRA_STYLESHEET
          "${DOXYGEN_AWESOME_CSS}")

set(DOXYGEN_AWESOME_CSS_EXTRA_FILES
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js"
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js"
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-paragraph-link.js"
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-interactive-toc.js"
    "${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-tabs.js"
    "${DOXYGEN_PROJECT_ROOT}/documentation/doxygen-custom/toggle-alternative-theme.js")
string(
  REPLACE ";"
          " \\\n"
          DOXYGEN_HTML_EXTRA_FILES
          "${DOXYGEN_AWESOME_CSS_EXTRA_FILES}")

set(DOXYGEN_SOURCE_INPUT
    "README.md"
    "README_building.md"
    "README_dependencies.md"
    "README_docker.md"
    "src/examples/README_examples.md"
    "documentation/pages/gains_and_speeds.md"
    "documentation/pages/icl_api.md"
    "include")
list(TRANSFORM DOXYGEN_SOURCE_INPUT PREPEND "${DOXYGEN_PROJECT_ROOT}/")
string(
  REPLACE ";"
          " \\\n"
          DOXYGEN_SOURCE_INPUT_STR
          "${DOXYGEN_SOURCE_INPUT}")

configure_file(${CMAKE_CURRENT_LIST_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target(
  GenerateDocs
  ${CMAKE_COMMAND}
  -E
  make_directory
  "${DOXYGEN_OUTPUT_DIRECTORY}"
  COMMAND doxygen "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" "--debug"
  COMMAND echo "Docs written to: ${DOXYGEN_OUTPUT_DIRECTORY}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
