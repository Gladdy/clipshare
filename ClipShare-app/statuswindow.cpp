#include "ui_statuswindow.h"

#include "statuswindow.h"
#include "applicationsettings.h"

#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QLineEdit>
#include <QSizePolicy>
#include <QStringList>

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
    connect(ui->pushButton_check, SIGNAL(pressed()), this, SLOT(processCheck()));

    connect(ui->pushButton_ok,SIGNAL(pressed()),this,SLOT(processOK()));
    connect(ui->pushButton_cancel,SIGNAL(pressed()),this,SLOT(processCancel()));
    connect(ui->pushButton_apply,SIGNAL(pressed()),this,SLOT(processApply()));

    connect(ui->pushButton_shutdown, SIGNAL(pressed()), qApp, SLOT(quit()));
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

    ui->progressBar_upload->setRange(0,progressResolution);
    ui->progressBar_upload->setValue(0);
    ui->progressBar_upload->hide();
}
void StatusWindow::processNotification(QString str, QString msg)
{
    if(str == "Error")
    {
        qDebug() << "Received systray message" << msg;
        showTrayMessage(str + "\t" + msg);
    }
    else if (str == "Progress")
    {
        ui->progressBar_upload->show();

        QStringList values = msg.split("\t", QString::SkipEmptyParts);
        long current = values.at(0).toLong();
        long total = values.at(1).toLong();
        int value = ((float)current/(float)total) * progressResolution;

        ui->progressBar_upload->setValue(value);

        if(current == 0 && total == 0)
        {
            showTrayMessage("Upload complete", QSystemTrayIcon::Information);
            ui->progressBar_upload->hide();
            ui->progressBar_upload->setValue(0);
        }
    }
    else if(str == "Login")
    {
        if(msg == "Correct")
        {
            showTrayMessage(tr("Logged in"),QSystemTrayIcon::Information);
            setLoginEnabled(false);
        }
        else
        {
            showTrayMessage(tr("Incorrect credentials"),QSystemTrayIcon::Warning);
            setLoginEnabled(true);
        }
    }
    else if(str == "Notification")
    {
        showTrayMessage(msg,QSystemTrayIcon::Information);
    }
}
void StatusWindow::setLoginEnabled(bool val) {
    ui->lineEdit_email->setReadOnly(!val);
    ui->lineEdit_email->setEnabled(val);
    ui->lineEdit_password->setReadOnly(!val);
    ui->lineEdit_password->setEnabled(val);
    ui->pushButton_check->setEnabled(val);
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
void StatusWindow::processCheck()
{
    if(applyAccount() == false)
    {
        showTrayMessage("Invalid credentials");
    }
    else
    {
        emitCommand("Connect","CheckCredentials");
    }
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
bool StatusWindow::applyAccount()
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

    return correct;
}
bool StatusWindow::applyGeneral()
{
    bool correct = true;

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

    return correct;
}

bool StatusWindow::processApply()
{
    bool correct = true;

    if(applyAccount() == false) { correct = false; }
    if(applyGeneral() == false) { correct = false; }
    if(correct) { settings->saveConfigToDisk(); }

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
