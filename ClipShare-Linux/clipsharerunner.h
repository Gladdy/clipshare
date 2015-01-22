#ifndef CLIPSHARERUNNER_H
#define CLIPSHARERUNNER_H

#include "clipboardchecker.h"
#include "tcpclient.h"

#include <QObject>
#include <QApplication>
#include <QClipboard>

class ClipboardContent;

class ClipShareRunner : public QObject
{
	Q_OBJECT
public:
	ClipShareRunner(QClipboard * app, QObject *parent = 0);
	~ClipShareRunner() {}

signals:
	void writeToSocket(const QString&);

public slots:
	void processClipboardChange();
	void readFromSocket(const QString&);

private:
	TcpClient * tcpclient;
	QClipboard * clipboard;

	void processClipboardContent(const ClipboardContent&);
};

#endif // CLIPSHARERUNNER_H
