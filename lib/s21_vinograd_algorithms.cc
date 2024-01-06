#include "s21_vinograd_algorithms.h"

#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace s21 {

SimpleVinograd::SimpleVinograd(m_ptr first, m_ptr second, m_ptr result_ptr)
    : first_(first), second_(second), result_(result_ptr) {
  if (result_->size() != first_->size()) {
    throw "";
  }

  for (auto &row : *result_) {
    if (row.size() != second_->at(0).size()) {
      throw "";
    }
  }

  f_rows_ = first_->size();
  f_cols_ = first_->at(0).size();
  s_rows_ = second_->size();
  s_cols_ = second_->at(0).size();
  row_factor_ = std::make_shared<row_type>(row_type(f_rows_, 0.0));
  col_factor_ = std::make_shared<row_type>(row_type(s_cols_, 0.0));
}

void SimpleVinograd::Multiply() {
  ComputeRowFactor();
  ComputeColFactor();
  MultiplyMainLoop(0, f_rows_);
  if (f_cols_ % 2 != 0) {
    AddBiasForOddRows(0, f_rows_);
  }
}

void SimpleVinograd::ComputeRowFactor() {
  std::fill(row_factor_->begin(), row_factor_->end(), 0.0);

  for (size_t i = 0; i < f_rows_; ++i) {
    for (size_t j = 0; j < f_cols_ / 2; ++j) {
      row_factor_->at(i) +=
          first_->at(i).at(2 * j) * first_->at(i).at(2 * j + 1);
    }
  }
}

void SimpleVinograd::ComputeColFactor() {
  std::fill(col_factor_->begin(), col_factor_->end(), 0.0);

  for (size_t i = 0; i < s_cols_; ++i) {
    for (size_t j = 0; j < f_cols_ / 2; ++j) {
      col_factor_->at(i) +=
          second_->at(2 * j).at(i) * second_->at(2 * j + 1).at(i);
    }
  }
}

void SimpleVinograd::AddBiasForOddRows(size_t start, size_t end) {
  for (size_t i = start; i < end; ++i) {
    for (size_t j = 0; j < s_cols_; ++j) {
      result_->at(i).at(j) +=
          first_->at(i).at(f_cols_ - 1) * second_->at(f_cols_ - 1).at(j);
    }
  }
}

void SimpleVinograd::MultiplyMainLoop(size_t start, size_t end) {
  for (size_t i = start; i < end; ++i) {
    for (size_t j = 0; j < s_cols_; ++j) {
      result_->at(i).at(j) = -row_factor_->at(i) - col_factor_->at(j);

      for (size_t k = 0; k < f_cols_ / 2; ++k) {
        result_->at(i).at(j) +=
            (first_->at(i).at(2 * k) + second_->at(2 * k + 1).at(j)) *
            (first_->at(i).at(2 * k + 1) + second_->at(2 * k).at(j));
      }
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void ParallelVinograd::Multiply() {
  std::vector<std::thread> thread_vector;
  auto step = f_rows_ / threads_num_;
  auto start = 0;
  auto end = step;

  for (std::size_t i = 0; i < threads_num_; ++i) {
    if (i == (threads_num_ - 1)) {
      end += (f_rows_ % threads_num_);
    }

    thread_vector.push_back(
        std::thread(&ParallelVinograd::MultiplyPartial, this, start, end));
    start += step;
    end += step;
  }

  for (auto &threaD : thread_vector) {
    threaD.join();
  }
}

void ParallelVinograd::MultiplyPartial(size_t start, size_t end) {
  std::call_once(flag_one_, [this]() { SimpleVinograd::ComputeRowFactor(); });
  std::call_once(flag_two_, [this]() { SimpleVinograd::ComputeColFactor(); });
  MultiplyMainLoop(start, end);
  if (f_cols_ % 2 != 0) {
    AddBiasForOddRows(start, end);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void PipeVinograd::Multiply() {
  std::packaged_task<void()> task(
      [this]() mutable { SimpleVinograd::ComputeRowFactor(); });
  future_one_ = task.get_future();
  std::thread th_one(std::move(task));

  std::packaged_task<void()> second_task(
      [&]() mutable { SimpleVinograd::ComputeColFactor(); });
  future_two_ = second_task.get_future();
  std::thread th_two(std::move(second_task));

  std::thread th_three(&PipeVinograd::MultiplyPartialMatrix, this, 0, f_rows_);

  th_one.join();
  th_two.join();
  th_three.join();
  if (f_cols_ % 2 != 0) {
    auto thread_four =
        std::thread(&SimpleVinograd::AddBiasForOddRows, this, 0, f_rows_);
    thread_four.join();
  }
}

void PipeVinograd::MultiplyPartialMatrix(size_t start, size_t end) {
  future_one_.wait();
  future_two_.wait();
  {
    std::lock_guard<std::mutex> vino_lock(mtx_a_);
    MultiplyMainLoop(start, end);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

} // namespace s21
