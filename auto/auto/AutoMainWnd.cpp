#include <QMenu>
#include <QAction>
#include <iosfwd>
#include <io.h>
#include <fstream>
#include "AutoMainWnd.h"


AutoMainWnd::AutoMainWnd(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    CItemInit::Instance()->AppInit();
    OnUiInit();
    InitTraySys();
}

AutoMainWnd::~AutoMainWnd()
{

}

void AutoMainWnd::on_btn_open_clicked()
{
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);

    TCHAR szBuffer[MAX_PATH] = { 0 };
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = (HWND)this->winId();
    ofn.lpstrFilter = _T("ini文件(*.ini)\0 * .ini\0");
    ofn.lpstrInitialDir = _T("D:\\Program Files");
    ofn.lpstrFile = szBuffer;
    ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
    ofn.nFilterIndex = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
    BOOL bSel = GetOpenFileName(&ofn);
    if (bSel) {
        m_strOpenFilePath = szBuffer;
    }
    else {
        m_strOpenFilePath = L"";
    }
    ui.edt_file_path->setText(QString::fromStdWString(m_strOpenFilePath));
    if (!m_strOpenFilePath.empty()) {
        std::wstring strTempModifyFile = m_strOpenFilePath.substr(0, m_strOpenFilePath.find(L".ini"));
        PathHelper::Instance()->SetCurrentModifyFile(strTempModifyFile);
        tool::FileHelp::ReadTempConfigFle(strTempModifyFile, CItemInit::Instance()->m_scTempModify);
        InitList();
    }
}

void AutoMainWnd::on_btn_save_clicked()
{

}


void AutoMainWnd::OnUiInit()
{

    //this->setFont(QFont("Microsoft YaHei", 14));
    this->setWindowIcon(QIcon(":/icon/logo"));

    QStringList headers;
    headers << QStringLiteral("") << QStringLiteral("序号")<< QStringLiteral("地名") << QStringLiteral("天气") /*<< QStringLiteral("天气2")*/ << QStringLiteral("最低温度") << QStringLiteral("最高温度") << QStringLiteral("风向1")  << QStringLiteral("风向2") << QStringLiteral("风向等级1") << QStringLiteral("风向等级2") << QStringLiteral("降水概率") ;
    ui.tableWidget->setColumnCount(headers.size());
    ui.tableWidget->setHorizontalHeaderLabels(headers);
    ui.tableWidget->setShowGrid(true);
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui.tableWidget->horizontalHeader()->setVisible(true);//是否显示水平表头
    ui.tableWidget->verticalHeader()->setVisible(false);//是否显示垂直表头
    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui.tableWidget->setColumnWidth(0, 40);
    ui.tableWidget->setColumnWidth(1, 80);
    ui.tableWidget->setColumnWidth(2, 120);
    ui.tableWidget->setColumnWidth(3, 150);// 天气
    ui.tableWidget->setColumnWidth(4, 80);
    ui.tableWidget->setColumnWidth(5, 80); 
    ui.tableWidget->setColumnWidth(6, 80);
    ui.tableWidget->setColumnWidth(7, 80);
    ui.tableWidget->setColumnWidth(8, 80);
    ui.tableWidget->setColumnWidth(9, 80);
}

/*
    创建系统托盘图标
*/
void AutoMainWnd::InitTraySys()
{
    m_pTrayIcon = new QSystemTrayIcon(QIcon(":/icon/logo"), this);
    QMenu* menu = new QMenu(this);
    QAction* openAction = menu->addAction(QStringLiteral("打开主界面"));
    QAction* closeAction = menu->addAction(QStringLiteral("关闭程序"));
    m_pTrayIcon->setContextMenu(menu);
    QObject::connect(openAction, &QAction::triggered, this, [&]() {this->showNormal(); });
    QObject::connect(closeAction, &QAction::triggered, this, [&]() {this->close(); });
    m_pTrayIcon->show();
}

