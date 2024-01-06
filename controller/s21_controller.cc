#include "s21_controller.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace s21 {

Controller::Controller(std::shared_ptr<ConsoleView> view) : view_(view) {
  if (view_.get() == nullptr) {
    throw std::invalid_argument("View is nullptr");
  }
}

void Controller::SetMatrixFillingStrategy(const int choice) const {
  switch ((ConsoleView::Choice)choice) {
  case ConsoleView::Choice::kBttnOne: {
    view_->SetFillingStrategy(ConsoleView::FillingMode::kUserFill);
    break;
  }
  case ConsoleView::Choice::kBttnTwo: {
    view_->SetFillingStrategy(ConsoleView::FillingMode::kRandFill);
    break;
  }
  case ConsoleView::Choice::kBttnThree: {
    view_->SetFillingStrategy(ConsoleView::FillingMode::kLoadFill);
    break;
  }
  default: {
    break;
  }
  }
}

bool Controller::RecieveInitialSignal() {
  bool result = true;
  view_->SetStrategy(ConsoleView::Mode::kInitMode);
  int choice = view_->Show();

  switch ((ConsoleView::Choice)choice) {
  case ConsoleView::Choice::kBttnOne: {
    view_->SetStrategy(ConsoleView::Mode::kSaleMode);
    ReceiveSignal(view_->Show(), &Controller::SolveSalesman);
    break;
  }

  case ConsoleView::Choice::kBttnTwo: {
    view_->SetStrategy(ConsoleView::Mode::kGaussMode);
    ReceiveSignal(view_->Show(), &Controller::SolveGaussSle);
    break;
  }

  case ConsoleView::Choice::kBttnThree: {
    view_->SetStrategy(ConsoleView::Mode::kVinoMode);
    ReceiveSignal(view_->Show(), &Controller::MultiplyByVinograd);
    break;
  }
  default:
  case ConsoleView::Choice::kExit: {
    result = false;
  }
  }
  return result;
}

void Controller::ReceiveSignal(int choice,
                               void (Controller::*solveFunction)()) {
  if (choice == ConsoleView::Choice::kExit) {
    return;
  }

  SetMatrixFillingStrategy(choice);

  try {
    (this->*solveFunction)();
  } catch (...) {
    view_->ShowMsg("Ошибка. Что-то пошло не так.");
  }
  view_->GetString("Введите любой символ для продолжения.");
}

// SALESMAN FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////
void Controller::SolveSalesman() {
  auto new_storage = SalesmanStorage(view_->GetMatrix());
  auto iters = view_->GetIterationsCount();
  if (iters == 0) {
    return;
  }
  auto threads = view_->GetThreadsCount();
  if (threads == 0) {
    return;
  }

  SolveSalesmanLinear(new_storage, iters);
  SolveSalesmanParallel(new_storage, iters, threads);
}

void Controller::SolveSalesmanLinear(SalesmanStorage &storage,
                                     const int iters) const {

  try {
    storage.SetStrategy(Storage::MultiMode::kSimple);
    view_->ShowMsg("\nЛинейные вычисления:\n");
    OutputSalesmanResult(ComputeSalesman(storage, iters, 1));
  } catch (...) {
    view_->ShowMsg(
        "Что-то пошло не так при линейных вычислениях. Попробуйте ещё раз");
  }
}

void Controller::SolveSalesmanParallel(SalesmanStorage &storage,
                                       const int iters,
                                       const int threads) const {

  try {
    storage.SetStrategy(Storage::MultiMode::kParallel);
    view_->ShowMsg("\nПараллельные вычисления:\n");
    OutputSalesmanResult(ComputeSalesman(storage, iters, threads));
  } catch (...) {
    view_->ShowMsg(
        "Что-то пошло не так при параллельных вычислениях. Попробуйте ещё раз");
  }
}

void Controller::OutputSalesmanResult(
    std::pair<TsmResult, std::string> buff) const {
  auto [tsm_result, buff_time] = buff;
  if (std::numeric_limits<double>::max() == tsm_result.distance_) {
    view_->ShowMsg("Путь не найден");
  } else {
    view_->ShowMsg("Кратчайшее расстояние: \n" +
                   std::to_string(tsm_result.distance_));
    view_->ShowMsg("Координаты вершин: ");
    view_->ShowVector(tsm_result.vertices_);
  }
  view_->ShowMsg("Время выполнения (мс): ");
  std::cout << buff_time << std::endl;
}

std::pair<TsmResult, std::string>
Controller::ComputeSalesman(SalesmanStorage &new_storage, const int count,
                            const int threads) const {
  auto start = std::chrono::high_resolution_clock::now();
  new_storage.SolveSalesman(count, threads);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  return std::pair<TsmResult, std::string>(new_storage.GetResult(),
                                           std::to_string(duration.count()));
}

