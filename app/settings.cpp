#include "settings.h"

#include <QDebug>
#include <QApplication>

#include <iostream>

Settings::Settings(QObject * parent) : StatusReporter(parent) {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("settings.db");

  //qDebug() << db.drivers();
  //qDebug() << QApplication::libraryPaths();
}

Settings::~Settings() {
  db.close();
}

void Settings::load() {

  //Open the database
  if(!db.open()) {
    emitMessage(Error, db.lastError().text());
  }

  //Initialize the database if necessary
  if(db.tables().contains("settings") == false)
  {
    //Create the settings table
    QSqlQuery createSettings;
    createSettings.exec( "CREATE TABLE IF NOT EXISTS settings ("
                "key TEXT PRIMARY KEY, "
                "value TEXT"
                ");");

    //Populate the settings table
    for(auto& keyvalue : defaultSettings) {
      QSqlQuery insertDefault;
      insertDefault.prepare("INSERT INTO settings (key,value) VALUES (:key, :value);");
      insertDefault.bindValue(":key", keyvalue.first);
      insertDefault.bindValue(":value",keyvalue.second);
      insertDefault.exec();
    }
  }
}

QVariant Settings::getSetting(QString key) {

  QSqlQuery checkSetting;
  checkSetting.prepare("SELECT * FROM settings WHERE key=:key");
  checkSetting.bindValue(":key", key);
  checkSetting.exec();

  if(checkSetting.next())
  {
    QVariant key = checkSetting.value(0);
    QVariant value = checkSetting.value(1);
    qDebug() << key.toString() << '\t' << value.toString();

    return value;
  }
  else
  {
    emitMessage(Error, tr("Key does not exist in database") + " : " + key);
  }
  return QVariant();
}

void Settings::saveSetting(QString key, QVariant value) {
  QSqlQuery updateSetting;
  updateSetting.prepare("UPDATE settings SET value=:value WHERE key=:key;");
  updateSetting.bindValue(":key", key);
  updateSetting.bindValue(":value",value);
  updateSetting.exec();
}