void AutoMainWnd::AutoOpenIniFile()
{
    auto inifile = PathHelper::Instance()->GetCurrentDir() + L"auto2.0.ini";
    if (_waccess(inifile.c_str(), 0) != 0) {
        return;
    }

    std::wstring bufLine;
    std::wifstream ifs;
    ifs.open(inifile.c_str(), std::ios::in);

    if (ifs.is_open()) {
        while (getline(ifs, bufLine)) {
            break;
        }
        ifs.close();
    }

    if (_waccess(bufLine.c_str(), 0) != 0) {
        return;
    }

    m_strOpenFilePath = bufLine;

    ui.edt_file_path->setText(QString::fromStdWString(m_strOpenFilePath));

    if (!m_strOpenFilePath.empty()) {

        std::wstring strTempModifyFile = m_strOpenFilePath.substr(0, m_strOpenFilePath.find(L".ini"));
        PathHelper::Instance()->SetCurrentModifyFile(strTempModifyFile);
        tool::FileHelp::ReadTempConfigFle(strTempModifyFile, CItemInit::Instance()->m_scTempModify);
        InitList();
    }
}


void AutoMainWnd::InitList()
{
    if (m_bListInited) {
        InitContentList();
        return;
    }
    m_bListInited = true;
    InitContentList();
}

void AutoMainWnd::InitContentList()
{
    m_vCtxTextList.clear();
    m_vCtxIndexList.clear();
    if (!tool::FileHelp::ReadConfigIniFile(m_strOpenFilePath, m_vCtxIndexList)) {
        MessageBox(NULL, L"读取文件失败,请检查文件格式", L"错误提示", MB_OK);
        return;
    }
    ChangeIndexVecToTextVec(m_vCtxIndexList);
    ShowContentList();
    QTableWidgetItem* selectedItem = ui.tableWidget->item(0, 1);
    ui.tableWidget->setCurrentItem(selectedItem);
    m_curIndex = 0;
    m_CopyedItem = m_vCtxTextList[0];
}

// index转换为text
void AutoMainWnd::ChangeIndexVecToTextVec(std::vector<ContentListItem>& vec)
{
    // 只需要调整 天气1,2，温度1,2，风向1,2 风向等级1,2 降水率
    m_vCtxTextList = vec;

    auto findfunc = [](std::vector<std::wstring>vect, std::wstring strIndex)->std::wstring {
        try {
            int index = _wtoi(strIndex.c_str());
            if (index<0 || index > vect.size()) return L"";
            return vect[index];
        }
        catch (...) {
            return L"";
        }
    };

    std::wstring strText;
    for (int i = 0; i < vec.size(); i++) {
        strText = findfunc(CItemInit::Instance()->g_scWeatherName, vec[i].strWeather);
        m_vCtxTextList[i].strWeather = strText;
        strText = findfunc(CItemInit::Instance()->g_scWeatherName, vec[i].strWeatherEx);
        m_vCtxTextList[i].strWeatherEx = strText;

        strText = findfunc(CItemInit::Instance()->g_scTempName, vec[i].strTemp);
        m_vCtxTextList[i].strTemp = strText;
        strText = findfunc(CItemInit::Instance()->g_scTempName, vec[i].strTempEx);
        m_vCtxTextList[i].strTempEx = strText;

        strText = findfunc(CItemInit::Instance()->g_scWindName, vec[i].strWind);
        m_vCtxTextList[i].strWind = strText;
        strText = findfunc(CItemInit::Instance()->g_scWindName, vec[i].strWindEx);
        m_vCtxTextList[i].strWindEx = strText;

        strText = findfunc(CItemInit::Instance()->g_scWindLv, vec[i].strWindLv);
        m_vCtxTextList[i].strWindLv = strText;
        strText = findfunc(CItemInit::Instance()->g_scWindLv, vec[i].strWindLvEx);
        m_vCtxTextList[i].strWindLvEx = strText;

        strText = findfunc(CItemInit::Instance()->g_scPrecipitationName, vec[i].strPrecipitation);
        m_vCtxTextList[i].strPrecipitation = strText;
    }
}

