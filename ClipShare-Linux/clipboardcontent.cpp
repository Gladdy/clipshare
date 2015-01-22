#include "clipboardcontent.h"

std::ostream& operator<< (std::ostream &out, ClipboardContent &c) {
	if(c.hasTextBool && c.text.length()) {
		QString t = c.text;
		t.replace(QString("\n"),QString(""));
		t.truncate(100);
		out << "text: " << t.toStdString() << std::endl;
	}

	if(c.hasHtmlBool && c.html.length()) {
		QString h = c.html;
		h.replace(QString("\n"),QString(""));
		h.truncate(100);
		out << "html: " << h.toStdString() << std::endl;
	}

	return out;
}

bool ClipboardContent::operator ==(const ClipboardContent& rhs) {
	if(rhs.hasTextBool != this->hasTextBool) { return false; }
	if(rhs.hasHtmlBool != this->hasHtmlBool) { return false; }
	if(rhs.text != this->text) { return false; }
	if(rhs.html != this->html) { return false; }

	return true;
}
bool ClipboardContent::operator !=(const ClipboardContent& rhs) {
	return !(*this == rhs);
}
