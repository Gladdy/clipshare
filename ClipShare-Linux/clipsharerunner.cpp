#include "clipsharerunner.h"
#include "clipboardcontent.h"
#include "tcpclient.h"

#include <QClipboard>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

#include <iostream>

ClipShareRunner::ClipShareRunner(QClipboard * qc, QObject * parent) :
QObject(parent),
clipboard(qc)
{
    tcpclient = new TcpClient("martijn-laptop", 1337, this);

    connect(clipboard,SIGNAL(dataChanged()),this,SLOT(processClipboardChange()));

    connect(this,SIGNAL(writeToSocket(QString)),tcpclient,SLOT(writeToSocket(QString)));
    connect(tcpclient,SIGNAL(readFromSocket(QString)),this,SLOT(readFromSocket(QString)));
}

void ClipShareRunner::processClipboardChange()
{
    ClipboardContent cc (QClipboard::Clipboard,clipboard);
    processClipboardContent(cc);
}
void ClipShareRunner::processClipboardContent(const ClipboardContent& cc)
{
    QJsonObject json;

    if(cc.hasText()) { json.insert("text", cc.getText()); }
    if(cc.hasHtml()) { json.insert("html", cc.getHtml()); }

    if(json.count() > 0) {

	QJsonDocument doc (json);
	QByteArray jsonData = doc.toJson();
	QString jsonString (jsonData);

	emit writeToSocket(jsonString);
    }
}

void ClipShareRunner::readFromSocket(const QString& str) {
    std::cout << "clipsharerunner: " << str.toStdString() << std::endl;
}
