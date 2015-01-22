#ifndef CLIPBOARDCHECKER_H
#define CLIPBOARDCHECKER_H

#include "clipboardcontent.h"

#include <QClipboard>
#include <QThread>

class ClipBoardChecker : public QThread
{
	Q_OBJECT

public:
	ClipBoardChecker(QClipboard * c, QObject * parent = 0) :
		QThread(parent),
		clipboard(c) {

	}

	~ClipBoardChecker();

signals:
	void sendClipboardChange(ClipboardContent);

protected:
	void run()
	{
		std::cout << "started run thread" << std::endl;
		std::cout << clipboard << std::endl;

		ClipboardContent cc_prev (QClipboard::Clipboard, clipboard);

		while(true)
		{
			std::cout << "loopin'" << std::endl;

			ClipboardContent cc (QClipboard::Clipboard, clipboard);

			if(cc != cc_prev)
			{
				std::cout << "attempting to emit" << std::endl;
				emit sendClipboardChange(cc);
				cc_prev = cc;
			}

			this->msleep(clipboardCheckPeriod);
		}
	}

private:
	QClipboard * clipboard;
	int clipboardCheckPeriod = 1000;
};

#endif // CLIPBOARDCHECKER_H
