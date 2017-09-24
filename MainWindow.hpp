#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

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

private:
  Ui::MainWindow *ui;
  ConnectionDialog *coUi;
  auth_api_t *api;
};

#endif // MAINWINDOW_HPP
