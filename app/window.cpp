#include "ui_statuswindow.h"

#include "window.h"
#include "settings.h"

#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QLineEdit>
#include <QSizePolicy>
#include <QStringList>

Window::Window(Settings *s, QObject *parent)
    : QMainWindow(dynamic_cast<QWidget *>(parent)), ui(new Ui::StatusWindow),
      titleString("ClipShare"), icon(":/icons/clipshare.png"), settings(s) {
  ui->setupUi(this);

  setupTrayMenu();
  fillFields();

  trayIcon->setIcon(icon);
  trayIcon->setToolTip(titleString);
  trayIcon->show();

  setWindowTitle(titleString);
  setWindowIcon(icon);
  setWindowFlags(Qt::Drawer);

  connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
          SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

  connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
  connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
  connect(quitAction, SIGNAL(triggered()), trayIcon, SLOT(hide()));
  connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

  connect(ui->pushButton_register, SIGNAL(pressed()), this,
          SLOT(processRegister()));
  connect(ui->pushButton_check, SIGNAL(pressed()), this, SLOT(processCheck()));

  connect(ui->pushButton_ok, SIGNAL(pressed()), this, SLOT(processOK()));
  connect(ui->pushButton_cancel, SIGNAL(pressed()), this,
          SLOT(processCancel()));
  connect(ui->pushButton_apply, SIGNAL(pressed()), this, SLOT(processApply()));

  connect(ui->pushButton_shutdown, SIGNAL(pressed()), quitAction,
          SLOT(trigger()));

  this->show();
}
Window::~Window() { delete ui; }

void Window::setupTrayMenu() {
  /*
   *  Create actions for the tray menu
   */
  minimizeAction = new QAction(tr("Mi&nimize"), this);
  restoreAction = new QAction(tr("&Restore"), this);
  quitAction = new QAction(tr("&Quit"), this);

  /*
   * Set up the tray icon menu
   */
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(minimizeAction);
  trayIconMenu->addAction(restoreAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  /*
   *  Add the menu to the tray icon
   */
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);
}
void Window::fillFields() {
  QString email = settings->getEmail();
  ui->lineEdit_email->setText(email);

  QString password = settings->getPassword();
  ui->lineEdit_password->setText(password);

  if (email.size() && password.size()) {
    ui->label_register->hide();
    ui->pushButton_register->hide();
  }

  int timeperiod = settings->getDoubleCopyPeriod();
  ui->lineEdit_interval->setText(QString::number(timeperiod));

  ui->progressBar_upload->setRange(0, progressResolution);
  ui->progressBar_upload->setValue(0);
  ui->progressBar_upload->hide();

  adjustSize();
}
void Window::processMessage(MessageType type, QString message) {
  switch (type) {
  case URL:
    ui->lineEdit_downloadLink->setText(message);
    showTrayMessage(tr("Download link available!"),
                    QSystemTrayIcon::Information);
    break;
  case Progress: {
    QStringList values = message.split("\t", QString::SkipEmptyParts);
    long current = values.at(0).toLong();
    long total = values.at(1).toLong();
    int value = ((float)current / (float)total) * progressResolution;

    ui->progressBar_upload->setValue(value);

    if (current == 0 && total == 0) {
      ui->progressBar_upload->hide();
      ui->progressBar_upload->setValue(0);
    } else {
      ui->progressBar_upload->show();
    }
  } break;

  case Login:
    if (message == "Correct") {
      showTrayMessage(tr("Logged in"), QSystemTrayIcon::Information);
      setLoginFields(false);
    } else {
      showTrayMessage(tr("Incorrect credentials"), QSystemTrayIcon::Warning);
      setLoginFields(true);
    }
    break;

  case Notification:
    showTrayMessage(message, QSystemTrayIcon::Information);
    break;

  case Error:
    showTrayMessage(message, QSystemTrayIcon::Critical);
    break;

  default:
    qDebug() << "Unknown notification type in StatusWindow::processNotification"
             << type << message;
    break;
  }
}
void Window::setLoginFields(bool val) {
  ui->lineEdit_email->setReadOnly(!val);
  ui->lineEdit_email->setEnabled(val);
  ui->lineEdit_password->setReadOnly(!val);
  ui->lineEdit_password->setEnabled(val);
  ui->pushButton_check->setEnabled(val);
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
  case QSystemTrayIcon::Trigger:
  // fall through
  case QSystemTrayIcon::DoubleClick:
    this->showNormal();
    break;
  default:
    break;
  }
}
void Window::processRegister() {
  QString webLocation = settings->getHostname();
  QDesktopServices::openUrl(QUrl("https://" + webLocation));
}
void Window::processCheck() {
  if (applyAccount() == false) {
    showTrayMessage("Invalid credentials");
  } else {
    emitCommand(Connect, "CheckCredentials");
  }
}

void Window::processOK() {
  if (processApply()) {
    this->hide();
  }
}
void Window::processCancel() {
  this->hide();
  fillFields();
}
bool Window::applyAccount() {
  bool correct = true;
  setTextStatus(ui->lineEdit_email, true);
  setTextStatus(ui->lineEdit_password, true);

  QString email = ui->lineEdit_email->text();
  if (settings->setEmail(email) == false) {
    correct = false;
    ui->lineEdit_email->setText(tr("Invalid email address"));
    showTrayMessage(tr("Invalid email address"));
    setTextStatus(ui->lineEdit_email, false);
  }

  QString password = ui->lineEdit_password->text();
  if (settings->setPassword(password) == false) {
    correct = false;
    ui->lineEdit_password->setText(tr("Invalid password"));
    showTrayMessage(tr("Invalid password"));
    setTextStatus(ui->lineEdit_password, false);
  }

  return correct;
}
bool Window::applyGeneral() {
  bool correct = true;
  setTextStatus(ui->lineEdit_interval, true);

  QString copyTimePeriod = ui->lineEdit_interval->text();
  if (settings->setDoubleCopyPeriod(copyTimePeriod) == false) {
    correct = false;
    ui->lineEdit_interval->setText(tr("Invalid period"));
    showTrayMessage(tr("Invalid period"));
    setTextStatus(ui->lineEdit_interval, false);
  }

  return correct;
}

bool Window::processApply() {
  bool correct = applyAccount() && applyGeneral();
  if (correct) {
    settings->saveConfigToDisk();
  }
  return correct;
}
void Window::setTextStatus(QLineEdit *lineEdit, bool correct) {
  if (correct) {
    lineEdit->setStyleSheet("color:black");
  } else {
    lineEdit->setStyleSheet("color:red");
  }
}
void Window::showTrayMessage(QString msg,
                                   QSystemTrayIcon::MessageIcon messageIcon) {
  trayIcon->showMessage(titleString, msg, messageIcon);

  if(messageIcon == QSystemTrayIcon::Critical) {
      connect(trayIcon, &QSystemTrayIcon::messageClicked, &QApplication::quit);
  }
}
