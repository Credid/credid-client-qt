#ifndef LOGSWINDOW_H
#define LOGSWINDOW_H

#include <QDialog>
#include <QListWidget>

class MainWindow;

namespace Ui {
  class LogsWindow;
}

class LogsWindow : public QDialog {
public:
  LogsWindow(MainWindow *mainWindow);
  ~LogsWindow();

  QListWidget *getAllLogs();
  QListWidget *getLogs();

private:
  Ui::LogsWindow *ui;
  MainWindow *parent;
};

#endif // LOGSWINDOW_H