// text 转 index
void AutoMainWnd::ChangeTextVecToIndexVec()
{
    m_vCtxIndexList = m_vCtxTextList;

    auto findfuncex = [](std::vector<std::wstring>vect, std::wstring strText)->std::wstring {
        wchar_t sz[12] = { 0 };
        for (int i = 0; i < vect.size(); i++) {
            if (vect[i] == strText) {
                wsprintf(sz, L"%03d", i);
                return std::wstring(sz);
            }
        }
        return L"";
    };

    std::wstring strIndex;
    for (int i = 0; i < m_vCtxTextList.size(); i++)
    {
        strIndex = findfuncex(CItemInit::Instance()->g_scWeatherName, m_vCtxTextList[i].strWeather);
        m_vCtxIndexList[i].strWeather = strIndex;
        strIndex = findfuncex(CItemInit::Instance()->g_scWeatherName, m_vCtxTextList[i].strWeatherEx);
        m_vCtxIndexList[i].strWeatherEx = strIndex;

        strIndex = findfuncex(CItemInit::Instance()->g_scTempName, m_vCtxTextList[i].strTemp);
        m_vCtxIndexList[i].strTemp = strIndex;
        strIndex = findfuncex(CItemInit::Instance()->g_scTempName, m_vCtxTextList[i].strTempEx);
        m_vCtxIndexList[i].strTempEx = strIndex;

        strIndex = findfuncex(CItemInit::Instance()->g_scWindName, m_vCtxTextList[i].strWind);
        m_vCtxIndexList[i].strWind = strIndex;
        strIndex = findfuncex(CItemInit::Instance()->g_scWindName, m_vCtxTextList[i].strWindEx);
        m_vCtxIndexList[i].strWindEx = strIndex;

        strIndex = findfuncex(CItemInit::Instance()->g_scWindLv, m_vCtxTextList[i].strWindLv);
        m_vCtxIndexList[i].strWindLv = strIndex;
        strIndex = findfuncex(CItemInit::Instance()->g_scWindLv, m_vCtxTextList[i].strWindLvEx);
        m_vCtxIndexList[i].strWindLvEx = strIndex;

        strIndex = findfuncex(CItemInit::Instance()->g_scPrecipitationName, m_vCtxIndexList[i].strPrecipitation);
        m_vCtxIndexList[i].strPrecipitation = strIndex;
    }
}

/*
    显示变动槽函数
*/
void AutoMainWnd::slot_text_select(QString strText, int nIndex,  ModifyType type)
{
    if (m_vCtxTextList.size() == 0) return;
    int nCtxIndex = ui.tableWidget->currentRow();
    if (nCtxIndex < 0) {
        return;
    }

    auto funcfinditem = [=](std::vector<std::wstring> vec, int index) ->std::wstring {
        if (index < 0 || index > vec.size()) return L"";
        return vec[index];
    };

    std::wstring strItemText = strText.toStdWString();

    switch (type)
    {
    case ModifyType::type_weather:
    {
        m_vCtxTextList[nCtxIndex].strWeather = strItemText;
        if (nIndex > 19)
            m_vCtxTextList[nCtxIndex].strWeatherEx = strItemText;
    }
        break;
    case ModifyType::type_weatherex:
    {
        m_vCtxTextList[nCtxIndex].strWeatherEx = strItemText;
        if (nIndex > 19)
            m_vCtxTextList[nCtxIndex].strWeather = strItemText;
    }
        break;
    case ModifyType::type_temp:
    {
        m_vCtxTextList[nCtxIndex].strTemp = strItemText;
    }
        break;
    case ModifyType::type_tempex:
    {
        m_vCtxTextList[nCtxIndex].strTempEx = strItemText;
    }
        break;
    case ModifyType::type_wind:
    {
        m_vCtxTextList[nCtxIndex].strWind = funcfinditem(CItemInit::Instance()->g_scWindName, nIndex);
    }
        break;
    case ModifyType::type_windex:
    {
        m_vCtxTextList[nCtxIndex].strWindEx = funcfinditem(CItemInit::Instance()->g_scWindName, nIndex);
    }
        break;
    case ModifyType::type_windlv:
    {
        m_vCtxTextList[nCtxIndex].strWindLv = funcfinditem(CItemInit::Instance()->g_scWindLv, nIndex);
    }
        break;
    case ModifyType::type_windlvex:
    {
        m_vCtxTextList[nCtxIndex].strWindLvEx = funcfinditem(CItemInit::Instance()->g_scWindLv, nIndex);
    }
        break;
    case ModifyType::type_precipitation:
    {
        m_vCtxTextList[nCtxIndex].strPrecipitation = strItemText;
    }
        break;
    default:
        break;
    }
    ChangeTextVecToIndexVec();
    //ShowContentList();
}

