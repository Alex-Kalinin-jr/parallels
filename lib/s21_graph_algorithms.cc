#include "s21_graph_algorithms.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <future>
#include <memory>
#include <random>
#include <thread>
#include <vector>

namespace s21 {

LinearSolver::LinearSolver(m_ptr matrix, std::shared_ptr<TsmResult> result)
    : matrix_(matrix), best_result_(result) {
  phero_ptr_ =
      std::make_shared<m_dbl_type>(InitializePheromone(matrix_->size()));
}

m_dbl_type LinearSolver::InitializePheromone(int n) const {
  if (n < 1) {
    throw "";
  }
  return m_dbl_type(n, std::vector<double>(n, kInitialPheromone));
}

double LinearSolver::Eta(int i, int j) const {
  return 1.0 / (matrix_->at(i).at(j));
}

int LinearSolver::Random() const {
  std::random_device rd;
  std::default_random_engine engine(rd());
  auto gen = std::bind(std::uniform_int_distribution<>(0, matrix_->size() - 2),
                       engine);
  return gen();
}

int LinearSolver::SelectNext(const int cur,
                             const std::vector<bool> &visited) const {
  int answ = -1;
  double answ_attractivness = 0.0;
  int size = visited.size();
  double sum = 0.0;
  for (int i = 0; i < size; ++i) {
    if (visited.at(i) == false && (matrix_->at(cur).at(i) > 0)) {
      sum += pow(phero_ptr_->at(cur).at(i), kAlpha) * pow(Eta(cur, i), kBeta);
    }
  }

  for (int i = 0; i < size; ++i) {
    if (visited.at(i) == false && (matrix_->at(cur).at(i) > 0)) {
      double buff_attractivness = 0.0;
      buff_attractivness = pow(phero_ptr_->at(cur).at(i), kAlpha) *
                           pow(Eta(cur, i), kBeta) / sum;
      if (buff_attractivness > answ_attractivness) {
        answ = i;
        answ_attractivness = buff_attractivness;
      }
    }
  }
  return answ;
}

void LinearSolver::UpdatePheromone(const std::vector<Ant> &ants) {
  for (auto &start : *phero_ptr_) {
    std::transform(start.cbegin(), start.cend(), start.begin(),
                   [this](double x) { return x * kRHO; });
  }

  for (auto &ant : ants) {
    for (auto itr = ant.ant_result_.vertices_.begin();
         itr != ant.ant_result_.vertices_.end() - 1; ++itr) {
      int start = *itr;
      int end = *(itr + 1);
      phero_ptr_->at(start).at(end) +=
          ant.quantity_ / matrix_->at(start).at(end);
    }
  }
}

Ant LinearSolver::BuildTour(int start, std::vector<bool> &visited) const {
  Ant ant;
  ant.ant_result_.vertices_.push_back(start);
  ant.ant_result_.distance_ = 0.0;
  visited.at(start) = true;
  int current = start;
  int size = visited.size();
  for (int i = 1; i < size; ++i) {
    int next = -1;
    next = SelectNext(current, visited);
    if (next == -1) {
      break;
    }
    ant.ant_result_.vertices_.push_back(next);
    ant.ant_result_.distance_ += matrix_->at(current).at(next);
    ant.quantity_ += phero_ptr_->at(current).at(next);
    visited.at(next) = true;
    current = next;
  }

  if (matrix_->at(current).at(start) > 0) {
    ant.ant_result_.vertices_.push_back(start);
    ant.ant_result_.distance_ += matrix_->at(current).at(start);
    ant.quantity_ += phero_ptr_->at(current).at(start);
  }

  return ant;
}

void LinearSolver::SolvePiece(const std::size_t iterations) {
  for (std::size_t iter = 0; iter < iterations; iter++) {
    std::vector<Ant> ants(kNumAnts);
    std::vector<Ant> succeded_ants;

    for (int k = 0; k < kNumAnts; ++k) {
      std::vector<bool> visited(matrix_->size(), false);
      ants.at(k) = BuildTour(Random(), visited);
      auto size = ants.at(k).ant_result_.vertices_.size();
      if (size == (matrix_->size() + 1)) {
        succeded_ants.push_back(ants.at(k));
        {
          std::lock_guard<std::mutex> general_lock(mutxB_);
          if (ants.at(k).ant_result_.distance_ <= best_result_->distance_) {
            *best_result_ = ants.at(k).ant_result_;
          }
        }
      }
    }

    {
      std::lock_guard<std::mutex> general_lock(mutxA_);
      UpdatePheromone(succeded_ants);
    }
  }
}

void LinearSolver::SolveSalesman(const std::size_t iterations,
                                 const std::size_t threads_num) {
  std::size_t iters_in_thread =
      (iterations > threads_num) ? iterations / threads_num : 1;
  std::vector<std::thread> threadVector{};

  for (std::size_t i = 0; i < threads_num; ++i) {
    threadVector.push_back(std::thread([this, iters_in_thread]() {
      LinearSolver::SolvePiece(iters_in_thread);
    }));
  }
  for (auto &threaD : threadVector) {
    threaD.join();
  }

  std::for_each(best_result_->vertices_.begin(), best_result_->vertices_.end(),
                [](int &x) { ++x; });
}

} // namespace s21
