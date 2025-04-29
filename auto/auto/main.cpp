#include "AutoMainWnd.h"
#include <QtWidgets/QApplication>
#include "Tools/rechelper.h"
#include <QTextCodec>

void ModifyDateRec()
{
	std::vector<DateModifyItem> items;
	tool::FileHelp::ReadDateRecConfig(PathHelper::Instance()->GetDateModifyIniFile(), items);
	RECHelper::Instance()->ModifyRecFileWithDate(items);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForLocale(codec);

    ModifyDateRec();

    AutoMainWnd w;
    w.show();
    return a.exec();
}

