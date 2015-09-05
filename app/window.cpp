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
#include <QDesktopWidget>

Window::Window(Settings *s, QObject *parent)
    : QMainWindow(dynamic_cast<QWidget *>(parent)),
      ui(new Ui::StatusWindow),
      titleString("Clipshare"),
      icon(":/icons/clipshare.png"),
      settings(s)
{
  ui->setupUi(this);

  minimizeAction = new QAction(tr("Mi&nimize"), this);
  helpAction = new QAction(tr("&Help"), this);
  settingsAction = new QAction(tr("&Settings"), this);
  quitAction = new QAction(tr("&Quit Clipshare"), this);

  //Create a tray icon menu
  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(minimizeAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(helpAction);
  trayIconMenu->addAction(settingsAction);
  trayIconMenu->addSeparator();
  trayIconMenu->addAction(quitAction);

  //Add the menu to the tray icon
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setContextMenu(trayIconMenu);

  //Set up the icon
  trayIcon->setIcon(icon);
  trayIcon->setToolTip(titleString);
  trayIcon->show();

  //Set up the window
  this->setWindowTitle(titleString);
  this->setWindowIcon(icon);
  this->setWindowFlags(Qt::Drawer);

  //Display the window whenever someone uses left or middle mouse on the icon
  connect(trayIcon, &QSystemTrayIcon::activated, this, &Window::iconActivated);

  connect(minimizeAction, &QAction::triggered, this, &Window::hide);
  connect(settingsAction, &QAction::triggered, this, &Window::forceRestore);
  connect(quitAction, &QAction::triggered, trayIcon, &QSystemTrayIcon::hide);
  connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

  connect(ui->register_pushbutton, &QPushButton::pressed, this, &Window::processRegister);
  connect(ui->login_pushbutton, &QPushButton::pressed, this, &Window::processLogin);

  connect(ui->pushButton_ok, &QPushButton::pressed, this, &Window::processOK);
  connect(ui->pushButton_cancel, &QPushButton::pressed, this, &Window::processCancel);
  connect(ui->pushButton_apply, &QPushButton::pressed, this, &Window::processApply);

  connect(ui->pushButton_shutdown, &QPushButton::pressed, quitAction, &QAction::trigger);

  this->show();
  this->adjustSize();
  move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
}
Window::~Window() { delete ui; }

void Window::forceRestore() {
  this->show();
  this->activateWindow();
  this->raise();
}

void Window::fillFields() {

  //Account page
  QString email = settings->getSetting("email").toString();
  ui->email_lineEdit->setText(email);

  QString password = settings->getSetting("password").toString();
  ui->password_lineEdit->setText(password);

  //General page
  int maxsize = settings->getSetting("maxsize").toInt();
  ui->maxsize_lineEdit->setText(QString::number(maxsize));

  int triggerperiod = settings->getSetting("triggerperiod").toInt();
  ui->triggerperiod_lineEdit->setText(QString::number(triggerperiod));

  bool localstorage = settings->getSetting("localstorage").toBool();
  ui->localstorage_checkbox->setChecked(localstorage);

  bool notifications = settings->getSetting("notifications").toBool();
  ui->notification_checkbox->setChecked(notifications);

  ui->progressbar->setRange(0, progressResolution);
  ui->progressbar->setValue(0);
  ui->progressbar->hide();

  adjustSize();
}
void Window::processMessage(MessageType type, QString message) {
  switch (type) {
  case URL:
    ui->downloadlink_lineEdit->setText(message);
    showTrayMessage(tr("Download link available!"),
                    QSystemTrayIcon::Information);
    break;
  case Progress: {
    QStringList values = message.split("\t", QString::SkipEmptyParts);
    long current = values.at(0).toLong();
    long total = values.at(1).toLong();
    int value = ((float)current / (float)total) * progressResolution;

    ui->progressbar->setValue(value);

    if (current == 0 && total == 0) {
      ui->progressbar->hide();
      ui->progressbar->setValue(0);
    } else {
      ui->progressbar->show();
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
  }
}
void Window::setLoginFields(bool val) {
  ui->email_lineEdit->setReadOnly(!val);
  ui->email_lineEdit->setEnabled(val);
  ui->password_lineEdit->setReadOnly(!val);
  ui->password_lineEdit->setEnabled(val);
  ui->login_pushbutton->setEnabled(val);
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
  case QSystemTrayIcon::Trigger:
    //Simple click
    break;
  case QSystemTrayIcon::DoubleClick:
    this->forceRestore();
    break;
  case QSystemTrayIcon::MiddleClick:
    this->forceRestore();
    break;
  default:
    break;
  }
}
void Window::processRegister() {
  QString webLocation = settings->getSetting("hostname").toString();
  QDesktopServices::openUrl(QUrl("https://" + webLocation));
}
void Window::processLogin() {
  emitCommand(Connect, "CheckCredentials");
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
bool Window::processApply() {

  bool correct = true;
  bool check = true;

  //Check and save the maxsize
  check = true;
  QString maxsizeStr = ui->maxsize_lineEdit->text();
  int maxsize = maxsizeStr.toInt(&check);
  if(!check || maxsize <= 0 || maxsize > settings->getSetting("maxsize_max").toInt()) {
    setTextStatus(ui->maxsize_lineEdit, false);
    correct = false;
  } else {
    settings->saveSetting("maxsize",maxsize);
  }

  //Check and save the triggerperiod
  check = true;
  QString triggerperiodStr = ui->triggerperiod_lineEdit->text();
  int triggerperiod = triggerperiodStr.toInt(&check);
  if(!check || maxsize <= 0) {
    setTextStatus(ui->triggerperiod_lineEdit, false);
    correct = false;
  } else {
    settings->saveSetting("triggerperiod",triggerperiod);
  }

  //Check and save the checkboxes
  settings->saveSetting("localstorage",ui->localstorage_checkbox->isChecked());
  settings->saveSetting("notifications",ui->notification_checkbox->isChecked());

  return correct;
}

void Window::setTextStatus(QLineEdit *lineEdit, bool correct) {
  if (correct) {
    lineEdit->setStyleSheet("color:black");
  } else {
    lineEdit->setStyleSheet("color:red");
  }
}

void Window::showTrayMessage(QString msg, QSystemTrayIcon::MessageIcon messageIcon) {

  // Don't show anything anymore when a critical error has been raised already
  if(gotCriticalError) { return; }

  // Register the critical error and
  if(messageIcon == QSystemTrayIcon::Critical) {
    connect(trayIcon, &QSystemTrayIcon::messageClicked, &QApplication::quit);
    gotCriticalError = true;
  }

  trayIcon->showMessage(titleString, msg, messageIcon);
}
