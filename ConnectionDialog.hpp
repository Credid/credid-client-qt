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

public slots:
  void fillFavorite();
  void addFavorite();
  void removeFavorite();

private:
  Ui::ConnectionDialog *ui;
  MainWindow *parent;
};

#endif // CONNECTIONWINDOW_HPP
