#ifndef PARALLELS_LIB_GAUSS_H_
#define PARALLELS_LIB_GAUSS_H_

#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

#include "s21_types.h"

namespace s21 {

/// @brief class for solving of SLEs with using of Gauss method. Used only
/// in conjunction with storage class.
class Gauss {
public:
  /// @brief default ctor
  Gauss(){};
  virtual ~Gauss() = default;

  /// @brief solves system of linear equations
  virtual void SolveSle() = 0;
};

/// @brief solves SLEs in linear mode
class SimpleGauss : public Gauss {

public:
  /// @brief ctor. Initialize matrix and output vector using smart pointers
  /// @param m input matrix - system of linear equations
  /// @param r output vector. Has the size "answer + 1"
  SimpleGauss(m_ptr m, row_ptr r);

  /// @brief solves SLE in linear mode
  void SolveSle() override;

protected:
  m_ptr matrix_;
  row_ptr output_;
  std::size_t rows_;
  std::size_t cols_;

  void AdjustEchelon(std::size_t k);
  void LeadToEchelon();
  void CheckEchelon();
  void BackPropagation();
};

/// @brief solves SLEs in parallel mode
class ParallelGauss : public SimpleGauss {
public:
  ParallelGauss(m_ptr m, row_ptr r, std::size_t th_count = 1)
      : SimpleGauss(m, r), th_count_(th_count){};
  ~ParallelGauss() = default;

  /// @brief solves SLEs in parallel mode
  void SolveSle() override;

private:
  std::size_t th_count_;

  void AdjustEchelon(std::size_t k);
  void CheckEchelon();
  void BackPropagation();
};

} // namespace s21

#endif // PARALLELS_LIB_GAUSS_H_
