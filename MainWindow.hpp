#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QListWidget>

#include "credid-api.h"
#include "ConnectionDialog.hpp"
#include "LogsWindow.hpp"

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

  void addLog(bool userOp = false);

public slots:
  void openWiki();
  void openRFC();
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
  LogsWindow *logsUi;
  credid_api_t *api;
};

#endif // MAINWINDOW_HPP
