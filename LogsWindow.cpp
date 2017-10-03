#include <QDir>

#include "LogsWindow.hpp"
#include "MainWindow.hpp"
#include "ui_LogsWindow.h"

LogsWindow::LogsWindow(MainWindow *mainWindow) :
  QDialog(mainWindow),
  ui(new Ui::LogsWindow) {
  parent = mainWindow;
  ui->setupUi(this);

  // Check that the logs folder exists, else create it
  if (!QDir(QDir::homePath() + "/.local").exists())
    QDir().mkdir(QDir::homePath() + "/.local");
}

LogsWindow::~LogsWindow() {
  delete ui;
}

QListWidget *LogsWindow::getAllLogs() {
  return ui->listAllLogs;
}

QListWidget *LogsWindow::getLogs() {
  return ui->listLogs;
}
