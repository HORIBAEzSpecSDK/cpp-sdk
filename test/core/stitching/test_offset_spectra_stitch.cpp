#include <horiba_cpp_sdk/core/stitching/offset_spectra_stitch.h>
#include <spdlog/spdlog.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <string>
#include <vector>

#include "../../printing_helpers.h"

namespace horiba::test {

using namespace horiba::core::stitching;

TEST_CASE("Test Offset Spectra Stitching", "[offset_spectra_stitch]") {
  // arrange
  const auto PRECISION = 1e-8;

  SECTION("Simple overlapping spectra") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.0, 1.0, 2.0}, {10.0, 20.0, 30.0}};
    std::vector<std::vector<double>> s2 = {{1.5, 2.0, 3.0},
                                           {100.0, 200.0, 300.0}};
    std::vector<double> expected_x = {0.0, 1.0, 2.0, 3.0};
    std::vector<double> expected_y = {10.0, 20.0, 30.0, 300.1};
    double offset = 0.1;

    // act
    OffsetSpectraStitch stitch({s1, s2}, offset);
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE_THAT(res[0][i],
                   Catch::Matchers::WithinRel(expected_x[i], PRECISION));
      REQUIRE_THAT(res[1][i],
                   Catch::Matchers::WithinRel(expected_y[i], PRECISION));
    }
  }

  SECTION("Very close double values") {
    // arrange
    std::vector<std::vector<double>> s1 = {{1.000001, 2.000001}, {10.0, 20.0}};
    std::vector<std::vector<double>> s2 = {{2.0000001, 3.0}, {100.0, 200.0}};
    std::vector<double> expected_x = {1.000001, 2.000001, 3.0};
    std::vector<double> expected_y = {10.0, 20.0, 200.1};
    double offset = 0.1;

    // act
    OffsetSpectraStitch stitch({s1, s2}, offset);
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE_THAT(res[0][i],
                   Catch::Matchers::WithinRel(expected_x[i], PRECISION));
      REQUIRE_THAT(res[1][i],
                   Catch::Matchers::WithinRel(expected_y[i], PRECISION));
    }
  }

  SECTION("Negative values") {
    // arrange
    std::vector<std::vector<double>> s1 = {{-3.0, -2.0, -1.0},
                                           {-30.0, -20.0, -10.0}};
    std::vector<std::vector<double>> s2 = {{-2.5, -1.5, 0.0},
                                           {-300.0, -200.0, 0.0}};
    std::vector<double> expected_x = {-3.0, -2.0, -1.0, 0.0};
    std::vector<double> expected_y = {-30.0, -20.0, -10.0, 0.1};
    double offset = 0.1;

    // act
    OffsetSpectraStitch stitch({s1, s2}, offset);
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE_THAT(res[0][i],
                   Catch::Matchers::WithinRel(expected_x[i], PRECISION));
      REQUIRE_THAT(res[1][i],
                   Catch::Matchers::WithinRel(expected_y[i], PRECISION));
    }
  }

  SECTION("Overlapping with very small differences") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.0, 0.000001, 0.000002},
                                           {1.0, 2.0, 3.0}};
    std::vector<std::vector<double>> s2 = {{0.0000015, 0.0000025},
                                           {100.0, 200.0}};
    std::vector<double> expected_x = {0.0, 0.000001, 0.000002, 0.0000025};
    std::vector<double> expected_y = {1.0, 2.0, 3.0, 200.11};
    double offset = 0.11;

    // act
    OffsetSpectraStitch stitch({s1, s2}, offset);
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE_THAT(res[0][i],
                   Catch::Matchers::WithinRel(expected_x[i], PRECISION));
      REQUIRE_THAT(res[1][i],
                   Catch::Matchers::WithinRel(expected_y[i], PRECISION));
    }
  }

  SECTION("Single-point overlap") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.1, 0.2}, {5.0, 10.0}};
    std::vector<std::vector<double>> s2 = {{0.2, 0.3}, {50.0, 100.0}};

    std::vector<double> expected_x = {0.1, 0.2, 0.3};
    std::vector<double> expected_y = {5.0, 10.0, 100.9};
    double offset = 0.9;

    // act
    OffsetSpectraStitch stitch({s1, s2}, offset);
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE_THAT(res[0][i],
                   Catch::Matchers::WithinRel(expected_x[i], PRECISION));
      REQUIRE_THAT(res[1][i],
                   Catch::Matchers::WithinRel(expected_y[i], PRECISION));
    }
  }

  SECTION("No overlap, just appending") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.1, 0.2}, {5.0, 10.0}};
    std::vector<std::vector<double>> s2 = {{0.3, 0.4}, {50.0, 100.0}};

    std::vector<double> expected_x = {0.1, 0.2, 0.3, 0.4};
    std::vector<double> expected_y = {5.0, 10.0, 482.1, 532.1};
    double offset = 432.1;

    // act
    OffsetSpectraStitch stitch({s1, s2}, offset);
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE_THAT(res[0][i],
                   Catch::Matchers::WithinRel(expected_x[i], PRECISION));
      REQUIRE_THAT(res[1][i],
                   Catch::Matchers::WithinRel(expected_y[i], PRECISION));
    }
  }

  SECTION("Complete overlap, second ignored") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.1, 0.2}, {5.0, 10.0}};
    std::vector<std::vector<double>> s2 = {{0.12, 0.19}, {50.0, 100.0}};

    std::vector<double> expected_x = {0.1, 0.2};
    std::vector<double> expected_y = {5.0, 10.0};
    double offset = 500.0;

    // act
    OffsetSpectraStitch stitch({s1, s2}, offset);
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE_THAT(res[0][i],
                   Catch::Matchers::WithinRel(expected_x[i], PRECISION));
      REQUIRE_THAT(res[1][i],
                   Catch::Matchers::WithinRel(expected_y[i], PRECISION));
    }
  }
}
}  // namespace horiba::test
