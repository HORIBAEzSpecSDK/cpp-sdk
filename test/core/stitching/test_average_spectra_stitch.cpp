#include <horiba_cpp_sdk/core/stitching/average_spectra_stitch.h>
#include <spdlog/spdlog.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <string>
#include <vector>

#include "../../printing_helpers.h"

namespace horiba::test {

using namespace horiba::core::stitching;

TEST_CASE("Test Average Spectra Stitching", "[average_spectra_stitch]") {
  // arrange
  const auto PRECISION = 1e-8;

  SECTION("Simple overlapping spectra") {
    // arrange
    std::vector<std::vector<double>> s1 = {
        {1.0, 2.0, 3.0, 4.0},     // x
        {10.0, 20.0, 30.0, 40.0}  // y
    };
    std::vector<std::vector<double>> s2 = {
        {3.5, 4.0, 5.0, 6.0},     // x
        {35.0, 45.0, 55.0, 65.0}  // y
    };
    std::vector<double> expected_x = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<double> expected_y = {10.0, 20.0, 30.0, 42.5, 55.0, 65.0};

    // act
    AverageSpectraStitch stitch({s1, s2});
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
    std::vector<double> expected_y = {-30.0, -110.0, -105.0, 0.0};

    // act
    AverageSpectraStitch stitch({s1, s2});
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

  SECTION("Overlapping with same x values") {
    // arrange
    std::vector<std::vector<double>> s1 = {{1.0, 2.0, 3.0}, {10.0, 20.0, 30.0}};
    std::vector<std::vector<double>> s2 = {{2.0, 3.0, 4.0}, {22.0, 32.0, 42.0}};
    std::vector<double> expected_x = {1.0, 2.0, 3.0, 4.0};
    std::vector<double> expected_y = {10.0, 21.0, 31.0, 42.0};

    // act
    AverageSpectraStitch stitch({s1, s2});
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
    std::vector<double> expected_y = {5.0, 30.0, 100.0};

    // act
    AverageSpectraStitch stitch({s1, s2});
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
    std::vector<double> expected_y = {5.0, 10.0, 50.0, 100.0};

    // act
    AverageSpectraStitch stitch({s1, s2});
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

  SECTION("Wrong order, no overlap") {
    // arrange
    std::vector<std::vector<double>> s1 = {{5.0, 6.0, 7.0}, {50.0, 60.0, 70.0}};
    std::vector<std::vector<double>> s2 = {{1.0, 2.0, 3.0}, {10.0, 20.0, 30.0}};

    std::vector<double> expected_x = {1.0, 2.0, 3.0, 5.0, 6.0, 7.0};
    std::vector<double> expected_y = {10.0, 20.0, 30.0, 50.0, 60.0, 70.0};

    // act
    AverageSpectraStitch stitch({s1, s2});
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

  SECTION("Complete overlap") {
    // arrange
    std::vector<std::vector<double>> s1 = {{1.0, 2.0, 3.0, 4.0, 5.0},
                                           {10.0, 20.0, 30.0, 40.0, 50.0}};
    std::vector<std::vector<double>> s2 = {{2.5, 3.0, 3.5}, {25.0, 35.0, 45.0}};

    std::vector<double> expected_x = {1.0, 2.0, 3.0, 4.0, 5.0};
    std::vector<double> expected_y = {10.0, 20.0, 32.5, 40.0, 50.0};

    // act
    AverageSpectraStitch stitch({s1, s2});
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
