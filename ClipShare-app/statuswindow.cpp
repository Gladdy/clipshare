#include "ui_statuswindow.h"

#include "statuswindow.h"
#include "applicationsettings.h"

#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QLineEdit>

StatusWindow::StatusWindow(ApplicationSettings * s, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatusWindow),
    titleString("ClipShare"),
    icon(":/icons/clipshare.png"),
    settings(s)
{
    ui->setupUi(this);
    setupTrayMenu();
    fillFields();

    trayIcon->setIcon(icon);
    trayIcon->setToolTip(titleString);
    trayIcon->setVisible(true);
    trayIcon->show();

    setWindowTitle(titleString);
    setWindowIcon(icon);
    setWindowFlags(Qt::Drawer);

    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    connect(ui->pushButton_register,SIGNAL(pressed()),this,SLOT(processRegister()));
    connect(ui->pushButton_ok,SIGNAL(pressed()),this,SLOT(processOK()));
    connect(ui->pushButton_cancel,SIGNAL(pressed()),this,SLOT(processCancel()));
    connect(ui->pushButton_apply,SIGNAL(pressed()),this,SLOT(processApply()));
}

StatusWindow::~StatusWindow() { delete ui; }

void StatusWindow::setupTrayMenu()
{
    /*
     *  Create actions for the tray menu
     */
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    restoreAction = new QAction(tr("&Restore"), this);
    quitAction = new QAction(tr("&Quit"), this);

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

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
void StatusWindow::fillFields()
{
    QString email = settings->getSetting("email").toString();
    ui->lineEdit_email->setText(email);

    QString password = settings->getSetting("password").toString();
    ui->lineEdit_password->setText(password);

    if(email.size() && password.size()) {
        ui->label_register->hide();
        ui->pushButton_register->hide();
    }

    int uploadlimit = settings->getSetting("uploadSizeLimit").toInt();
    ui->lineEdit_maxsize->setText(QString::number(uploadlimit));

    int timeperiod = settings->getSetting("copyTimePeriod").toInt();
    ui->lineEdit_interval->setText(QString::number(timeperiod));
}

void StatusWindow::processNotification(QString str, QString msg)
{
    showTrayMessage(str + "\t" + msg,QSystemTrayIcon::Information);
}
void StatusWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //fall through
    case QSystemTrayIcon::DoubleClick:
        this->showNormal();
        break;
    default:
        break;
    }
}
void StatusWindow::processRegister()
{
    QDesktopServices::openUrl(QUrl("http://developer.qt.nokia.com"));
}
void StatusWindow::processOK()
{
    if(processApply()) {
        this->hide();
    }
}
void StatusWindow::processCancel()
{
    this->hide();
    fillFields();
}
bool StatusWindow::processApply()
{
    bool correct = true;

    /*
     *  Email
     */
    QString email = ui->lineEdit_email->text();

    if(settings->validateEmail(email) == false)
    {
        correct = false;
        ui->lineEdit_email->setText(tr("Invalid email address"));
        showTrayMessage(tr("Invalid email address"));
        setError(ui->lineEdit_email);
    }
    else
    {
        settings->setSetting("email",email);
        setCorrect(ui->lineEdit_email);
    }

    /*
     *  Password
     */
    QString password = ui->lineEdit_password->text();

    if(password.size() == 0)
    {
        correct = false;
        ui->lineEdit_password->setText(tr("Invalid password"));
        showTrayMessage(tr("Invalid password"));
        setError(ui->lineEdit_password);
    }
    else
    {
        settings->setSetting("password",password);
        setCorrect(ui->lineEdit_password);
    }
    /*
     *  uploadSizeLimit
     */
    QString uploadSizeLimit = ui->lineEdit_maxsize->text();
    if(settings->validateNumber(uploadSizeLimit, 1, 20000) == false)
    {
        correct = false;
        ui->lineEdit_maxsize->setText(tr("Invalid size"));
        showTrayMessage(tr("Invalid maximum size"));
        setError(ui->lineEdit_maxsize);
    }
    else
    {
        settings->setSetting("uploadSizeLimit",uploadSizeLimit.toInt());
        setCorrect(ui->lineEdit_maxsize);
    }

    /*
     *  copyTimePeriod
     */
    QString copyTimePeriod = ui->lineEdit_interval->text();
    if(settings->validateNumber(copyTimePeriod, 100, 5000) == false)
    {
        correct = false;
        ui->lineEdit_interval->setText(tr("Invalid period"));
        showTrayMessage(tr("Invalid period"));
        setError(ui->lineEdit_interval);
    }
    else
    {
        settings->setSetting("copyTimePeriod",copyTimePeriod.toInt());
        setCorrect(ui->lineEdit_interval);
    }

    if(correct)
    {
        settings->saveConfigToDisk();
    }
    else
    {
        qDebug() << "something was wrong";
    }

    return correct;
}
void StatusWindow::setError(QLineEdit* lineEdit)
{
    lineEdit->setStyleSheet("color:red");
}
void StatusWindow::setCorrect(QLineEdit* lineEdit)
{
    lineEdit->setStyleSheet("color:black");
}
void StatusWindow::showTrayMessage(QString msg, QSystemTrayIcon::MessageIcon messageIcon)
{
    trayIcon->showMessage(titleString, msg, messageIcon);
}
