#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {
  coUi = new ConnectionDialog(this);
  ui->setupUi(this);
  connect(ui->actionConnect, SIGNAL(triggered()), coUi, SLOT(exec()));
}

MainWindow::~MainWindow() {
  delete ui;
  delete coUi;
  if (api != NULL) {
    auth_api_free(api);
//    delete api;
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
  char *users = auth_api_last_result(api);
  char *buffer = strtok(users, "\"");
  while (buffer != NULL) {
    buffer = strtok(NULL, "\"");
    ui->listUsers->addItem(buffer);
    buffer = strtok(NULL, "\"");
  }

  // Request list of groups
  auth_api_group_list(api);
  char *groups = auth_api_last_result(api);
  buffer = strtok(groups, "\"");
  while (buffer != NULL) {
    buffer = strtok(NULL, "\"");
    ui->listGroups->addItem(buffer);
    buffer = strtok(NULL, "\"");
  }
}
