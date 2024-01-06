#include "gtest/gtest.h"
#include <algorithm>
#include <vector>

#include "lib/s21_storage.h"
#include "lib/s21_types.h"

namespace s21 {

TEST(vinograd, aa) {
  for (int kk = 1; kk < 20; ++kk) {
    for (int p = 1; p < 20; ++p) {
      for (int q = 1; q < 20; ++q) {
        m_dbl_type first_matrix = s21::Storage::FillMatrixRandomly(p, kk);
        m_dbl_type second_matrix = s21::Storage::FillMatrixRandomly(kk, q);

        // now we compare simple matrix multiplication with vinograd
        std::vector<std::vector<double>> result_simple(
            p, std::vector<double>(q, 0.0));
        for (int m = 0; m < p; ++m) {
          for (int z = 0; z < q; ++z) {
            for (int k = 0; k < kk; ++k) {
              result_simple[m][z] +=
                  first_matrix.at(m).at(k) * second_matrix.at(k).at(z);
            }
          }
        }

        s21::VinogradStorage storage(first_matrix, second_matrix);
        storage.SetStrategy(s21::Storage::MultiMode::kSimple);
        storage.Multiply();
        auto result_5 = storage.GetResult();

        s21::VinogradStorage storage_2(first_matrix, second_matrix);
        storage_2.SetStrategy(s21::Storage::MultiMode::kParallel);
        storage_2.Multiply();
        auto result_6 = storage_2.GetResult();

        s21::VinogradStorage storage_3(first_matrix, second_matrix);
        storage_3.SetStrategy(s21::Storage::MultiMode::kPipe);
        storage_3.Multiply();
        auto result_7 = storage_3.GetResult();

        for (std::size_t i = 0; i < result_simple.size(); ++i) {
          for (std::size_t j = 0; j < result_simple.at(0).size(); ++j) {
            ASSERT_NEAR(result_simple.at(i).at(j), result_5.at(i).at(j), kEps);
          }
        }

        for (std::size_t i = 0; i < result_simple.size(); ++i) {
          for (std::size_t j = 0; j < result_simple.at(0).size(); ++j) {
            ASSERT_NEAR(result_simple.at(i).at(j), result_6.at(i).at(j), kEps);
          }
        }

        for (std::size_t i = 0; i < result_simple.size(); ++i) {
          for (std::size_t j = 0; j < result_simple.at(0).size(); ++j) {
            ASSERT_NEAR(result_simple.at(i).at(j), result_7.at(i).at(j), kEps);
          }
        }
      }
    }
  }
}

TEST(gauss, storage_creation) {
  m_dbl_type matr = s21::Storage::FillMatrixRandomly(5, 6);
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
}

TEST(gauss, storage_fail) {
  m_dbl_type matr = s21::Storage::FillMatrixRandomly(5, 6);
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
}

TEST(salesman, solving) {
  m_dbl_type matr = s21::Storage::FillMatrixFromFile(
      "tests/examples/weighted_undirected_graph.txt");
  s21::SalesmanStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSalesman(1000, 6);
  auto result = storage.GetResult();
  EXPECT_LE(result.distance_, 255);
}

TEST(salesman, solving2) {
  m_dbl_type matr = s21::Storage::FillMatrixFromFile("tests/examples/wug2.txt");
  s21::SalesmanStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSalesman(1000, 4);
  auto result = storage.GetResult();
  EXPECT_EQ(result.distance_, 80);
}

TEST(salesman, solving3) {
  m_dbl_type matr = s21::Storage::FillMatrixFromFile("tests/examples/wug3.txt");
  s21::SalesmanStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSalesman(1000, 1);
  auto result = storage.GetResult();
  EXPECT_EQ(result.distance_, 48);
}

TEST(gauss, solving_1) {
  m_dbl_type matr = {
      {2.0, 4.0, -2.0, 6.0}, {3.0, 3.0, 3.0, 3.0}, {2.0, -2.0, 8.0, 4.0}};
  s21::GaussStorage storage(matr);
  storage.SetThreadCount(4);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {14, -8, -5, 1};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
}

TEST(gauss, solving_2) {
  m_dbl_type matr = {{1.0, 2.0, 3.0}, {2.0, 3.0, 1.0}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {-7, 5, 1};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
}

TEST(gauss, solving_3) {
  m_dbl_type matr = {
      {1.0, 4.0, 3.0, 1.0}, {1.0, 2.0, 9.0, 1.0}, {1.0, 6.0, 6.0, 1.0}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {1, 0, 0, 1};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
}

TEST(gauss, solving_4) {
  m_dbl_type matr = {
      {1.0, 3.0, 3.0, 2.0}, {3.0, 9.0, 3.0, 3.0}, {3.0, 6.0, 6.0, 4.0}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {0, 1.0 / 6, 1.0 / 2, 1};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
}

TEST(gauss, solving_5) {
  m_dbl_type matr = {
      {3.0, 4.0, 1.0, 1.0}, {2.0, 3.0, 0.0, 0.0}, {4.0, 3.0, -1.0, -2.0}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {-3.0 / 7, 2.0 / 7, 8.0 / 7};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
}

TEST(gauss, solving_6) {
  m_dbl_type matr = {{4.0, 9, 0, 8}, {8, 0, 6, -1}, {0, 6, 6, -1}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {0.5, 2.0 / 3.0, -5.0 / 6.0, 1};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
  std::cout << std::endl;
}

TEST(gauss, solving_7) {
  m_dbl_type matr = {{15, 2, -3, 4, 2.14, 44.96},
                     {2, 3, 4, 5, 6, 0},
                     {-1, 14, 2, -13, 4, -36.44},
                     {-3, -1.25, 12.6, 13.4, -2.16, 94.69},
                     {2, 1, -4, 2, 1, -18}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {4.74777, 0.201392, 5.93893, 1.45832, -6.85784, 1};
  for (std::size_t i = 0; i < result.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
  std::cout << std::endl;
}

TEST(gauss, solving_8) {
  m_dbl_type matr = {{15, 2, -3, 4, 2.14, 3.17, 2.23, 51.125},
                     {2, 3, 4, 5, 6, 4.24, 11.2, -13.08},
                     {-1, 14, 2, -13, 4, -2.32, -10.5, 116.79},
                     {-3, -1.25, 12.6, 13.4, -2.16, 0, 25.4, -13.99},
                     {2, 1, -4, 2, 1, 0, -12, 42},
                     {1.15, -2.25, -0.2, 0, 15.4, -11.1, 2.86, -239.09},
                     {1.35, -0.86, 0.34, -0.43, -1.55, -2.55, 3.55, -10.045}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {1, 15, -2.5, 4, -11, 3, -1.5, 1};
  for (std::size_t i = 0; i < result.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
  std::cout << std::endl;
}

TEST(gauss, solving_9) {
  m_dbl_type matr = {{15, 2, -3, 4, 2.14, 3.17, 2.23, 51.125},
                     {2, 3, 4, 5, 6, 0, 11.2, -25.8},
                     {-1, 14, 2, -13, 4, 0, -10.5, 123.75},
                     {-3, -1.25, 12.6, 13.4, -2.16, 0, 25.4, -13.99},
                     {2, 1, -4, 2, 1, 0, -12, 42},
                     {1.15, -2.25, -0.2, 0, 15.4, -11.1, 2.86, -239.09},
                     {1.35, -0.86, 0.34, -0.43, -1.55, -2.55, 3.55, -10.045}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {1, 15, -2.5, 4, -11, 3, -1.5, 1};
  for (std::size_t i = 0; i < result.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
  std::cout << std::endl;
}

TEST(gauss, solving_10) {
  m_dbl_type matr = {{15, 2, -3, 4, 2.14, 3.17, 2.23, 51.125},
                     {2, 3, 4, 5, 6, 0, 11.2, -25.8},
                     {-1, 14, 2, -13, 4, 0, -10.5, 123.75},
                     {-3, -1.25, 0, 13.4, -2.16, 0, 25.4, -17.51},
                     {2, 1, 0, 2, 1, 0, -12, 32},
                     {1.15, -2.25, 0, 0, 15.4, -11.1, 2.86, -239.59},
                     {1.35, -0.86, 0.34, -0.43, -1.55, -2.55, 3.55, -10.045}};
  s21::GaussStorage storage(matr);
  storage.SetThreadCount(4);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  storage.SolveSle();
  auto result = storage.GetResult();
  row_type expected = {2.20779,  12.3672, 1.07847,  2.06057,
                       -10.3237, 4.52368, -1.78498, 1};
  for (std::size_t i = 0; i < result.size(); ++i) {
    EXPECT_NEAR(result.at(i), expected.at(i), kEps);
  }
  std::cout << std::endl;
}

TEST(gauss, no_solution_1) {
  m_dbl_type matr = {
      {1.0, 1.0, 1.0, 2.0}, {0.0, 1.0, -3.0, 1.0}, {2.0, 1.0, 5.0, 0.0}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  EXPECT_THROW(storage.SolveSle(), std::runtime_error);
}

// inf of solutions
TEST(gauss, inf_solutions_1) {
  m_dbl_type matr = {
      {-3.0, -5.0, 36.0, 10.0}, {-1.0, 0.0, 7.0, 5.0}, {1.0, 1.0, -10.0, -4.0}};
  s21::GaussStorage storage(matr);
  storage.SetStrategy(s21::Storage::MultiMode::kSimple);
  EXPECT_THROW(storage.SolveSle(), std::runtime_error);
}

TEST(gauss, extreme_1) {
  for (auto i = 0; i < 10; ++i) {
    m_dbl_type matr = {{15, 2, -3, 4, 2.14, 3.17, 2.23, 51.125},
                       {2, 3, 4, 5, 6, 4.24, 11.2, -13.08},
                       {-1, 14, 2, -13, 4, -2.32, -10.5, 116.79},
                       {-3, -1.25, 12.6, 13.4, -2.16, 0, 25.4, -13.99},
                       {2, 1, -4, 2, 1, 0, -12, 42},
                       {1.15, -2.25, -0.2, 0, 15.4, -11.1, 2.86, -239.09},
                       {1.35, -0.86, 0.34, -0.43, -1.55, -2.55, 3.55, -10.045}};
    s21::GaussStorage storage(matr);
    storage.SetThreadCount(4);
    storage.SetStrategy(s21::Storage::MultiMode::kSimple);
    storage.SolveSle();
    auto result = storage.GetResult();
    row_type expected = {1, 15, -2.5, 4, -11, 3, -1.5, 1};
    for (std::size_t i = 0; i < result.size(); ++i) {
      EXPECT_NEAR(result.at(i), expected.at(i), kEps);
    }
  }
}

TEST(gauss, extreme_2) {
  for (auto i = 0; i < 10; ++i) {
    m_dbl_type matr = {{2, 1, -4, 2, 1, 0, -12, 42},
                       {2, 3, 4, 5, 6, 4.24, 11.2, -13.08},
                       {15, 2, -3, 4, 2.14, 3.17, 2.23, 51.125},
                       {1.35, -0.86, 0.34, -0.43, -1.55, -2.55, 3.55, -10.045},
                       {-1, 14, 2, -13, 4, -2.32, -10.5, 116.79},
                       {1.15, -2.25, -0.2, 0, 15.4, -11.1, 2.86, -239.09},
                       {-3, -1.25, 12.6, 13.4, -2.16, 0, 25.4, -13.99}};
    s21::GaussStorage storage(matr);
    storage.SetThreadCount(4);
    storage.SetStrategy(s21::Storage::MultiMode::kParallel);
    storage.SolveSle();
    auto result = storage.GetResult();
    row_type expected = {1, 15, -2.5, 4, -11, 3, -1.5, 1};
    for (std::size_t i = 0; i < result.size(); ++i) {
      EXPECT_NEAR(result.at(i), expected.at(i), kEps);
    }
  }
}

} // namespace s21

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
