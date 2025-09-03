#include <horiba_cpp_sdk/core/stitching/simple_spectra_stitch.h>
#include <spdlog/spdlog.h>

#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <strstream>
#include <utility>
#include <vector>

namespace horiba::test {

void print_vector(const std::vector<double>& vec, const std::string& name) {
  spdlog::debug("{}: ", name);

  std::ostringstream ss;
  for (const auto& val : vec) {
    ss << val << " ";
  }

  spdlog::debug("{}", ss.str());
}

using namespace horiba::core::stitching;

TEST_CASE("Test Simple Spectra Stitching", "[simple_spectra_stitch]") {
  // arrange
  auto approx_equal = [](double a, double b) { return std::abs(a - b) < 1e-8; };

  SECTION("Simple overlapping spectra") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.0, 1.0, 2.0}, {10.0, 20.0, 30.0}};
    std::vector<std::vector<double>> s2 = {{1.5, 2.0, 3.0},
                                           {100.0, 200.0, 300.0}};
    std::vector<double> expected_x = {0.0, 1.0, 2.0, 3.0};
    std::vector<double> expected_y = {10.0, 20.0, 30, 300.0};

    // act
    SimpleSpectraStitch stitch({s1, s2});
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE(approx_equal(res[0][i], expected_x[i]));
      REQUIRE(approx_equal(res[1][i], expected_y[i]));
    }
  }

  SECTION("Very close double values") {
    // arrange
    std::vector<std::vector<double>> s1 = {{1.000001, 2.000001}, {10.0, 20.0}};
    std::vector<std::vector<double>> s2 = {{2.0000001, 3.0}, {100.0, 200.0}};
    std::vector<double> expected_x = {1.000001, 2.000001, 3.0};
    std::vector<double> expected_y = {10.0, 20.0, 200.0};

    // act
    SimpleSpectraStitch stitch({s1, s2});
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE(approx_equal(res[0][i], expected_x[i]));
      REQUIRE(approx_equal(res[1][i], expected_y[i]));
    }
  }

  SECTION("Negative values") {
    // arrange
    std::vector<std::vector<double>> s1 = {{-3.0, -2.0, -1.0},
                                           {-30.0, -20.0, -10.0}};
    std::vector<std::vector<double>> s2 = {{-2.5, -1.5, 0.0},
                                           {-300.0, -200.0, 0.0}};
    std::vector<double> expected_x = {-3.0, -2.0, -1.0, 0.0};
    std::vector<double> expected_y = {-30.0, -20.0, -10.0, 0.0};

    // act
    SimpleSpectraStitch stitch({s1, s2});
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE(approx_equal(res[0][i], expected_x[i]));
      REQUIRE(approx_equal(res[1][i], expected_y[i]));
    }
  }

  SECTION("Overlapping with very small differences") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.0, 0.000001, 0.000002},
                                           {1.0, 2.0, 3.0}};
    std::vector<std::vector<double>> s2 = {{0.0000015, 0.0000025},
                                           {100.0, 200.0}};
    std::vector<double> expected_x = {0.0, 0.000001, 0.000002, 0.0000025};
    std::vector<double> expected_y = {1.0, 2.0, 3.0, 200.0};

    // act
    SimpleSpectraStitch stitch({s1, s2});
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE(approx_equal(res[0][i], expected_x[i]));
      REQUIRE(approx_equal(res[1][i], expected_y[i]));
    }
  }

  SECTION("Single-point overlap") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.1, 0.2}, {5.0, 10.0}};
    std::vector<std::vector<double>> s2 = {{0.2, 0.3}, {50.0, 100.0}};

    std::vector<double> expected_x = {0.1, 0.2, 0.3};
    std::vector<double> expected_y = {5.0, 10.0, 100.0};

    // act
    SimpleSpectraStitch stitch({s1, s2});
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE(approx_equal(res[0][i], expected_x[i]));
      REQUIRE(approx_equal(res[1][i], expected_y[i]));
    }
  }

  SECTION("No overlap, just appending") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.1, 0.2}, {5.0, 10.0}};
    std::vector<std::vector<double>> s2 = {{0.3, 0.4}, {50.0, 100.0}};

    std::vector<double> expected_x = {0.1, 0.2, 0.3, 0.4};
    std::vector<double> expected_y = {5.0, 10.0, 50.0, 100.0};

    // act
    SimpleSpectraStitch stitch({s1, s2});
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE(approx_equal(res[0][i], expected_x[i]));
      REQUIRE(approx_equal(res[1][i], expected_y[i]));
    }
  }

  SECTION("Complete overlap, second ignored") {
    // arrange
    std::vector<std::vector<double>> s1 = {{0.1, 0.2}, {5.0, 10.0}};
    std::vector<std::vector<double>> s2 = {{0.12, 0.19}, {50.0, 100.0}};

    std::vector<double> expected_x = {0.1, 0.2};
    std::vector<double> expected_y = {5.0, 10.0};

    // act
    SimpleSpectraStitch stitch({s1, s2});
    auto res = stitch.stitched_spectra();

    // assert
    print_vector(res[0], "res_x");
    print_vector(expected_x, "expected_x");
    print_vector(res[1], "res_y");
    print_vector(expected_y, "expected_y");
    spdlog::debug("{}", std::string(80, '-'));

    REQUIRE(res[0].size() == expected_x.size());
    for (size_t i = 0; i < expected_x.size(); ++i) {
      REQUIRE(approx_equal(res[0][i], expected_x[i]));
      REQUIRE(approx_equal(res[1][i], expected_y[i]));
    }
  }
}
}  // namespace horiba::test
