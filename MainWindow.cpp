#include "MainWindow.hpp"
#include "ui_MainWindow.h"

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
  delete api;
}

void MainWindow::initializeApi(QString host, QString port) {
  api = auth_api_init(host.toStdString().c_str(), port.toInt());
}
