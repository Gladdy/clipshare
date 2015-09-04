#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include "status.h"

#include "sqlite3.h"

#include <QObject>
#include <QString>
#include <QJsonValue>
#include <QJsonDocument>
#include <QMutex>

class Settings : public StatusReporter {
  Q_OBJECT
public:
  Settings(QObject *parent = 0);
  ~Settings() {}


  void load();
  void saveConfigToDisk();

  template<typename T>
  T getSetting(QString);

  template<typename T>
  void saveSetting(QString,T);

private:
  sqlite3 *db;



  /*
   * Legacy crap - should be removed when moved to SQLite
   */
public:
  QString getConnectString() {
    return QString(hostname + ":" + QString::number(port));
  }
  QString getHostname() { return hostname; }
  int getSizeLimit() { return sizeLimit; }
  QString getEmail() { return email; }
  bool setEmail(QString);
  QString getPassword() { return password; }
  bool setPassword(QString p);
  bool getTrayNotification() { return trayNotification; }
  bool setTrayNotification(bool t) {
    trayNotification = t;
    return true;
  }
  bool getLocalStorage() { return localStorage; }
  bool setLocalStorage(bool l) {
    localStorage = l;
    return true;
  }
  int getDoubleCopyPeriod() { return doubleCopyPeriod; }
  bool setDoubleCopyPeriod(QString d);

private:
  void loadDefaults();
  bool validateEmail(QString);
  int validateNumber(QString, int, int);

  QString hostname;
  int port;
  int sizeLimit;

  QString email;
  QString password;
  bool trayNotification;
  bool localStorage;
  int doubleCopyPeriod;

  const QString configFilename = "config.cfg";

  /*
   *  END
   */

};

#endif // APPLICATIONSETTINGS_H
