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

private:
  QSqlDatabase db;

  const char *dbfile = "settings.db";
  const std::map<QString, QVariant> defaultSettings {
        { "email", ""},
        { "password", ""},
        { "hostname", "prostendo.com"},
        { "maxsize", 20000000 },
        { "maxsize_max", 50000000 },
        { "localstorage", true },
        { "notifications", true },
        { "triggerperiod", 500 }
  };
};

#endif // SETTINGS_H
