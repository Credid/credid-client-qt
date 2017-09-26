#include "ConnectionDialog.hpp"
#include "ui_ConnectionDialog.h"
#include "MainWindow.hpp"

ConnectionDialog::ConnectionDialog(MainWindow *mainWindow) :
  QDialog(mainWindow),
  ui(new Ui::ConnectionDialog) {
  parent = mainWindow;
  ui->setupUi(this);

  // TODO : open save file and fill with first favorite

  connect(ui->favFill, &QPushButton::clicked, this, &ConnectionDialog::fillFavorite);
  connect(ui->favAdd, &QPushButton::clicked, this, &ConnectionDialog::addFavorite);
  connect(ui->favRemove, &QPushButton::clicked, this, &ConnectionDialog::removeFavorite);
}

ConnectionDialog::~ConnectionDialog() {

}

void ConnectionDialog::accept() {
  parent->initializeApi(ui->host->text(), ui->port->text(), ui->username->text(), ui->password->text());
  close();
}

void ConnectionDialog::fillFavorite() {
  ui->host->setText(ui->favList->selectedItems().first()->text());
  ui->port->setText(ui->favList->selectedItems().at(1)->text());
  ui->username->setText(ui->favList->selectedItems().back()->text());
}

void ConnectionDialog::addFavorite() {
  // TODO : also add in save file
  ui->favList->insertRow(ui->favList->rowCount());
  ui->favList->setItem(ui->favList->rowCount() - 1, 0, new QTableWidgetItem(ui->host->text()));
  ui->favList->setItem(ui->favList->rowCount() - 1, 1, new QTableWidgetItem(ui->port->text()));
  ui->favList->setItem(ui->favList->rowCount() - 1, 2, new QTableWidgetItem(ui->username->text()));
}

void ConnectionDialog::removeFavorite() {
  // TODO : also remove from save file
  if (ui->favList->selectedItems().empty())
    return;
  ui->favList->removeRow(ui->favList->selectedItems().first()->row());
}
