#ifndef PARALLELS_SRC_LIB_TYPES_H_
#define PARALLELS_SRC_LIB_TYPES_H_

#include <limits>
#include <memory>
#include <vector>

namespace s21 {
using row_type = std::vector<double>;
using m_dbl_type = std::vector<row_type>;
using row_ptr = std::shared_ptr<row_type>;
using m_ptr = std::shared_ptr<m_dbl_type>;
using const_m_ptr = std::shared_ptr<const m_dbl_type>;

static const double kEps = 2e-5;

/// @brief structure for storing result of TSP algorithm. Has 2 open fields:
/// vertices (vector) and distance (fractional number).
struct TsmResult {
  std::vector<int> vertices_;
  double distance_ = std::numeric_limits<double>::max();
};

/// @brief structure for implementing TSP ant colony algorithm
struct Ant {
  Ant() : quantity_(0) {}
  TsmResult ant_result_;
  double quantity_;
};

} // namespace s21

#endif // PARALLELS_SRC_LIB_TYPES_H_