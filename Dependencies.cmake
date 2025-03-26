include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(horiba_cpp_sdk_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  CPMAddPackage(
    NAME Ccache.cmake
    GITHUB_REPOSITORY TheLartians/Ccache.cmake
    VERSION 1.2)

  CPMAddPackage("gh:TheLartians/Format.cmake@1.8.1")

  if(NOT TARGET spdlog::spdlog)
    CPMAddPackage(
      NAME spdlog
      VERSION 1.14.0
      GITHUB_REPOSITORY "gabime/spdlog")
  endif()

  if(NOT TARGET Catch2::Catch2)
    CPMAddPackage("gh:catchorg/Catch2@3.6.0")
  endif()

  # Used only in the examples
  if(NOT TARGET Matplot++::matplot)
    message(STATUS "Matplot++ not added already, adding it as a CPM package")
    CPMAddPackage("gh:alandefreitas/matplotplusplus@1.2.2")
  endif()

  if(NOT TARGET Boost::beast)
    CPMAddPackage(
      NAME Boost
      VERSION 1.84.0
      URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
      URL_HASH SHA256=2e64e5d79a738d0fa6fb546c6e5c2bd28f88d268a2a080546f74e5ff98f29d0e
      OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_INCLUDE_LIBRARIES static_string\\\;logic\\\;endian\\\;container\\\;asio")

    CPMAddPackage(
      NAME BoostBeast
      VERSION 1.84.0
      URL https://github.com/boostorg/beast/archive/refs/tags/boost-1.84.0.tar.gz
      URL_HASH SHA256=2e3fdd5fb79f23a1f6d239e747af21633f4236983f9e03eb21d2da77453b07de
      OPTIONS "BOOST_ENABLE_CMAKE ON" "BOOST_INCLUDE_LIBRARIES container\\\;asio")
  endif()

  if(NOT TARGET nlohmann_json::nlohmann_json)
    CPMAddPackage("gh:nlohmann/json@3.11.3")
  endif()
endfunction()
