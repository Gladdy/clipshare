#ifndef STATUSWINDOW_H
#define STATUSWINDOW_H

#include "status.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QLineEdit>
#include <QObject>

namespace Ui {
class StatusWindow;
}

class Settings;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(Settings * s, QObject *parent = 0);
    ~Window();

    /**
     * @brief fillFields
     * Pull the data from the application settings and attempt to fill as much fields as possible
     */
    void fillFields();

public slots:

    void processMessage(MessageType type, QString message);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason);

    void processRegister();
    void processLogin();

    void processOK();
    void processCancel();
    bool processApply();

    void showTrayMessage(QString message, QSystemTrayIcon::MessageIcon messageIcon = QSystemTrayIcon::Warning);

signals:

    void emitCommand(CommandType type, QString command);

private:
    Ui::StatusWindow *ui;

    void setLoginFields(bool enabled);
    void setTextStatus(QLineEdit*, bool correct);

    bool gotCriticalError = false;

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QString titleString;
    const QIcon icon;
    Settings * settings;

    const int progressResolution = 1024;
};

#endif // STATUSWINDOW_H