// VINOGRAD MULTIPLICATION FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////
void Controller::MultiplyByVinograd() {
  auto new_storage = VinogradStorage(view_->GetMatrix(), view_->GetMatrix());
  auto count = view_->GetIterationsCount();
  if (count == 0) {
    return;
  }
  auto threads_count = view_->GetThreadsCount();
  MultiplyVinoSimple(new_storage, count);
  MultiplyVinoParallel(new_storage, count, threads_count);
  MultiplyVinoPipe(new_storage, count);
}

void Controller::MultiplyVinoSimple(VinogradStorage &storage,
                                    const int count) const {
  try {
    storage.SetStrategy(Storage::MultiMode::kSimple);
    view_->ShowMsg("\nЛинейные вычисления:\n");
    OutputVinogradResult(ComputeVinograd(storage, count));
  } catch (...) {
    view_->ShowMsg(
        "Что-то пошло не так при линейных вычислениях. Попробуйте ещё раз");
  }
}

void Controller::MultiplyVinoParallel(VinogradStorage &storage, const int count,
                                      const int threads) const {
  try {
    storage.SetThreadCount(threads);
    storage.SetStrategy(Storage::MultiMode::kParallel);
    view_->ShowMsg("\nПараллельные вычисления:\n");
    OutputVinogradResult(ComputeVinograd(storage, count));
  } catch (...) {
    view_->ShowMsg(
        "Что-то пошло не так при параллельных вычислениях. Попробуйте ещё раз");
  }
}

void Controller::MultiplyVinoPipe(VinogradStorage &storage,
                                  const int count) const {
  try {
    storage.SetStrategy(Storage::MultiMode::kPipe);
    view_->ShowMsg("\nPipe mode:\n");
    OutputVinogradResult(ComputeVinograd(storage, count));
  } catch (...) {
    view_->ShowMsg("Что-то пошло не так во время каскадных вычислений. "
                   "Попробуйте ещё раз");
  }
}

std::pair<m_dbl_type, std::string>
Controller::ComputeVinograd(VinogradStorage new_storage,
                            std::size_t count) const {
  auto start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < count; ++i) {
    new_storage.Multiply();
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  return std::pair<m_dbl_type, std::string>(new_storage.GetResult(),
                                            std::to_string(duration.count()));
}

void Controller::OutputVinogradResult(
    std::pair<m_dbl_type, std::string> buff) const {
  auto [matr, buff_time] = buff;
  view_->ShowMsg("Результат перемножения:");
  view_->ShowMatrix(matr);
  view_->ShowMsg("Время выполнения (мс): ");
  std::cout << buff_time << std::endl;
}

// GAUSS MULTIPLICATION FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////////
void Controller::SolveGaussSle() {
  auto new_storage = GaussStorage(view_->GetMatrix());
  auto count = view_->GetIterationsCount();
  if (count == 0) {
    return;
  }
  auto threads = view_->GetThreadsCount();
  if (threads == 0) {
    return;
  }
  SolveGaussSleLinear(new_storage, count);
  SolveGaussSleParallel(new_storage, count, threads);
}

void Controller::SolveGaussSleLinear(GaussStorage &storage,
                                     const int count) const {
  try {
    storage.SetStrategy(Storage::MultiMode::kSimple);
    view_->ShowMsg("\nЛинейные вычисления:\n");
    OutputGaussResult(ComputeGauss(storage, count));
  } catch (...) {
    view_->ShowMsg(
        "Что-то пошло не так при линейных вычислениях. Попробуйте ещё раз");
  }
}

void Controller::SolveGaussSleParallel(GaussStorage &storage, const int count,
                                       const int threads) const {
  try {
    storage.SetThreadCount(threads);
    storage.SetStrategy(Storage::MultiMode::kParallel);
    view_->ShowMsg("\nПараллельные вычисления:\n");
    OutputGaussResult(ComputeGauss(storage, count));
  } catch (...) {
    view_->ShowMsg(
        "Что-то пошло не так при параллельных вычислениях. Попробуйте ещё раз");
  }
}

std::pair<row_type, std::string>
Controller::ComputeGauss(GaussStorage new_storage, std::size_t count) const {
  auto start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i < count; ++i) {
    new_storage.SolveSle();
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  return std::pair<row_type, std::string>(new_storage.GetResult(),
                                          std::to_string(duration.count()));
}

void Controller::OutputGaussResult(
    std::pair<row_type, std::string> buff) const {
  auto [row, buff_time] = buff;
  row.pop_back();
  view_->ShowMsg("Результат:");
  view_->ShowVector(row);
  view_->ShowMsg("Время выполнения (мс): ");
  std::cout << buff_time << std::endl;
}

} // namespace s21
