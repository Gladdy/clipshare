#include "clipsharerunner.h"
#include "clipboardcontent.h"
#include "tcpclient.h"

#include <QClipboard>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>

#include <iostream>


ClipShareRunner::ClipShareRunner(QClipboard * qc, QObject * parent) :
	QObject(parent),
	clipboard(qc)
{
	tcpclient = new TcpClient("192.168.2.68", 1337, this);

	connect(clipboard,SIGNAL(dataChanged()),this,SLOT(processClipboardChange()));

	connect(this,SIGNAL(writeToSocket(QString)),tcpclient,SLOT(writeToSocket(QString)));
	connect(tcpclient,SIGNAL(readFromSocket(QString)),this,SLOT(readFromSocket(QString)));

	processConfigFile();

	tcpclient->initConnection();
}
ClipShareRunner::~ClipShareRunner() {
	delete tcpclient;
}

void ClipShareRunner::processClipboardChange()
{
	ClipboardContent cc (QClipboard::Clipboard,clipboard);

	std::cout << cc << std::endl;

	processClipboardContent(cc);
}

void ClipShareRunner::processConfigFile()
{
	QFile configFile (configFilename);

	if(!configFile.open(QIODevice::ReadOnly)) {
		std::cerr << "Could not open config file: " << configFilename.toStdString() << std::endl;
		exit(1);
	}
	else
	{
		QByteArray configFileContent = configFile.readAll();
		configFile.close();

		QJsonDocument configJSONDoc = QJsonDocument::fromJson(configFileContent);

		if(configJSONDoc.isNull()) {
			std::cerr << "invalid config file" << std::endl;

			exit(1);
		}

		QJsonObject configJSON = configJSONDoc.object();

		if(configJSON.contains("username") && configJSON.contains("password"))
		{
			QString username = configJSON["username"].toString();
			QString password = configJSON["password"].toString();
			tcpclient->updateConnectString(username,password);
		}
	}
}

void ClipShareRunner::processClipboardContent(const ClipboardContent& cc)
{
	QJsonObject json;

	if(cc.hasText()) { json.insert("text", cc.getText()); }
	if(cc.hasHtml()) { json.insert("html", cc.getHtml()); }

	if(json.count() > 0)
	{
		QJsonDocument doc (json);
		QByteArray jsonData = doc.toJson();
		QString jsonString (jsonData);

		emit writeToSocket(jsonString);
	}
}

void ClipShareRunner::readFromSocket(const QString& str) {
	std::cout << "clipsharerunner: " << str.toStdString() << std::endl;
}
