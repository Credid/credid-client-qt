#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {
  coUi = new ConnectionDialog(this);
  ui->setupUi(this);
  connect(ui->actionConnect, SIGNAL(triggered()), coUi, SLOT(exec()));

  // Connect users pannel buttons
  connect(ui->listUsers, &QListWidget::clicked, this, &MainWindow::displayUserInfo);
  connect(ui->newUser, &QPushButton::clicked, this, &MainWindow::addUser);
}

MainWindow::~MainWindow() {
  delete ui;
  delete coUi;
  if (api != NULL) {
    auth_api_free(api);
  }
}

void MainWindow::initializeApi(QString const &host, QString const &port, QString const &username, QString const &password) {
  // Connect to the server
  api = auth_api_init(host.toStdString().c_str(), port.toInt());
  if (api == NULL) {
    ui->errorMessage->setText("Could not connect to the server.");
    return;
  } else {
    ui->errorMessage->setText("");
  }

  // Authenticate
  auth_api_auth(api, username.toStdString().c_str(), password.toStdString().c_str());
  if (!auth_api_success(api)) {
    ui->errorMessage->setText("Authentication failed: invalid username and/or password");
    auth_api_free(api);
    api = NULL;
    return;
  } else {
    ui->errorMessage->setText("");
  }

  // Request list of users
  auth_api_user_list(api);
  listToDisplay(ui->listUsers);

  // Request list of groups
  auth_api_group_list(api);
  listToDisplay(ui->listGroups);
}

void MainWindow::displayUserInfo() {
  // Empty list
  ui->listUserGroups->clear();

  // Refill list
  auth_api_user_list_group(api, ui->listUsers->selectedItems().first()->text().toStdString().c_str());
  listToDisplay(ui->listUserGroups);
  std::vector<QWidget*> addedGroups;
  for (int i = 0; i < ui->listGroups->count(); i++) {
    QWidget *w = ui->listUserGroups->find(i);
    if (std::find(addedGroups.begin(), addedGroups.end(), w) != addedGroups.end()) {
      ui->userGroupSelector->addItem(ui->listGroups->item(i)->text());
      addedGroups.push_back(w);
    }
  }
}

void MainWindow::addUser() {
  ui->errorMessage->setText("");
  auth_api_user_add(api, ui->newUser_Name->text().toStdString().c_str(), ui->newUser_Password->text().toStdString().c_str());
  if (auth_api_success(api)) {
    // Display new user in list
    ui->listUsers->addItem(ui->newUser_Name->text());
    ui->newUser_Name->setText("");
    ui->newUser_Password->setText("");
  } else {
    // Display error message
    ui->errorMessage->setText(auth_api_last_result(api));
  }
}

void MainWindow::listToDisplay(QListWidget *dest) {
  char *result = auth_api_last_result(api);
  char *buffer = strtok(result, "\"");
  while (buffer != NULL) {
    buffer = strtok(NULL, "\"");
    if (buffer != NULL)
      dest->addItem(buffer);
    buffer = strtok(NULL, "\"");
  }
}
