#ifndef CLIPBOARDCONTENT_H
#define CLIPBOARDCONTENT_H

#include <QClipboard>
#include <QString>
#include <QMimeData>

#include <iostream>

class ClipboardContent {
public:
	ClipboardContent() {}
	ClipboardContent(QClipboard::Mode m, QClipboard * clipboard) :
		mode(m)
	{
		const QMimeData * mimeData = clipboard->mimeData(mode);

		if(mimeData->hasHtml()) {
			hasHtmlBool = true;
			html = mimeData->html();
		}

		if(mimeData->hasText()) {
			hasTextBool = true;
			text = mimeData->text();
		}
	}
	~ClipboardContent() {}
	ClipboardContent(const ClipboardContent& other) {
		this->mode = other.mode;
		this->hasTextBool = other.hasTextBool;
		this->text = other.text;
		this->hasHtmlBool = other.hasHtmlBool;
		this->html = other.html;
	}

	bool hasText() const { return hasTextBool; }
	bool hasHtml() const { return hasHtmlBool; }
	QString getText() const { return hasTextBool ? text : ""; }
	QString getHtml() const { return hasHtmlBool ? html : ""; }

	friend std::ostream& operator<< (std::ostream &out, ClipboardContent &c);

	bool operator ==(const ClipboardContent& rhs);
	bool operator !=(const ClipboardContent& rhs);

private:
	QClipboard::Mode mode;

	bool hasTextBool = false;
	QString text;

	bool hasHtmlBool = false;
	QString html;
};

#endif // CLIPBOARDCONTENT_H
