#ifndef PARALLELS_SRC_VIEW_CONSOLE_VIEW_H_
#define PARALLELS_SRC_VIEW_CONSOLE_VIEW_H_

#include <iostream>

#include "../controller/s21_controller.h"

/// @brief
namespace s21 {

class Controller;
class ViewShower;
class MatrixFiller;

/// @brief
class ConsoleView {

public:
  /// @brief enmeration for choice of controller actions
  enum Choice { kExit, kBttnOne, kBttnTwo, kBttnThree, kBttnEnd };
  /// @brief enum for swithing between view modes
  enum Mode { kInitMode, kSaleMode, kGaussMode, kVinoMode, kEndMode };
  /// @brief enum for swiching between filling modes.
  enum FillingMode { kRandFill, kUserFill, kLoadFill, kEndFill };

  /// @brief default ctor.
  ConsoleView(){};
  /// @brief sets strategy for "Show" method. Class ViewShower is used
  /// to control showing messages and return value boundaries.
  /// @param mode - enum for swithing between view modes
  void SetStrategy(Mode mode);
  /// @brief shows different messages according to view mode and returns user
  /// choice
  /// @return user choice
  int Show() const;
  /// @brief adjust the process of getting input data from user
  /// @param fill_mode - enum for swiching between filling modes
  void SetFillingStrategy(FillingMode fill_mode);

  /// @brief displays initial menu
  void DisplayInit() const;
  /// @brief displays menu for Salesman algorithm
  void DisplaySalesmanMenu() const;
  /// @brief displays menu for Gauss algorithm
  void DisplayGaussMenu() const;
  /// @brief displays menu for Vinogradov algorithm
  void DisplayVinogradovMenu() const;
  /// @brief displays header
  void DisplayHeader() const;

  /// @brief returns int number from user
  /// @param  msg - message for user
  /// @return integer number
  int GetUserChoice(const std::string &msg) const;

  /// @brief returns string from user
  /// @param msg - message for user
  /// @return any string
  std::string GetString(const std::string &msg) const;

  /// @brief returns vector of double values from user
  /// @param count - size of vector to be returned
  /// @return std::vector<double>
  row_type GetVector(const int count) const;
  /// @brief returns matrix of double values from user. Size of matrix is
  /// defined inside function
  m_dbl_type GetMatrix() const;
  /// @brief returns non negative integer number from user
  /// @return integer number
  int GetIterationsCount() const;
  /// @brief returns non negative integer number from user (0, 1 or multiple of
  /// 2)
  /// @return integer number: 0, 1, 2, 4 or 6
  int GetThreadsCount() const;
  /// @brief just shows the message
  /// @param msg - message
  void ShowMsg(const std::string &msg) const;

  /// @brief just output the matrix. It is responsibility of developer to check
  /// that type T has methods for stream output
  /// @param result - matrix to output
  template <typename T>
  void ShowMatrix(const std::vector<std::vector<T>> &result) const {
    for (auto &row : result) {
      for (auto &value : row) {
        std::cout << value << " ";
      }
      std::cout << std::endl;
    }
  }
  /// @brief just output the vector. It is responsibility of developer to check
  /// that type T has methods for stream output
  /// @param result - vector to output
  template <typename T> void ShowVector(const std::vector<T> &result) const {
    for (auto &value : result) {
      std::cout << value << " ";
    }
    std::cout << std::endl;
  }

private:
  std::shared_ptr<ViewShower> shower_;
  std::shared_ptr<MatrixFiller> filler_;
};
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief interface for showing different messages. Used to simplify work with
/// view
class ViewShower {
public:
  /// @brief ctor
  explicit ViewShower(ConsoleView *w) : view_(w){};
  virtual ~ViewShower() = default;

  /// @brief interface for showing different messages
  virtual int Display() const = 0;

protected:
  ConsoleView *view_;
};

class InitShower : public ViewShower {
public:
  explicit InitShower(ConsoleView *w) : ViewShower(w){};
  ~InitShower() = default;

  int Display() const override;
};

class SalesmanShower : public ViewShower {
public:
  explicit SalesmanShower(ConsoleView *w) : ViewShower(w){};
  ~SalesmanShower() = default;
  int Display() const override;
};

class GaussShower : public ViewShower {
public:
  explicit GaussShower(ConsoleView *w) : ViewShower(w){};
  ~GaussShower() = default;
  int Display() const override;
};

class VinogradovShower : public ViewShower {
public:
  explicit VinogradovShower(ConsoleView *w) : ViewShower(w){};
  ~VinogradovShower() = default;
  int Display() const override;
};

class VinoModeShower : public ViewShower {
public:
  explicit VinoModeShower(ConsoleView *w) : ViewShower(w){};
  ~VinoModeShower() = default;
  int Display() const override;
};
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/// @brief interface for getting different matrices (loading, or user input or
/// random). Used to adjust view's behavior in different methods of getting
/// input data
class MatrixFiller {
public:
  explicit MatrixFiller(ConsoleView *w) : view_(w){};
  virtual ~MatrixFiller() = default;

  virtual m_dbl_type FillMatrix() = 0;

protected:
  ConsoleView *view_;
};

class RandomFiller : public MatrixFiller {
public:
  explicit RandomFiller(ConsoleView *w) : MatrixFiller(w){};
  ~RandomFiller() = default;

  m_dbl_type FillMatrix() override;
};

class UserFiller : public MatrixFiller {
public:
  explicit UserFiller(ConsoleView *w) : MatrixFiller(w){};
  ~UserFiller() = default;

  m_dbl_type FillMatrix() override;
};

class LoadFiller : public MatrixFiller {
public:
  explicit LoadFiller(ConsoleView *w) : MatrixFiller(w){};
  ~LoadFiller() = default;

  m_dbl_type FillMatrix() override;
};

} // namespace s21

#endif // PARALLELS_SRC_VIEW_CONSOLE_VIEW_H_
