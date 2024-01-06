#ifndef PARALLELS_SRC_LIB_S21_GRAPH_ALGORITHMS_H_
#define PARALLELS_SRC_LIB_S21_GRAPH_ALGORITHMS_H_

#include <future>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#include "s21_types.h"

namespace s21 {

/// @brief class for multithreading solving of Salesman problem. Used only
/// in conjunction with storage class.
class GraphAlgorithms {
public:
  const int kNumAnts = 100;
  const double kAlpha = 1.0;
  const double kBeta = 2.0;
  const double kRHO = 0.5;
  const double kQ = 100.0;
  const double kInitialPheromone = 0.1;
  const int kInf = std::numeric_limits<int>::max();

  /// @brief default ctor
  GraphAlgorithms() = default;
  virtual ~GraphAlgorithms() = default;

  /// @brief launch Salesman problem solving
  /// @param iterations count of iterations
  /// @param threads count of threads
  virtual void SolveSalesman(const std::size_t iterations,
                             const std::size_t threads) = 0;
};

class LinearSolver : public GraphAlgorithms {
public:
  /// @brief ctor. Initialize matrix and output vector using smart pointers
  /// @param matrix weights graph
  /// @param result TsmResult with shortest path and distance. Initially
  /// the distance is set to infinity, shortest path - empty.
  LinearSolver(m_ptr matrix, std::shared_ptr<TsmResult> result);
  ~LinearSolver() = default;

  /// @brief launch Salesman problem solving
  /// @param iterations count of iterations
  /// @param threads count of threads
  void SolveSalesman(const std::size_t iterations,
                     const std::size_t threads) override;

protected:
  m_ptr matrix_;
  std::shared_ptr<TsmResult> best_result_;
  m_ptr phero_ptr_;

  std::mutex mutxA_;
  std::mutex mutxB_;

  m_dbl_type InitializePheromone(int n) const;
  double Eta(int i, int j) const;
  int Random() const;
  int SelectNext(const int cur, const std::vector<bool> &visited) const;
  void UpdatePheromone(const std::vector<Ant> &ants);
  void SolvePiece(const std::size_t iterations);
  Ant BuildTour(int start, std::vector<bool> &visited) const;
};

} // namespace s21

#endif // PARALLELS_SRC_LIB_S21_GRAPH_ALGORITHMS_H_
