#ifndef CONNECTIONWINDOW_HPP
#define CONNECTIONWINDOW_HPP

#include <QDialog>

class MainWindow;

namespace Ui {
  class ConnectionDialog;
}

class ConnectionDialog : public QDialog {
public:
  ConnectionDialog(MainWindow *mainWindow);
  ~ConnectionDialog();
  void accept();

private:
  Ui::ConnectionDialog *ui;
  MainWindow *parent;
};

#endif // CONNECTIONWINDOW_HPP
