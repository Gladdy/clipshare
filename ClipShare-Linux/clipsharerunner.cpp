#include "clipsharerunner.h"
#include "clipboardcontent.h"
#include "tcpclient.h"

#include <QClipboard>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QFile>

#include <iostream>

ClipShareRunner::ClipShareRunner(QApplication *a, QObject * p) :
	QObject(p),
	app(a)
{
	tcpclient = new TcpClient(this);
	mimeData = new QMimeData();

	connect(app->clipboard(),SIGNAL(dataChanged()),this,SLOT(processClipboardChange()));

	connect(this,SIGNAL(writeToSocket(QString)),tcpclient,SLOT(writeToSocket(QString)));
	connect(tcpclient,SIGNAL(readFromSocket(QString)),this,SLOT(readFromSocket(QString)));
}
void ClipShareRunner::initialize() {
	processConfigFile();
	tcpclient->initConnection();
}

void ClipShareRunner::processClipboardChange()
{
	if(!updatingClipboard)
	{
        clipboardMutex.lock();
		ClipboardContent cc (QClipboard::Clipboard, app, &supportedTypes);
        QString clipboardJSON = cc.toJSONString();
        qDebug() << "processing clipboard change";
        clipboardMutex.unlock();

		emit writeToSocket(clipboardJSON);
	}
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

		if(configJSON.contains("username") && configJSON.contains("password") && configJSON.contains("hostname") && configJSON.contains("port"))
		{
			QString username = configJSON["username"].toString();
			QString password = configJSON["password"].toString();
			QString hostname = configJSON["hostname"].toString();
			int port = configJSON["port"].toInt();

			tcpclient->updateConnectInfo(username,password,hostname,port);
		}
		else
		{
			std::cerr << "invalid connection info in config file" << std::endl;
			exit(1);
		}
	}
}

/**
 * @brief ClipShareRunner::readFromSocket performs a clipboard operation thus must be locked
 * @param str
 */
void ClipShareRunner::readFromSocket(QString str)
{
    updatingClipboard = true;
    clipboardMutex.lock();

    mimeData->clear();

	QByteArray strData = str.toLatin1();
	QJsonDocument strJsonDoc = QJsonDocument::fromJson(strData);

	if(strJsonDoc.isNull()) {
		std::cerr << "received invalid json object" << std::endl;
		exit(1);
	}

	QJsonObject strJsonObject = strJsonDoc.object();

	for(QString type : supportedTypes)
	{
		if(strJsonObject.contains(type))
		{
			QByteArray data = strJsonObject[type].toString().toLatin1();
			mimeData->setData(type, data);
		}
	}

    qDebug() << "before setting mime data";
	app->clipboard()->setMimeData(mimeData);
    qDebug() << "after setting mime data";

    clipboardMutex.unlock();
    updatingClipboard = false;
}
