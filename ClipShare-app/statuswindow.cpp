#include "statuswindow.h"
#include "ui_statuswindow.h"

#include <QDebug>

StatusWindow::StatusWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StatusWindow)
{
    ui->setupUi(this);

    createActions();
    createTrayIcon();

    trayIcon->show();
}

StatusWindow::~StatusWindow()
{
    delete ui;
}

void StatusWindow::createActions()
{
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void StatusWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
void StatusWindow::processNotification(QString str, QString msg) {
    //qDebug() << "got a notification: " << str << "\t" << msg;
    showMessage(str,msg);
}

void StatusWindow::showMessage(QString str, QString msg)
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon();
    //QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(typeComboBox->itemData(typeComboBox->currentIndex()).toInt());
    trayIcon->showMessage(str, msg, icon, 2000);
}
