#include "clipboardcontent.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QImage>

QString ClipboardContent::toJSONString() {

	const QMimeData * mimeData = app->clipboard()->mimeData();
	QJsonObject clipboardJSON;

	for(QString t : *supportedTypes) {
		if(mimeData->hasFormat(t)) {
			QByteArray data = mimeData->data(t);
			clipboardJSON.insert(t,QString(data));
		}
	}

	QJsonDocument doc (clipboardJSON);
	QByteArray clipboardJSONdata = doc.toJson();
	QString clipboardJSONString {clipboardJSONdata};

	return clipboardJSONString;
}

/*
if(mimeData->hasImage()) {
	QVariant v = mimeData->imageData();

	QImage image = v.value<QImage>();

	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer,"JPEG", -1);

	QString data {ba};
	clipboardJSON.insert("image/jpeg",data);

	std::cout << "image size: " << data.length() << std::endl;
}
*/
