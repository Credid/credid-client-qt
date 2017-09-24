#ifndef CONNECTIONWINDOW_HPP
#define CONNECTIONWINDOW_HPP

#include <QDialog>
#include <QMainWindow>

namespace Ui {
  class ConnectionDialog;
}

class ConnectionDialog : public QDialog {
public:
  ConnectionDialog(QMainWindow *mainWindow);
  ~ConnectionDialog();

private:
  Ui::ConnectionDialog *ui;
};

#endif // CONNECTIONWINDOW_HPP
