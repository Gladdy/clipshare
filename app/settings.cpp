#include "settings.h"

#include <QDebug>
#include <QApplication>

#include <iostream>

Settings::Settings(QObject * parent) : StatusReporter(parent) {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("settings.db");
}

Settings::~Settings() {
  saveToDisk();
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

    //Populate the settings table with initial values
    saveToDisk(true);
  }
  else
  {
    //Load the values from the database into the map
    loadFromDisk();
  }
}

QVariant Settings::getSetting(QString key) {

  auto it = settings.find(key);

  if(it != settings.end())
  {
    return it->second;
  }
  else
  {
    emitMessage(Error, tr("Key does not exist in database") + " : " + key);
  }
  return QVariant();
}

void Settings::saveSetting(QString key, QVariant value) {

  qDebug() << "saved " << key << " as " << value;
  settings[key] = value;

  //No need to save all data on every update
  //saveToDisk();
}

void Settings::saveToDisk(bool insert) {
  qDebug() << "SAVING";
  for(auto& keyvalue : settings) {

    QSqlQuery saveQuery;

    if(insert) {
      saveQuery.prepare("INSERT INTO settings (key,value) VALUES (:key, :value);");
    } else {
      saveQuery.prepare("UPDATE settings SET value=:value WHERE key=:key;");
    }

    saveQuery.bindValue(":key", keyvalue.first);
    saveQuery.bindValue(":value",keyvalue.second);
    saveQuery.exec();
  }
}

void Settings::loadFromDisk() {
  QSqlQuery loadValues;
  loadValues.exec("SELECT key,value FROM settings;");

  while(loadValues.next()) {
    QString key = loadValues.value(0).toString();
    QVariant value = loadValues.value(1);
    settings[key] = value;
  }
}

