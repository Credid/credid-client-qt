#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  api(new auth_api_t) {
  coUi = new ConnectionDialog(this);
  ui->setupUi(this);
  connect(ui->actionConnect, SIGNAL(triggered()), coUi, SLOT(exec()));
}

MainWindow::~MainWindow() {
  delete ui;
  delete coUi;
  delete api;
}
