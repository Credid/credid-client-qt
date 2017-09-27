#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>

#include "ConnectionDialog.hpp"
#include "ui_ConnectionDialog.h"
#include "MainWindow.hpp"

ConnectionDialog::ConnectionDialog(MainWindow *mainWindow) :
  QDialog(mainWindow),
  ui(new Ui::ConnectionDialog) {
  parent = mainWindow;
  ui->setupUi(this);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConnectionDialog::connectToServer);

  connect(ui->favFill, &QPushButton::clicked, this, &ConnectionDialog::fillFavorite);
  connect(ui->favAdd, &QPushButton::clicked, this, &ConnectionDialog::addFavorite);
  connect(ui->favRemove, &QPushButton::clicked, this, &ConnectionDialog::removeFavorite);

  // Check that the favorites folder exists
  if (!QDir(QDir::homePath() + "/.local").exists())
    QDir().mkdir(QDir::homePath() + "/.local");
  // Open save file and fill with first favorite
  QFile saveFile(QDir::homePath() + "/.local/favorites.csv");
  if (!saveFile.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  QTextStream in(&saveFile);
  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList splitLine = line.split(",");
    displayFavorite(splitLine.at(0), splitLine.at(1), splitLine.at(2));
    if (ui->favList->rowCount() == 1) {
      ui->favList->selectRow(0);
      fillFavorite();
    }
  }
  saveFile.close();
}

ConnectionDialog::~ConnectionDialog() {

}

void ConnectionDialog::connectToServer() {
  parent->initializeApi(ui->host->text(), ui->port->text(), ui->username->text(), ui->password->text());
  close();
}

void ConnectionDialog::fillFavorite() {
  ui->host->setText(ui->favList->selectedItems().first()->text());
  ui->port->setText(ui->favList->selectedItems().at(1)->text());
  ui->username->setText(ui->favList->selectedItems().back()->text());
}

void ConnectionDialog::displayFavorite(QString const &host, QString const &port, QString const &username) {
  ui->favList->insertRow(ui->favList->rowCount());
  ui->favList->setItem(ui->favList->rowCount() - 1, 0, new QTableWidgetItem(host));
  ui->favList->setItem(ui->favList->rowCount() - 1, 1, new QTableWidgetItem(port));
  ui->favList->setItem(ui->favList->rowCount() - 1, 2, new QTableWidgetItem(username));
}

void ConnectionDialog::addFavorite() {
  // Add in save file
  QString toAdd = ui->host->text() + "," + ui->port->text() + "," + ui->username->text() + "\n";

  QFile saveFile(QDir::homePath() + "/.local/favorites.csv");
  if (!saveFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
    // Create file
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
      return;
  }

  QTextStream out(&saveFile);
  while (!out.atEnd()) {
    QString line = out.readLine();
    if (line == toAdd) // Avoid duplicates
      return;
  }
  out << toAdd;
  saveFile.close();

  displayFavorite(ui->host->text(), ui->port->text(), ui->username->text());
}

void ConnectionDialog::removeFavorite() {
  // Remove from save file
  QString toErase = ui->favList->selectedItems().first()->text() + "," + ui->favList->selectedItems().at(1)->text() + "," + ui->favList->selectedItems().back()->text();

  QFile saveFile(QDir::homePath() + "/.local/favorites.csv");
  if (!saveFile.open(QIODevice::ReadWrite | QIODevice::Text))
    return;

  QTextStream io(&saveFile);
  QString content;
  while (!io.atEnd()) {
    QString line = io.readLine();
    // Append content to string except for line to be erased
    if (line != toErase)
      content.append(line + "\n");
  }
  // Replace content of savefile
  saveFile.resize(0);
  io << content;
  saveFile.close();

  if (ui->favList->selectedItems().empty())
    return;
  ui->favList->removeRow(ui->favList->selectedItems().first()->row());
}
