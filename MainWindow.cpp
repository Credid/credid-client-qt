#include <QDesktopServices>
#include <QUrl>
#include <QDate>
#include <QTime>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>

#include "MainWindow.hpp"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow) {
  coUi = new ConnectionDialog(this);
  logsUi = new LogsWindow(this);
  ui->setupUi(this);
  ui->connectedMessage->setText("Disconnected.");

  // Connect menu buttons
  connect(ui->actionConnect, SIGNAL(triggered()), coUi, SLOT(exec()));
  connect(ui->actionOpen_logs, SIGNAL(triggered()), logsUi, SLOT(exec()));
  connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::disconnect);
  connect(ui->actionRFC, &QAction::triggered, this, &MainWindow::openRFC);
  connect(ui->actionWiki, &QAction::triggered, this, &MainWindow::openWiki);
  connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);

  // Connect users pannel buttons
  connect(ui->listUsers, &QListWidget::clicked, this, &MainWindow::displayUserInfo);

  connect(ui->newUser, &QPushButton::clicked, this, &MainWindow::addUser);
  connect(ui->removeUser, &QPushButton::clicked, this, &MainWindow::removeUser);
  connect(ui->setPassword, &QPushButton::clicked, this, &MainWindow::changePassword);

  connect(ui->addUserGroup, &QPushButton::clicked, this, &MainWindow::addUserGroup);
  connect(ui->removeUserGroup, &QPushButton::clicked, this, &MainWindow::removeUserGroup);

  // Group pannel buttons
  connect(ui->listGroups, &QListWidget::clicked, this, &MainWindow::displayGroupInfo);

  connect(ui->newGroup, &QPushButton::clicked, this, &MainWindow::addGroup);
  connect(ui->removeGroup, &QPushButton::clicked, this, &MainWindow::removeGroup);
  connect(ui->removePermission, &QPushButton::clicked, this, &MainWindow::removePermission);
}

MainWindow::~MainWindow() {
  delete ui;
  delete coUi;
  if (api != NULL) {
    credid_api_free(api);
  }
}

void MainWindow::initializeApi(QString const &host, QString const &port, QString const &username, QString const &password) {
  // Connect to the server
  api = credid_api_init(host.toStdString().c_str(), port.toInt());
  if (api == NULL) {
    ui->errorMessage->setText("Could not connect to the server.");
    addLog(false, "Could not connect to the server.");
    return;
  } else {
    ui->errorMessage->setText("");
  }

  // authenticate
  credid_api_auth(api, username.toStdString().c_str(), password.toStdString().c_str());
  if (!credid_api_success(api)) {
    ui->errorMessage->setText("Authentication failed: invalid username and/or password");
    addLog(false, "Authentication failed: invalid username and/or password");
    credid_api_free(api);
    api = NULL;
    return;
  } else {
    ui->errorMessage->setText("");
  }

  credid_api_setup_logs(api, 1);

  // Request list of users
  credid_api_user_list(api);
  listToDisplay(ui->listUsers);
  if (ui->listUsers->count() > 0) {
    ui->listUsers->item(0)->setSelected(true);
    displayUserInfo();
  }

  // Request list of groups
  credid_api_group_list(api);
  listToDisplay(ui->listGroups);
  if (ui->listGroups->count() > 0) {
    ui->listGroups->item(0)->setSelected(true);
    displayGroupInfo();
  }

  // Enable UI
  ui->centralWidget->setEnabled(true);
  ui->connectedMessage->setText("Connected to " + host + ":" + port);
  addLog(false, "Connected to " + host.toStdString() + ":" + port.toStdString());
}

void MainWindow::disconnect() {
  if (api != NULL) {
    addLog("Disconnected from the server.");
    credid_api_free(api);
    api = NULL;
  }
  ui->centralWidget->setEnabled(false);
  ui->connectedMessage->setText("Disconnected.");

  // Clear UI
  for (auto widget: ui->centralWidget->findChildren<QListWidget*>())
    widget->clear();
  for (auto widget: ui->centralWidget->findChildren<QLineEdit*>())
    widget->clear();
  ui->removeUser->setText("Delete user");
  ui->removeGroup->setText("Delete group");
}

