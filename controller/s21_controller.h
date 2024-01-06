#ifndef PARALLELS_SRC_CONTROLLER_CONTROLLER_H_
#define PARALLELS_SRC_CONTROLLER_CONTROLLER_H_

#include "../lib/s21_storage.h"
#include "../lib/s21_types.h"
#include "../view/s21_console_view.h"

namespace s21 {

class ConsoleView;

class Controller {
public:
  /// @brief ctor
  /// @param view view to be controlled
  explicit Controller(std::shared_ptr<ConsoleView> view);
  Controller(const Controller &other) = delete;
  Controller &operator=(const Controller &other) = delete;
  ~Controller() {}
  /// @brief handles initial interaction with user
  /// @return returns false if user wants to quit, otherwise returns true
  bool RecieveInitialSignal();
  /// @brief adjusts filling strategy according to user choice.
  /// @param choice user choice for filling strategy
  /// @param solveFunction function to be called (SolveSalesman,
  /// MultiplyByVinograd, etc)
  void ReceiveSignal(int choice, void (Controller::*solveFunction)());

private:
  std::shared_ptr<ConsoleView> view_;

  /// @brief adjusts filling strategy for input data(load, random, user-defined)
  /// @param choice user choice for filling strategy
  void SetMatrixFillingStrategy(const int choice) const;

  /// @brief launches solving process 2 times - linear and parallel
  void SolveSalesman();
  /// @brief launches linear solving
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param iters count of iterations
  void SolveSalesmanLinear(SalesmanStorage &storage, const int iters) const;
  /// @brief launches parallel solving
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param iters count of iterations
  /// @param threads count of threads
  void SolveSalesmanParallel(SalesmanStorage &storage, const int iters,
                             const int threads) const;
  /// @brief wrapper for solving and duration measurement
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param iters count of iterations
  /// @param threads count of threads
  /// @return pair of result and duration
  std::pair<TsmResult, std::string> ComputeSalesman(SalesmanStorage &storage,
                                                    const int iters,
                                                    const int threads) const;
  /// @brief operates by view and outputs the Salesman result in apporpriate
  /// format
  /// @param buff pair of result and duration
  void OutputSalesmanResult(std::pair<TsmResult, std::string> buff) const;
  /// @brief launches Vinograd multiplication 3 times - simple, parallel and
  /// pipe
  void MultiplyByVinograd();
  /// @brief launches simple Vinograd multiplication
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param count count of iterations
  void MultiplyVinoSimple(VinogradStorage &storage, const int count) const;
  /// @brief launches parallel Vinograd multiplication
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param count count of multiplications
  /// @param threads number of threads
  void MultiplyVinoParallel(VinogradStorage &storage, const int count,
                            const int threads) const;
  /// @brief launches pipe Vinograd multiplication
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param count count of multiplications
  void MultiplyVinoPipe(VinogradStorage &storage, const int count) const;
  /// @brief wrapper for solving and duration measurement
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param count count of multiplications
  /// @return pair of result and duration
  std::pair<m_dbl_type, std::string> ComputeVinograd(VinogradStorage storage,
                                                     size_t count) const;
  /// @brief operates by view and outputs the Salesman result in apporpriate
  /// format
  /// @param result pair of result and duration
  void OutputVinogradResult(std::pair<m_dbl_type, std::string> result) const;
  /// @brief launches Gaussian equations solving 2 times - linear and parallel
  void SolveGaussSle();
  /// @brief solves linear Gaussian equation linear
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param count count of multiplications
  void SolveGaussSleLinear(GaussStorage &storage, const int count) const;
  /// @brief solves linear Gaussian equation in parallel
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param count count of multiplications
  void SolveGaussSleParallel(GaussStorage &storage, const int count,
                             const int threads) const;
  /// @brief wrapper for solving and duration measurement
  /// @param storage storage for input data, result and sandbox for operating
  /// with them.
  /// @param count count of multiplications
  /// @return pair of result and duration
  std::pair<row_type, std::string> ComputeGauss(GaussStorage storage,
                                                size_t count) const;
  /// @brief operates by view and outputs the Salesman result in apporpriate
  /// format
  /// @param result pair of result and duration
  void OutputGaussResult(const std::pair<row_type, std::string> result) const;
};

} // namespace s21

#endif // SRC_CONTROLLER_CONTROLLER_H_
