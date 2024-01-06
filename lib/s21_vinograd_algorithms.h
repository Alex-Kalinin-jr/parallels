#ifndef PARALLELS_SRC_LIB_S21_VINOGRAD_ALGORITHMS_H_
#define PARALLELS_SRC_LIB_S21_VINOGRAD_ALGORITHMS_H_

#include <future>
#include <memory>
#include <mutex>
#include <vector>

#include "s21_types.h"

namespace s21 {

class Vinograd {
public:
  Vinograd() = default;
  virtual ~Vinograd() = default;

  /// @brief multiplies two matrices
  virtual void Multiply() = 0;

};

class SimpleVinograd : public Vinograd {
public:
  /// @brief ctor
  /// @param first first matrix
  /// @param second second matrix
  /// @param result result matrix.
  SimpleVinograd(m_ptr first, m_ptr second, m_ptr result);
  ~SimpleVinograd() = default;

  /// @brief multiplies two matrices
  void Multiply() override;
  /// @brief perform necessary operations under result matrix from start row to
  /// end row
  /// @param start start row
  /// @param end end row
  void AddBiasForOddRows(size_t start, size_t end);

protected:
  const_m_ptr first_;
  const_m_ptr second_;
  m_ptr result_;
  std::size_t f_rows_;
  std::size_t f_cols_;
  std::size_t s_rows_;
  std::size_t s_cols_;
  std::shared_ptr<std::vector<double>> row_factor_;
  std::shared_ptr<std::vector<double>> col_factor_;

  void ComputeRowFactor();
  void ComputeColFactor();
  void MultiplyMainLoop(size_t, size_t);
};

class PipeVinograd : public SimpleVinograd {
public:
  /// @brief ctor
  /// @param first first matrix for multiplication
  /// @param second second matrix for multiplication
  /// @param result result matrix
  PipeVinograd(m_ptr first, m_ptr second, m_ptr result)
      : SimpleVinograd(first, second, result){};
  ~PipeVinograd() = default;

  /// @brief multiplies two matrices
  virtual void Multiply() override;

private:
  std::mutex mtx_a_;
  std::future<void> future_one_;
  std::future<void> future_two_;

  void MultiplyPartialMatrix(size_t start, size_t end);
};

class ParallelVinograd : public SimpleVinograd {
public:
  /// @brief ctor
  /// @param first first matrix
  /// @param second second matrix
  /// @param result result matrix
  /// @param t_num number of threads
  ParallelVinograd(m_ptr first, m_ptr second, m_ptr result,
                   std::size_t t_num = 1)
      : SimpleVinograd(first, second, result), threads_num_(t_num){};
  ~ParallelVinograd() = default;

  /// @brief launches multiplying
  virtual void Multiply() override;

private:
  std::once_flag flag_one_;
  std::once_flag flag_two_;
  std::once_flag flag_three_;
  std::size_t threads_num_;

  void MultiplyPartial(size_t start, size_t end);
};

} // namespace s21

#endif // PARALLELS_SRC_LIB_S21_VINOGRAD_ALGORITHMS_H_
