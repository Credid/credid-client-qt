#include "ConnectionDialog.hpp"
#include "ui_ConnectionDialog.h"

ConnectionDialog::ConnectionDialog(QMainWindow *mainWindow) :
  QDialog(mainWindow),
  ui(new Ui::ConnectionDialog) {
  ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog() {

}
