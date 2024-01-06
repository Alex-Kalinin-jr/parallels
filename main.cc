#include "controller/s21_controller.h"
#include "lib/s21_storage.h"
#include "view/s21_console_view.h"

int main() {

  std::shared_ptr<s21::ConsoleView> view(new s21::ConsoleView());
  std::shared_ptr<s21::Controller> controller(new s21::Controller(view));
  while (true) {
    if (!controller->RecieveInitialSignal()) {
      break;
    }
  }
  return 0;
}
