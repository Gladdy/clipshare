#ifndef STATUSWINDOW_H
#define STATUSWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QLineEdit>
#include <QObject>

namespace Ui {
class StatusWindow;
}

class ApplicationSettings;

class StatusWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatusWindow(ApplicationSettings * s, QWidget *parent = 0);
    ~StatusWindow();

public slots:
    void processNotification(QString, QString);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason);

    void processRegister();
    void processCheck();

    void processOK();
    void processCancel();
    bool processApply();

    void showTrayMessage(QString msg, QSystemTrayIcon::MessageIcon messageIcon = QSystemTrayIcon::Warning);

signals:
    void emitCommand(QString, QString);

private:
    Ui::StatusWindow *ui;

    void setupTrayMenu();
    void fillFields();
    void setLoginEnabled(bool);

    void setError(QLineEdit*);
    void setCorrect(QLineEdit*);

    bool applyAccount();
    bool applyGeneral();

    QAction *minimizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QString titleString;
    const QIcon icon;
    ApplicationSettings * settings;

    const int progressResolution = 1024;
};

#endif // STATUSWINDOW_H
