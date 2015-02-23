#ifndef STATUSWINDOW_H
#define STATUSWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui {
class StatusWindow;
}

class StatusWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatusWindow(QWidget *parent = 0);
    ~StatusWindow();

public slots:
    void processNotification(QString, QString);

signals:
    void emitCommand(QString, QString);

private:
    Ui::StatusWindow *ui;

    void createActions();
    void createTrayIcon();
    void showMessage(QString, QString);

    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // STATUSWINDOW_H