CLineComboxComplete* AutoMainWnd::BuilderItem(ModifyType type)
{
    CLineComboxComplete* pItem = new CLineComboxComplete();
    connect(pItem, &CLineComboxComplete::signal_select, this, &AutoMainWnd::slot_text_select);

    QStringList slistweather;
    /*
    for (auto& item : CItemInit::Instance()->g_scWeatherName){
        slistweather << QString::fromStdWString(item);
    }*/

    // 把前20个过滤掉。因为weathermap中包含了。
    for (int i=20;i< CItemInit::Instance()->g_scWeatherName.size();i++)
    {
        slistweather << QString::fromStdWString(CItemInit::Instance()->g_scWeatherName[i]);
    }

    for (auto & item : CItemInit::Instance()->g_scWeatherMap)
    {
        slistweather << QString::fromStdWString(item.first);
    }
    QStringList slisttemp;
    for (auto& item : CItemInit::Instance()->g_scTempName) {
        slisttemp << QString::fromStdWString(item);
    }

    QStringList slistwindname;
    for (auto& item : CItemInit::Instance()->g_scWindName) {
        slistwindname << QString::fromStdWString(item);
    }

    QStringList slistwindlv;
    for (auto& item : CItemInit::Instance()->g_scWindLv) {
        slistwindlv << QString::fromStdWString(item);
    }

    QStringList slistPre;
    for (auto& item : CItemInit::Instance()->g_scPrecipitationName) {
        slistPre << QString::fromStdWString(item);
    }

    switch (type)
    {
    case ModifyType::type_weather:
    {
        pItem->SetModifyType(ModifyType::type_weather);
        //pItem->SetPlaceholderText(QStringLiteral("天气"));
        pItem->SetWordList(slistweather);
    }
        break;
    case ModifyType::type_weatherex:
    {
        pItem->SetModifyType(ModifyType::type_weatherex);
        pItem->SetWordList(slistweather);
    }
        break;
    case ModifyType::type_temp:
    {
        pItem->SetModifyType(ModifyType::type_temp);
        pItem->SetWordList(slisttemp);
    }
        break;
    case ModifyType::type_tempex:
    {
       pItem->SetModifyType(ModifyType::type_tempex);
       pItem->SetWordList(slisttemp);
    }
        break;
    case ModifyType::type_wind:
    {
        pItem->SetModifyType(ModifyType::type_wind);
        pItem->SetWordList(slistwindname);
    }
        break;
    case ModifyType::type_windex:
    {
        pItem->SetModifyType(ModifyType::type_windex);
        pItem->SetWordList(slistwindname);
    }
        break;
    case ModifyType::type_windlv:
    {
        pItem->SetModifyType(ModifyType::type_windlv);
        pItem->SetWordList(slistwindlv);
    }
        break;
    case ModifyType::type_windlvex:
    {
       pItem->SetModifyType(ModifyType::type_windlvex);
       pItem->SetWordList(slistwindlv);
    }
        break;
    case ModifyType::type_precipitation:
    {
        pItem->SetModifyType(ModifyType::type_precipitation);
        pItem->SetWordList(slistPre);
    }
        break;
    default:
        break;
    }

   
    return pItem;
}

