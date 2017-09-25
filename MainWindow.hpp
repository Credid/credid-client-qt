#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QListWidget>

#include "auth-api.h"
#include "ConnectionDialog.hpp"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void initializeApi(QString const &host, QString const &port, QString const &username, QString const &password);
  void listToDisplay(QListWidget *dest, bool clear = true);

public slots:
  void disconnect();

  // Users
  void displayUserInfo();

  void addUser();
  void removeUser();
  void changePassword();

  void addUserGroup();
  void removeUserGroup();

  // Groups
  void displayGroupInfo();

  void addGroup();
  void removeGroup();

  void removePermission();

private:
  Ui::MainWindow *ui;
  ConnectionDialog *coUi;
  auth_api_t *api;
};

#endif // MAINWINDOW_HPP
