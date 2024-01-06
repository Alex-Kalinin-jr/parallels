#include "s21_console_view.h"

#include <cstdlib>
#include <iostream>

namespace s21 {

void ConsoleView::SetStrategy(Mode st) {
  switch (st) {
  case Mode::kInitMode: {
    shower_ = std::make_shared<InitShower>(this);
    break;
  }
  case Mode::kSaleMode: {
    shower_ = std::make_shared<SalesmanShower>(this);
    break;
  }
  case Mode::kGaussMode: {
    shower_ = std::make_shared<GaussShower>(this);
    break;
  }
  case Mode::kVinoMode: {
    shower_ = std::make_shared<VinogradovShower>(this);
    break;
  }
  case Mode::kEndMode:
  default:
    break;
  }
}

int ConsoleView::Show() const { return shower_->Display(); }

void ConsoleView::SetFillingStrategy(FillingMode mode) {
  switch (mode) {
  case FillingMode::kRandFill: {
    filler_ = std::make_shared<RandomFiller>(this);
    break;
  }
  case FillingMode::kUserFill: {
    filler_ = std::make_shared<UserFiller>(this);
    break;
  }
  case FillingMode::kLoadFill: {
    filler_ = std::make_shared<LoadFiller>(this);
    break;
  }
  case FillingMode::kEndFill:
  default:
    break;
  }
}

m_dbl_type ConsoleView::GetMatrix() const { return filler_->FillMatrix(); }

int ConsoleView::GetUserChoice(const std::string &message) const {
  std::cout << message << std::endl;
  int choice = kExit - 1;

  while (true) {
    std::cin >> choice;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Ошибка ввода. Попробуйте снова" << std::endl;
    } else {
      break;
    }
  }
  return choice;
}

std::vector<double> ConsoleView::GetVector(const int count) const {
  std::vector<double> result_vector;
  for (int i = 0; i < count; ++i) {
    double value;
    std::cin >> value;
    if (std::cin.fail()) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "Ошибка ввода. Попробуйте снова" << std::endl;
      result_vector.clear();
      i = -1;
    } else {
      result_vector.push_back(value);
    }
  }
  return result_vector;
}

std::string ConsoleView::GetString(const std::string &message) const {
  std::cin.clear();
  std::cout << message << std::endl;
  std::string filename;
  std::cin >> filename;
  return filename;
}

int ConsoleView::GetIterationsCount() const {
  int count = -1;
  while (count < 0) {
    count = GetUserChoice("Введите число итераций (0 для выхода):\n");
  }
  return count;
}

int ConsoleView::GetThreadsCount() const {
  int count = -1;
  while (
      !(count == 0 || count == 1 || count == 2 || count == 4 || count == 6)) {
    count = GetUserChoice("Введите число потоков:\n");
    if (count % 2 != 0) {
      ShowMsg("Число потоков должно быть четным.");
    }
  }
  return count;
}

void ConsoleView::ShowMsg(const std::string &msg) const {
  std::cout << msg << std::endl;
}

void ConsoleView::DisplayHeader() const {
  system("clear");
  std::cout << "============" << std::endl;
  std::cout << "  М Е Н Ю   " << std::endl;
  std::cout << "============" << std::endl;
}

void ConsoleView::DisplayInit() const {
  DisplayHeader();
  std::cout << "1. Проблема коммивояжера" << std::endl;
  std::cout << "2. Решение СЛАУ (метод Гаусса)" << std::endl;
  std::cout << "3. Умножение матриц по Винограду" << std::endl;
  std::cout << "0. Выход" << std::endl << std::endl;
}

void ConsoleView::DisplaySalesmanMenu() const {
  DisplayHeader();
  std::cout << "3. Загрузите матрицу весов из файла" << std::endl;
  std::cout << "0. Выход" << std::endl << std::endl;
}

void ConsoleView::DisplayGaussMenu() const {
  DisplayHeader();
  std::cout << "Алгорим Гаусса" << std::endl;
  std::cout << "1. Ввести матрицу" << std::endl;
  std::cout << "2. Сгенерировать случайную матрицу" << std::endl;
  std::cout << "0. Выход" << std::endl << std::endl;
}

void ConsoleView::DisplayVinogradovMenu() const {
  DisplayHeader();
  std::cout << "Перемножение матриц" << std::endl;
  std::cout << "1. Пользовательский ввод матриц" << std::endl;
  std::cout << "2. Случайные матрицы" << std::endl;
  std::cout << "0. Выход" << std::endl << std::endl;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int InitShower::Display() const {
  view_->DisplayInit();
  int choice = ConsoleView::Choice::kBttnEnd;
  while (choice < ConsoleView::Choice::kExit ||
         choice >= ConsoleView::Choice::kBttnEnd) {
    choice = view_->GetUserChoice("Выберите режим");
  }
  return choice;
}

int SalesmanShower::Display() const {
  view_->DisplaySalesmanMenu();
  int choice = ConsoleView::Choice::kBttnEnd;
  while (choice != ConsoleView::Choice::kExit &&
         choice != ConsoleView::Choice::kBttnThree) {
    choice = view_->GetUserChoice("Выберите режим");
  }
  return choice;
}

int GaussShower::Display() const {
  view_->DisplayGaussMenu();
  int choice = ConsoleView::Choice::kBttnEnd;
  while (choice < ConsoleView::Choice::kExit ||
         choice >= ConsoleView::Choice::kBttnEnd) {
    choice = view_->GetUserChoice("Выберите режим");
  }
  return choice;
}

int VinogradovShower::Display() const {
  view_->DisplayVinogradovMenu();
  int choice = ConsoleView::Choice::kBttnEnd;
  while (choice < ConsoleView::Choice::kExit ||
         choice >= ConsoleView::Choice::kBttnThree) {
    choice = view_->GetUserChoice("Выберите режим");
  }
  return choice;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

m_dbl_type UserFiller::FillMatrix() {
  view_->ShowMsg("Введите матрицу:");
  auto rows = view_->GetUserChoice("Введите число строк");
  auto cols = view_->GetUserChoice("Введите число столбцов");
  std::vector<std::vector<double>> matrix;
  for (int i = 0; i < rows; ++i) {
    view_->ShowMsg("Заполните ряд");
    std::vector<double> one_row = view_->GetVector(cols);
    matrix.push_back(one_row);
  }
  return matrix;
}

m_dbl_type RandomFiller::FillMatrix() {
  auto rows = view_->GetUserChoice("Введите число строк");
  auto cols = view_->GetUserChoice("Введите число столбцов");
  return s21::Storage::FillMatrixRandomly(rows, cols);
}

m_dbl_type LoadFiller::FillMatrix() {
  auto file = view_->GetString("Введите путь к файлу");
  return s21::Storage::FillMatrixFromFile(file);
}

} // namespace s21
