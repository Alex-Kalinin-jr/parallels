#include "s21_storage.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <vector>

namespace s21 {

m_dbl_type Storage::FillMatrixRandomly(int rows, int cols) {
  std::random_device rd;
  std::default_random_engine engine(rd());
  auto gen = std::bind(std::uniform_real_distribution<>(0, 10000), engine);

  m_dbl_type result_ptr(rows, row_type(cols, 0.0));

  for (auto &row : result_ptr) {
    std::generate(row.begin(), row.end(), gen);
  }

  return result_ptr;
}

m_dbl_type Storage::FillMatrixFromFile(std::string filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw "fill_matrix_from_file: wrong file";
  }

  std::size_t vert_num = 0;
  if (!(file >> vert_num)) {
    throw "";
  }

  m_dbl_type adjacency_matrix;
  int buff_val = 0;
  while (!file.eof() && adjacency_matrix.size() != vert_num) {
    row_type buff_vectr;
    for (std::size_t i = 0; i < vert_num; ++i) {
      if (!(file >> buff_val)) {
        throw "";
      }

      buff_vectr.push_back(buff_val);
    }
    adjacency_matrix.push_back(buff_vectr);
  }
  file.close();

  return adjacency_matrix;
}

bool Storage::CheckMatrixGraphCorrectness(m_dbl_type matrix) {
  bool answ = Storage::CheckMatrixCorrectness(matrix);
  if (answ) {
    for (auto &row : matrix) {
      if (row.size() != matrix.size()) {
        answ = false;
        break;
      }
    }
  }
  return answ;
}

bool Storage::CheckMatrixCorrectness(m_dbl_type matrix) {
  bool answ = true;
  if (matrix.empty()) {
    answ = false;
  } else {
    size_t row_size = matrix.at(0).size();
    if (row_size == 0) {
      answ = false;
    } else {
      for (auto &row : matrix) {
        if (row.size() != row_size) {
          answ = false;
          break;
        }
      }
    }
  }
  return answ;
}

bool Storage::CheckForMultiplication(m_dbl_type first, m_dbl_type second) {
  bool answ = Storage::CheckMatrixCorrectness(first) &&
              Storage::CheckMatrixCorrectness(second);
  if (answ) {
    for (auto &row : first) {
      if (row.size() != second.size()) {
        answ = false;
        break;
      }
    }
  }
  return answ;
}

bool Storage::CheckSleSizeCorrectness(m_dbl_type matrix) {
  bool answ = Storage::CheckMatrixCorrectness(matrix);
  if (matrix.size() != (matrix.at(0).size() - 1)) {
    answ = false;
  }
  return answ;
}

VinogradStorage::VinogradStorage(m_dbl_type first, m_dbl_type second)
    : Storage(), vinograd_(nullptr), th_count_(1) {
  if (!Storage::CheckForMultiplication(first, second)) {
    throw "";
  }

  first_ = std::make_shared<m_dbl_type>(first);
  second_ = std::make_shared<m_dbl_type>(second);
  result_ = (std::make_shared<m_dbl_type>(m_dbl_type(
      first_->size(), std::vector<double>(second_->at(0).size(), 0))));
}

void VinogradStorage::SetStrategy(MultiMode mode) {
  switch (mode) {
  case (MultiMode::kSimple): {
    vinograd_ = std::make_shared<SimpleVinograd>(first_, second_, result_);
    break;
  }
  case (MultiMode::kParallel): {
    vinograd_ =
        std::make_shared<ParallelVinograd>(first_, second_, result_, th_count_);
    break;
  }
  case (MultiMode::kPipe): {
    vinograd_ = std::make_shared<PipeVinograd>(first_, second_, result_);
    break;
  }
  default:
  case (MultiMode::kEnd): {
    break;
  }
  }
}

void VinogradStorage::SetThreadCount(std::size_t t_num) {
  if (t_num > 6) {
    throw "";
  }
  th_count_ = t_num;
}

void VinogradStorage::Multiply() {
  if (vinograd_ == nullptr) {
    throw "";
  }
  ResetResult();
  vinograd_->Multiply();
}

void VinogradStorage::ResetResult() {
  for (auto &row : *result_) {
    std::fill(row.begin(), row.end(), 0.0);
  }
}

m_dbl_type VinogradStorage::GetResult() const { return *result_; }
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
GaussStorage::GaussStorage(m_dbl_type first)
    : Storage(), gauss_(nullptr), th_count_(1) {
  if (!Storage::CheckSleSizeCorrectness(first)) {
    throw "";
  }
  matrix_ = std::make_shared<m_dbl_type>(first);
  result_ = std::make_shared<row_type>(row_type(first.at(0).size(), 1.0));
}

void GaussStorage::SetStrategy(MultiMode mode) {
  switch (mode) {
  case (MultiMode::kSimple): {
    gauss_ = std::make_shared<SimpleGauss>(matrix_, result_);
    break;
  }
  case (MultiMode::kParallel): {
    gauss_ = std::make_shared<ParallelGauss>(matrix_, result_, th_count_);
    break;
  }
  default:
    break;
  }
}

void GaussStorage::ResetResult() {
  std::fill(result_->begin(), result_->end(), 1.0);
}

void GaussStorage::SolveSle() {
  if (gauss_ == nullptr) {
    throw "";
  }
  ResetResult();
  gauss_->SolveSle();
}

row_type GaussStorage::GetResult() const { return *result_; }

void GaussStorage::SetThreadCount(std::size_t t_num) {
  if (t_num > 6) {
    throw "";
  }
  th_count_ = t_num;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

SalesmanStorage::SalesmanStorage(m_dbl_type matrix)
    : Storage(), algorithm_(nullptr) {
  if (!Storage::CheckMatrixGraphCorrectness(matrix)) {
    throw "";
  }
  matrix_ = std::make_shared<m_dbl_type>(matrix);
  best_result_ = std::make_shared<TsmResult>();
}

void SalesmanStorage::SetStrategy(MultiMode mode) {
  switch (mode) {
  case (MultiMode::kSimple): {
    algorithm_ = std::make_shared<LinearSolver>(matrix_, best_result_);
    break;
  }
  case (MultiMode::kParallel): {
    algorithm_ = std::make_shared<LinearSolver>(matrix_, best_result_);
    break;
  }
  default:
    break;
  }
}

void SalesmanStorage::SolveSalesman(const std::size_t iterations,
                                    const std::size_t threads) {
  if (algorithm_ == nullptr) {
    throw "";
  }
  algorithm_->SolveSalesman(iterations, threads);
}

void SalesmanStorage::ResetResult() {
  best_result_.reset();
  best_result_ = std::make_shared<TsmResult>();
}

TsmResult SalesmanStorage::GetResult() const { return *best_result_; }

} // namespace s21