void MainWindow::openRFC() {
  QString link = "https://credid.sceptique.eu/v1.html";
  QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::openWiki() {
  QString link = "https://github.com/credidCr/qt-credid-api/wiki";
  QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::displayUserInfo() {
  // Empty lists
  ui->userGroupSelector->clear();

  // Refill lists
  credid_api_user_list_groups(api, ui->listUsers->selectedItems().first()->text().toStdString().c_str());
  listToDisplay(ui->listUserGroups);
  ui->removeUser->setText("Delete user " + ui->listUsers->selectedItems().first()->text());
  std::vector<QString> addedGroups;
  addedGroups.empty();
  // Only display in the "add group" box the groups the user is not in
  ui->listUserPermissions->clear();
  for (int i = 0; i < ui->listUserGroups->count(); i++) {
    addedGroups.push_back(ui->listUserGroups->item(i)->text());
    // Put perms in user's perms list
    credid_api_group_list_perms(api, ui->listUserGroups->item(i)->text().toStdString().c_str());
    listToDisplay(ui->listUserPermissions, false);
  }

  for (int i = 0; i < ui->listGroups->count(); i++) {
    QString s = ui->listGroups->item(i)->text();
    if (std::find(addedGroups.begin(), addedGroups.end(), s) == addedGroups.end()) {
      ui->userGroupSelector->addItem(s);
    }
  }
  addLog();
}

void MainWindow::addUser() {
  ui->errorMessage->setText("");
  credid_api_user_add(api, ui->newUser_Name->text().toStdString().c_str(), ui->newUser_Password->text().toStdString().c_str());
  if (credid_api_success(api)) {
    // Update user list
    ui->listUsers->addItem(ui->newUser_Name->text());
    ui->newUser_Name->setText("");
    ui->newUser_Password->setText("");
  } else {
    // Display error message
    QString log = credid_api_last_result(api);
    ui->errorMessage->setText(log);
  }
  addLog();
}

void MainWindow::removeUser() {
  if (QMessageBox::question( this, "Remove user", "Are you sure you want to remove this user?", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Cancel)
    return;
  ui->errorMessage->setText("");
  credid_api_user_remove(api, ui->listUsers->selectedItems().first()->text().toStdString().c_str());
  if (credid_api_success(api)) {
    // Update user list
    delete ui->listUsers->selectedItems().first();
  } else {
    // Display error message
    ui->errorMessage->setText(credid_api_last_result(api));
  }
  addLog();
}

#include <iostream>
void MainWindow::changePassword() {
  bool ok;
  QString pw = QInputDialog::getText(this, "Change user password", "Enter the new password again", QLineEdit::Password, "", &ok);
  if (!ok)
    return;
  if (pw != ui->newPassword->text()) {
    addLog(true, "Tried to change password but passwords mismatch");
    ui->errorMessage->setText("The two passwords don't match, please try again.");
    return;
  }

  ui->errorMessage->setText("");
  credid_api_user_change_password(api, ui->listUsers->selectedItems().first()->text().toStdString().c_str(), ui->newPassword->text().toStdString().c_str());
  if (credid_api_success(api)) {
    // Display OK
    ui->errorMessage->setText("Password successfully changed");
    ui->newPassword->setText("");
  } else {
    // Display error message
    ui->errorMessage->setText(credid_api_last_result(api));
  }
  addLog();
}

void MainWindow::addUserGroup() {
  ui->errorMessage->setText("");
  credid_api_user_add_group(api, ui->listUsers->selectedItems().first()->text().toStdString().c_str(), ui->userGroupSelector->currentText().toStdString().c_str());
  if (credid_api_success(api)) {
    // Update group list
    ui->listUserGroups->addItem(ui->userGroupSelector->currentText());
    ui->userGroupSelector->removeItem(ui->userGroupSelector->currentIndex());
  } else {
    // Display error message
    ui->errorMessage->setText(credid_api_last_result(api));
  }
  addLog();
}

void MainWindow::removeUserGroup() {
  ui->errorMessage->setText("");
  credid_api_user_remove_group(api, ui->listUsers->selectedItems().first()->text().toStdString().c_str(), ui->listUserGroups->selectedItems().first()->text().toStdString().c_str());
  if (credid_api_success(api)) {
    // Update group list
    ui->userGroupSelector->addItem(ui->listUserGroups->selectedItems().first()->text());
    delete ui->listUserGroups->selectedItems().first();
  } else {
    // Display error message
    ui->errorMessage->setText(credid_api_last_result(api));
  }
  addLog();
}

/* Groups management functions */

void MainWindow::displayGroupInfo() {
  ui->errorMessage->setText("");

  credid_api_group_list_perms(api, ui->listGroups->selectedItems().first()->text().toStdString().c_str());
  if (credid_api_success(api)) {
    // Update group list
    listToDisplay(ui->listGroupPermissions);
    ui->removeGroup->setText("Delete group " + ui->listGroups->selectedItems().first()->text());
  } else {
    // Display error message
    ui->errorMessage->setText(credid_api_last_result(api));
  }
  addLog();
}

void MainWindow::addGroup() {
  ui->errorMessage->setText("");
  credid_api_group_add(api, ui->newGroup_Name->text().toStdString().c_str(), ui->newGroup_Permission->text().toStdString().c_str(), ui->newGroup_Resource->text().toStdString().c_str());
  if (credid_api_success(api)) {
    // Update selected user
    if (!ui->listUsers->selectedItems().empty())
      displayUserInfo();

    // Update selected group
    if (!ui->listGroups->selectedItems().empty() && ui->newGroup_Name->text().toStdString() == ui->listGroups->selectedItems().first()->text().toStdString())
      ui->listGroupPermissions->addItem(ui->newGroup_Resource->text() + " => " + ui->newGroup_Permission->text());

    // Update "group add" interface
    ui->newGroup_Permission->setText("");
    ui->newGroup_Resource->setText("");

    // Handle case where the group already exists
    for (int i = 0; i < ui->listGroups->count(); i++) {
      if (ui->listGroups->item(i)->text() == ui->newGroup_Name->text()) {
        ui->newGroup_Name->setText("");
        return;
      }
    }
    ui->listGroups->addItem(ui->newGroup_Name->text());
    ui->newGroup_Name->setText("");
  } else {
    ui->errorMessage->setText(credid_api_last_result(api));
    credid_api_group_list(api);
    ui->listGroups->clear();
    listToDisplay(ui->listGroups);

    // Update selected user
    if (!ui->listUsers->selectedItems().empty())
      displayUserInfo();
  }
  addLog();
}

void MainWindow::removeGroup() {
  if (QMessageBox::question( this, "Delete group", "Are you sure you want to delete this group?\nAll users belonging to this group will be removed from it.", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Cancel)
    return;
  if (ui->listGroups->selectedItems().empty()) {
    ui->errorMessage->setText("Please select a group to delete");
    return;
  }
  ui->errorMessage->setText("");
  credid_api_group_remove(api, ui->listGroups->selectedItems().first()->text().toStdString().c_str(), "");
  if (credid_api_success(api)) {
    // Refresh list of users : remove them from group
    for (int i = 0; i < ui->listUsers->count(); i++)
      credid_api_user_remove_group(api, ui->listUsers->item(i)->text().toStdString().c_str(), ui->listGroups->selectedItems().first()->text().toStdString().c_str());
    if (!ui->listUsers->selectedItems().empty())
      displayUserInfo();
    credid_api_user_list(api);
    listToDisplay(ui->listUsers);

    // Update selected user
    if (!ui->listUsers->selectedItems().empty())
      displayUserInfo();

    // Update group list
    delete ui->listGroups->selectedItems().first();
    ui->listGroupPermissions->clear();
  } else {
    // Display error message
    ui->errorMessage->setText(credid_api_last_result(api));
  }
  addLog();
}

void MainWindow::removePermission() {
  if (ui->listGroups->selectedItems().count() == 0 || ui->listGroupPermissions->selectedItems().count() == 0) {
    ui->errorMessage->setText("Please select a group and a permission");
    return;
  }
  ui->errorMessage->setText("");
  credid_api_group_remove(api, ui->listGroups->selectedItems().first()->text().toStdString().c_str(), ui->listGroupPermissions->selectedItems().first()->text().split(" => ").first().toStdString().c_str());
  if (credid_api_success(api)) {
    // Update permissions display
    delete ui->listGroupPermissions->selectedItems().first();

    // Update permissions display for selected user
    displayUserInfo();
  } else {
    ui->errorMessage->setText(credid_api_last_result(api));
  }
  addLog();
}

void MainWindow::listToDisplay(QListWidget *dest, bool clear) {
  if (clear)
    dest->clear();
  QString result = credid_api_last_result(api);
  QStringList resultList = result.split("\"");
  for (QStringList::iterator it = resultList.begin(); it != resultList.end(); it++) {
    it++;
    if (it == resultList.end())
      return;
    if (result[0] == '{') {
      QString tmp = *it + " => ";
      it += 2;
      tmp += *it;
      dest->addItem(tmp);
    }
    else
      dest->addItem(*it);
  }
}

void MainWindow::addLog(bool userOp, std::string const &logPhrase) {
  QFile saveFile(QDir::homePath() + "/.local/logs");
  if (!saveFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
    // Create file
    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text))
      return;
  }

  credid_api_log_t *log;
  while ((log = credid_api_fetch_log(api)) != NULL) {
    QString toAdd = "[ " + QDate::currentDate().toString() + " " + QTime::currentTime().toString() + "] ";
    if (log->status) {
      toAdd += "[ERROR] ";
      toAdd += log->query;
      toAdd += " (";
      toAdd += credid_api_last_result(api);
      toAdd += ")";
    }
    else
      toAdd += log->query;
    toAdd.remove('\n');

    logsUi->getAllLogs()->addItem(toAdd);
    if (!userOp)
      logsUi->getLogs()->addItem(toAdd);

    // Add in log file
    QTextStream out(&saveFile);
    while (!out.atEnd())
      out.readLine();
    out << toAdd;

    // Handle disconnection case
    if (QString(credid_api_last_result(api)) == "not connected : failure\n") {
      disconnect();
      free(log->query);
      free(log);
      return;
    }

    free(log->query);
    free(log);
  }
  if (logPhrase != "") {
    QString toAdd = "[ " + QDate::currentDate().toString() + " " + QTime::currentTime().toString() + "] " + logPhrase.c_str();
    logsUi->getAllLogs()->addItem(toAdd);
    if (!userOp)
      logsUi->getLogs()->addItem(toAdd);

    // Add in log file
    QTextStream out(&saveFile);
    while (!out.atEnd())
      out.readLine();
    out << toAdd;
  }
  saveFile.close();
}
