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

public slots:
    /** The main slot for the status window
     *  @param type     The type of notification, choice from Error, Progress, Login, Notification
     *  @param message  Type-dependent information
     */
    void processMessage(MessageType type, QString message);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason);

    /**
     * @brief processRegister
     * Open the web page where the can register themselves (not included in the application)
     */
    void processRegister();

    /**
     * @brief processCheck
     * Apply the account page and emit a network request for checking of the credentials.
     * The response will return via processNotification(QString,QString)
     */
    void processCheck();

    /**
     * @brief processOK
     * Apply the settings using processApply and if successfull hide the screen
     */
    void processOK();

    /**
     * @brief processCancel
     * Hide the form and restore the previous settings, still stored in the ApplicationSettings
     */
    void processCancel();

    /**
     * @brief processApply
     * Check all the pages for possible errors using applyAccount() and applyGeneral() and if all is right
     * store the information in the application settings to disk for next time.
     */
    bool processApply();

    /**
     * @brief showTrayMessage
     * @param msg           The string to show as system tray pop up message
     * @param messageIcon   QSystemTrayIcon::[NoIcon Information Warning Critical
     */
    void showTrayMessage(QString message, QSystemTrayIcon::MessageIcon messageIcon = QSystemTrayIcon::Warning);

signals:
    /**
     * @brief emitCommand
     * Processed by ClipShareRunner::processCommand(QString,QString)
     * Only supported type is "Connect"
     */
    void emitCommand(CommandType type, QString command);

private:
    Ui::StatusWindow *ui;

    /**
     * @brief setupTrayMenu
     */
    void setupTrayMenu();

    /**
     * @brief fillFields
     * Pull the data from the application settings and attempt to fill as much fields as possible
     */
    void fillFields();

    /**
     * @brief applyAccount
     * @brief applyGeneral
     * Check whether the entered information could be valid, ie. filled and in the proper form
     */
    bool applyAccount();
    bool applyGeneral();

    /**
     * Auxiliary methods for changing form fields
     */
    void setLoginFields(bool enabled);
    void setTextStatus(QLineEdit*, bool correct);


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
