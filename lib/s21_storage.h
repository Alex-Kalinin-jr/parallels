#ifndef PARALLELS_SRC_LIB_S21_STORAGE_H_
#define PARALLELS_SRC_LIB_S21_STORAGE_H_

#include <memory>
#include <vector>

#include "s21_gauss_algorithms.h"
#include "s21_graph_algorithms.h"
#include "s21_types.h"
#include "s21_vinograd_algorithms.h"

namespace s21 {

class Storage {
public:
  /// @brief mode of computations
  enum class MultiMode { kSimple, kParallel, kPipe, kEnd };

  /// @brief default ctor.
  Storage(){};
  virtual ~Storage() = default;

  /// @brief sets the strategy for computations: simple, parallel
  /// @param mode kSimple, kParallel
  virtual void SetStrategy(MultiMode mode) = 0;

  /// @brief creates matrix of rows x cols (in science consider it cols x rows)
  /// @param rows rows of matrix
  /// @param cols cols of matrix
  /// @return vector of rows. each row is a vector of cols
  static m_dbl_type FillMatrixRandomly(int rows, int cols);

  /// @brief fills matrix from file
  /// @param filename path to file
  /// @return filled matrix
  static m_dbl_type FillMatrixFromFile(std::string filename);

  /// @brief checks that weight matrix has correct size (width = height)
  /// @param matrix checked matrix
  /// @return true if correct, false if not
  static bool CheckMatrixGraphCorrectness(m_dbl_type matrix);

  /// @brief checks if matrix has correct size (that it is a rectangular)
  /// @param matrix matrix to be checked
  /// @return true or false
  static bool CheckMatrixCorrectness(m_dbl_type matrix);

  /// @brief firstly performs "CheckMatrixCorrectness" and then that rows = cols
  /// - 1 (for linear equation)
  /// @param matrix matrix to be checked
  /// @return true or false
  static bool CheckSleSizeCorrectness(m_dbl_type matrix);

  /// @brief firstly performs "CheckMatrixCorrectness" for matrices and then
  /// checks that first.cols == second.rows
  /// @param first first matrix
  /// @param second second matrix
  /// @return true or false
  static bool CheckForMultiplication(m_dbl_type first, m_dbl_type second);

protected:
  virtual void ResetResult() = 0;
};

class VinogradStorage : public Storage {
public:
  /// @brief ctor. Creates init matrices and result matrix (shared ptrs)
  /// @param first first matrix
  /// @param second second matrix
  VinogradStorage(m_dbl_type first, m_dbl_type second);
  ~VinogradStorage() = default;

  /// @brief returns copy of result matrix
  /// @return copy of result matrix
  m_dbl_type GetResult() const;

  /// @brief sets computation mode
  /// @param mode kSimple, kParallel, kPipe
  void SetStrategy(MultiMode mode) override;

  /// @brief multiplies two matrices and stores result inside
  void Multiply();

  void SetThreadCount(std::size_t t_num);

protected:
  virtual void ResetResult() override;

private:
  m_ptr first_;
  m_ptr second_;
  m_ptr result_;
  std::shared_ptr<Vinograd> vinograd_;
  std::size_t th_count_;
};

/// @brief class for storing of matrix and result vector for SLE (Gauss method)
class GaussStorage : public Storage {
public:
  /// @brief ctor. Creates matrix and result vector(shared ptrs)
  /// @param first initial matrix (copied)
  explicit GaussStorage(m_dbl_type first);
  ~GaussStorage() = default;

  /// @brief change computation method
  /// @param mode method (linear, parallel)
  void SetStrategy(MultiMode mode) override;

  /// @brief reset values of result to 1.0 (for loop computations)
  virtual void ResetResult() override;

  void SetThreadCount(std::size_t t_num);

  /// @brief launches SLE
  void SolveSle();

  /// @brief returns result vector
  /// @return vector of result (copied)
  row_type GetResult() const;

private:
  m_ptr matrix_;
  row_ptr result_;
  std::shared_ptr<Gauss> gauss_;
  std::size_t th_count_;
};

class SalesmanStorage : public Storage {
public:
  /// @brief ctor. Creates matrix and TsmResult(shared ptrs)
  /// @param matrix matrix of weights
  explicit SalesmanStorage(m_dbl_type matrix);
  ~SalesmanStorage() = default;
  /// @brief sets computation mode. For salesman storage implemented for
  /// compatibility.
  /// @param mode mode to be set(kSimple, kParallel)
  void SetStrategy(MultiMode mode) override;
  /// @brief launches computation process.
  /// @param iterations number of computations
  /// @param threads number of threads in which the computations will be
  /// launched
  void SolveSalesman(const std::size_t iterations, const std::size_t threads);
  /// @brief sets values of best_result to initial
  void ResetResult() override;
  /// @brief returns instance of TsmResult stored in the instance of this class
  /// @return TsmResult
  TsmResult GetResult() const;

private:
  m_ptr matrix_;
  std::shared_ptr<TsmResult> best_result_;
  std::shared_ptr<GraphAlgorithms> algorithm_;
};

} // namespace s21

#endif // PARALLELS_SRC_LIB_S21_STORAGE_H_