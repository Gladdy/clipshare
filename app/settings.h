#ifndef SETTINGS_H
#define SETTINGS_H

#include "status.h"

#include <QObject>
#include <QString>
#include <QtSql>

#include <map>

class Settings : public StatusReporter {
  Q_OBJECT
public:
  Settings(QObject *parent = 0);
  ~Settings();

  void load();

  QVariant getSetting(QString);
  void saveSetting(QString, QVariant);

signals:
  void triggerHide();

private:
  QSqlDatabase db;

  void saveToDisk(bool insert = false);
  void loadFromDisk();

  const char *dbfile = "settings.db";
  std::map<QString, QVariant> settings {
        { "email", ""},
        { "password", ""},
        { "hostname", "localhost"},
        { "port", 8000 },
        { "maxsize", 20000000 },
        { "maxsize_max", 50000000 },
        { "localstorage", true },
        { "notifications", true },
        { "triggerperiod", 500 }
  };
};

#endif // SETTINGS_H
