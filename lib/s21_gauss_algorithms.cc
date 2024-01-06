#include "s21_gauss_algorithms.h"

#include <algorithm>
#include <cmath>
#include <future>
#include <mutex>
#include <thread>

namespace s21 {

SimpleGauss::SimpleGauss(m_ptr m, row_ptr r)
    : Gauss(), matrix_(m), output_(r), rows_(matrix_->size()),
      cols_(matrix_->at(0).size()) {}

void SimpleGauss::SolveSle() {
  m_dbl_type buff_matrix = *matrix_;

  LeadToEchelon();

  for (std::size_t k = 0; k < rows_; ++k) {
    AdjustEchelon(k);
  }

  CheckEchelon();

  BackPropagation();
  *matrix_ = buff_matrix;
}

void SimpleGauss::AdjustEchelon(std::size_t k) {
  for (size_t i = k + 1; i < rows_; ++i) {
    double f = matrix_->at(i).at(k) / matrix_->at(k).at(k);

    for (size_t j = k + 1; j <= rows_; ++j)
      matrix_->at(i).at(j) -= matrix_->at(k).at(j) * f;
    matrix_->at(i).at(k) = 0;
  }
}

void SimpleGauss::LeadToEchelon() {
  for (std::size_t k = 0; k < rows_; ++k) {
    std::size_t max_index = k;
    double max_value = 0;
    for (std::size_t i = k; i < rows_; ++i) {
      double scale_factor = 0;
      for (std::size_t j = k; j < rows_; ++j)
        scale_factor = std::max(std::abs(matrix_->at(i).at(j)), scale_factor);
      if (scale_factor == 0)
        continue;
      auto abs = std::abs(matrix_->at(i).at(k)) / scale_factor;
      if (abs > max_value) {
        max_index = i;
        max_value = abs;
      }
    }
    if (k != max_index)
      std::swap(matrix_->at(k), matrix_->at(max_index));
  }
}

void SimpleGauss::CheckEchelon() {
  for (std::size_t k = 0; k < rows_; ++k) {
    if (fabs(matrix_->at(k).at(k)) < 0.00001)
      throw std::runtime_error("matrix is singular");
  }
}

void SimpleGauss::BackPropagation() {
  for (size_t i = rows_; i-- > 0;) {
    output_->at(i) = matrix_->at(i).at(rows_);
    for (size_t j = i + 1; j < rows_; ++j)
      output_->at(i) -= matrix_->at(i).at(j) * output_->at(j);
    output_->at(i) /= matrix_->at(i).at(i);
  }
}

void ParallelGauss::SolveSle() {
  m_dbl_type buff_matrix = *matrix_;

  LeadToEchelon();

  std::vector<std::future<void>> futures;
  for (std::size_t k = 0; k < rows_; ++k) {
    futures.push_back(
        std::async(std::launch::async, &ParallelGauss::AdjustEchelon, this, k));
  }
  for (auto &f : futures) {
    f.wait();
  }

  std::thread th_1(&ParallelGauss::CheckEchelon, this);
  th_1.join();

  std::thread th_2(&ParallelGauss::BackPropagation, this);
  th_2.join();

  *matrix_ = buff_matrix;
}

void ParallelGauss::AdjustEchelon(std::size_t k) {
  SimpleGauss::AdjustEchelon(k);
}

void ParallelGauss::CheckEchelon() { SimpleGauss::CheckEchelon(); }

void ParallelGauss::BackPropagation() { SimpleGauss::BackPropagation(); }

} // namespace s21