void AutoMainWnd::ShowContentList()
{
    CLineComboxComplete* pItem = nullptr;
    ui.tableWidget->clearContents();
    ui.tableWidget->setRowCount(m_vCtxTextList.size());


    auto compfunc = [](std::wstring str1, std::wstring str2, std::wstring strFlag)->std::wstring {
        std::wstring strText;
        if (str1.empty() && str2.empty()) {
            strText = L"空";
        }
        else if (str1.empty() || str2.empty()) {
            strText = str1.empty() ? str2 : str1;
        }
        else if (str1 == str2) {
            strText = str1;
        }
        else {
            strText = str1 + strFlag + str2;
        }
        return strText;
    };


    QStringList slisttmp;
    std::wstring strTmp, strTmpEx, strText;
    int nw1 = 0, nw2 = 0;
    QCheckBox* pcb = nullptr;
    wchar_t buff[8] = { 0 };
    for (int i = 0; i < m_vCtxTextList.size(); i++)
    {
        pcb = new QCheckBox(this);
        pcb->setMinimumSize(40, 40);
        pcb->setStyleSheet("QCheckBox{border: none; }QCheckBox::indicator{width: 40px;height: 40px;}QCheckBox::indicator:unchecked{image: url(:/icon/icon_uncheck);}QCheckBox::indicator:checked{image: url(:/icon/icon_check);}");

        ui.tableWidget->setCellWidget(i, 0, pcb);

        ZeroMemory(buff, sizeof(buff));
        wsprintf(buff, L"%03d", i);
        ui.tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdWString(buff)));
        ui.tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdWString(m_vCtxTextList[i].strLocation)));

        strTmp = m_vCtxTextList[i].strWeather;
        strTmpEx = m_vCtxTextList[i].strWeatherEx;
        nw1 = _wtoi(m_vCtxIndexList[i].strWeather.c_str());
        nw2 = _wtoi(m_vCtxIndexList[i].strWeatherEx.c_str());
        if ((nw1 > 19 && nw2 < 19) || (nw1 < 19 && nw2 > 19)) {
            //strText = compfunc(strTmp, strTmpEx, L"X");
            strText = strTmp;
        }
        else if ((nw1 == 19 || nw2 == 19) && (nw1 != nw2)) {
            strText = compfunc(strTmp, strTmpEx, L"X");
        }
        else if (nw1 == 19 && nw2 == 19) {
            strText = L"空";
        }
        else if (nw1 > 19 && nw2 > 19) {
            strText = strTmp;
        }
        else {
            strText = compfunc(strTmp, strTmpEx, L"转");
        }
        //ui.tableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdWString(strText)));

        pItem = BuilderItem(ModifyType::type_weather);
        pItem->SetTextContent(strText);
        ui.tableWidget->setCellWidget(i, 3, pItem);
        /*
        pItem = BuilderItem(ModifyType::type_weatherex);
        pItem->SetTextContent(m_vCtxTextList[i].strWeatherEx);
        ui.tableWidget->setCellWidget(i, 4, pItem);
        */

        pItem = BuilderItem(ModifyType::type_temp);
        pItem->SetTextContent(m_vCtxTextList[i].strTemp);
        ui.tableWidget->setCellWidget(i, 4, pItem);

        pItem = BuilderItem(ModifyType::type_tempex);
        pItem->SetTextContent(m_vCtxTextList[i].strTempEx);
        ui.tableWidget->setCellWidget(i, 5, pItem);
        
        pItem = BuilderItem(ModifyType::type_wind);
        pItem->SetTextContent(m_vCtxTextList[i].strWind);
        ui.tableWidget->setCellWidget(i, 6, pItem);

        pItem = BuilderItem(ModifyType::type_windex);
        pItem->SetTextContent(m_vCtxTextList[i].strWindEx);
        ui.tableWidget->setCellWidget(i, 7, pItem);
        
        pItem = BuilderItem(ModifyType::type_windlv);
        pItem->SetTextContent(m_vCtxTextList[i].strWindLv);
        ui.tableWidget->setCellWidget(i, 8, pItem);

        pItem = BuilderItem(ModifyType::type_windlvex);
        pItem->SetTextContent(m_vCtxTextList[i].strWindLvEx);
        ui.tableWidget->setCellWidget(i, 9, pItem);
        
        pItem = BuilderItem(ModifyType::type_precipitation);
        pItem->SetTextContent(m_vCtxTextList[i].strPrecipitation);
        ui.tableWidget->setCellWidget(i, 10, pItem);

        ui.tableWidget->setRowHeight(i, 40);
    }
}