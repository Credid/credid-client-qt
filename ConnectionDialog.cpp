#include "ConnectionDialog.hpp"
#include "ui_ConnectionDialog.h"
#include "MainWindow.hpp"

ConnectionDialog::ConnectionDialog(MainWindow *mainWindow) :
  QDialog(mainWindow),
  ui(new Ui::ConnectionDialog) {
  parent = mainWindow;
  ui->setupUi(this);
}

ConnectionDialog::~ConnectionDialog() {

}

void ConnectionDialog::accept() {
  parent->initializeApi(ui->host->text(), ui->port->text(), ui->username->text(), ui->password->text());
  close();
}
