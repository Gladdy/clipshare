#ifndef CLIPBOARDCONTENT_H
#define CLIPBOARDCONTENT_H

#include <QClipboard>
#include <QApplication>
#include <QString>
#include <QMimeData>
#include <QStringList>


class ClipboardContent {
public:
	ClipboardContent() {}
	ClipboardContent(QClipboard::Mode m, QApplication * a, QStringList * st) :
		mode(m),
		app(a),
		supportedTypes(st)
	{
	}
	QString toJSONString();
	~ClipboardContent() {}

private:
	QClipboard::Mode mode;
	QApplication * app;
	QStringList* supportedTypes;
};


#endif // CLIPBOARDCONTENT_H
