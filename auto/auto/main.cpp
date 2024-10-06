#include "AutoMainWnd.h"
#include <QtWidgets/QApplication>
#include "Tools/rechelper.h"

void ModifyDateRec()
{
	std::vector<DateModifyItem> items;
	tool::FileHelp::ReadDateRecConfig(PathHelper::Instance()->GetDateModifyIniFile(), items);
	RECHelper::Instance()->ModifyRecFileWithDate(items);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ModifyDateRec();

    AutoMainWnd w;
    w.show();
    return a.exec();
}

