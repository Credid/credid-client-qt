#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  api(new auth_api_t)
{
  ui->setupUi(this);
//  connect(ui->actionConnect, &QAction::triggered, this, &MyClass::doSomeFunction); // replace last f by something that spawns the menu
}

MainWindow::~MainWindow()
{
  delete ui;
  delete api;
}
