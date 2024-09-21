#include <QMenu>
#include <QAction>
#include <iosfwd>
#include <io.h>
#include <fstream>
#include "AutoMainWnd.h"
#include "Tools/wavhelper.h"
#include "Tools/rechelper.h"

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

/*
    打开文件
*/
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

/*
    保存并转换
*/
void AutoMainWnd::on_btn_save_clicked()
{
	if (m_vCtxTextList.size() == 0) return;
	std::wstring strFileName =ui.edt_file_path->text().toStdWString();
	tool::FileHelp::WriteConfigIniFile(strFileName, m_vCtxIndexList);
#ifdef _DEBUG
	WavHelper::Instance()->BuildWavWithOnceCall(m_vCtxIndexList);
	//WavHelper::Instance()->BuildAndCatWav(m_vCtxIndexList[m_pCtList->GetCurSel()]);
	//RECHelper::Instance()->ModifyRecFileEx(m_vCtxIndexList[m_pCtList->GetCurSel()]);
#else
	//WavHelper::Instance()->BuildAndCatWavAll(m_vCtxIndexList);
	WavHelper::Instance()->BuildWavWithOnceCall(m_vCtxIndexList);
	RECHelper::Instance()->ModifyRecFileAll(m_vCtxIndexList);
#endif // _DEBUG

	MessageBox(0, L"    结束! <<北京中天华视气象科技有限公司>>\n\n                        010-62146148", L"ok", MB_OK | MB_ICONINFORMATION);
}

/*
    一键粘贴
*/
void AutoMainWnd::on_btn_copy_clicked()
{
    if (ui.tableWidget->currentRow() < 0) return;

    auto FuncGetRowCheckedStat = [&](int row) {
        if (row < 0 || row > ui.tableWidget->rowCount()) return false;
        QCheckBox* pCheckBox = static_cast<QCheckBox*>(ui.tableWidget->cellWidget(row, 0));
        if (pCheckBox) {
            return pCheckBox->isChecked() ? true : false;
        }
        return false;
    };

    for (int i=0;i<m_vCtxTextList.size();i++)
    {
        // 勾选的才进行拷贝
        if(FuncGetRowCheckedStat(i) == false)
            continue;
        m_vCtxTextList[i].strWeather = m_CopyedItem.strWeather;
        m_vCtxTextList[i].strTemp = m_CopyedItem.strTemp;
        m_vCtxTextList[i].strTempEx = m_CopyedItem.strTempEx;
        m_vCtxTextList[i].strWind = m_CopyedItem.strWind;
        m_vCtxTextList[i].strWindEx = m_CopyedItem.strWindEx;
        m_vCtxTextList[i].strWindLv = m_CopyedItem.strWindLv;
        m_vCtxTextList[i].strWindLvEx = m_CopyedItem.strWindLvEx;
        m_vCtxTextList[i].strPrecipitation = m_CopyedItem.strPrecipitation;
    }

	// 复制的时候根据配置修改
	ModifyTempWithConfigEx(m_vCtxTextList, CItemInit::Instance()->m_scTempModify);

	ShowContentList();
	ChangeTextVecToIndexVec();
}


void AutoMainWnd::ModifyTempWithConfigEx(std::vector<ContentListItem>& vec, std::vector<TempItem>& vectemp)
{
	int nSize = vectemp.size();
	for (int i = 0; i < nSize; i++) {
		for (int j = 0; j < vec.size(); j++) {
			if (j == _wtoi(vectemp[i].strIndex.c_str()))
				ModifyTempWithConfig(vec[j], vectemp[i]);
		}
	}
}

void AutoMainWnd::ModifyTempWithConfig(ContentListItem& item, TempItem& tempItem)
{
	std::wstring str;
	int n1 = 0, n2 = 0, t1 = 0, t2 = 0;
	auto pfunc_num = [](std::wstring str) -> std::wstring {
		int nIndex = str.find(L"度");
		if (nIndex != -1) {
			return str.substr(0, nIndex);
		}
		else {
			return L"0";
		}
	};

	str = pfunc_num(item.strTemp);
	n1 = _wtoi(str.c_str());
	str = pfunc_num(item.strTempEx);
	n2 = _wtoi(str.c_str());

	t1 = _wtoi(tempItem.strTemp1.c_str());
	t2 = _wtoi(tempItem.strTemp2.c_str());

	item.strTemp = std::to_wstring(n1 + t1) + L"度";
	item.strTempEx = std::to_wstring(n2 + t2) + L"度";
}

/*
    读预报文件
*/
void AutoMainWnd::on_btn_read_clicked()
{

#ifndef _DEBUG
	if (m_vCtxTextList.size() == 0) {
		MessageBox(NULL, L"无修改内容可用!", L"提示", MB_ICONERROR | MB_OK);
		return;
	}
#endif // !_DEBUG

	std::wstring strOpenFilePath;
	TCHAR szBuffer[MAX_PATH] = { 0 };
	OPENFILENAME ofn = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = (HWND)(this->winId());
	ofn.lpstrFilter = _T("txt文件(*.txt)\0 * .txt\0");
	ofn.lpstrInitialDir = _T("D:\\Program Files");
	ofn.lpstrFile = szBuffer;
	ofn.nMaxFile = sizeof(szBuffer) / sizeof(*szBuffer);
	ofn.nFilterIndex = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
	BOOL bSel = GetOpenFileName(&ofn);
	if (bSel) {
		strOpenFilePath = szBuffer;
	}
	else {
		return;
	}

	FILE* fp = _wfopen(strOpenFilePath.c_str(), L"r");
	if (fp == nullptr) {
		MessageBox(NULL, L"打开文件失败!", L"提示", MB_ICONERROR | MB_OK);
		return;
	}
	char buffer[500] = { 0 };
	wstring strTextContent;
	while (fgets(buffer, 500, fp) != NULL) {
		strTextContent = tool::CodeHelper::Utf8ToUnicode(std::string(buffer));
		OutputDebugString((strTextContent + L"\r\n").c_str());
		if (strTextContent.find(L" ")) {
			HandleReportFile(strTextContent);
		}
		memset(buffer, 0, 500);
	}

	fclose(fp);
	MessageBox(NULL, L"批量修改完成!", L"提示", MB_OK);
}


// L"安平乡    晴天    15  16      北风12级"
bool AutoMainWnd::HandleReportFile(std::wstring strLineText)
{
	bool			bRet = false;
	ContentListItem item;
	int				nIndex = 0;
	std::wstring	strTemp;

	auto vec = tool::CodeHelper::vStringSplit(strLineText, L" ");
	for (int i = 0; i < vec.size(); i++) {
		if (vec[i] == L" " || vec[i] == L"") {
			vec.erase(vec.begin() + i);
			i--;
		}
	}

	if (vec.size() != 5) {
		//MessageBox(NULL, L"解析失败!", L"提示", MB_ICONERROR | MB_OK);
		return bRet;
	}

#ifndef _DEBUG
	for (auto& vecitem : m_vCtxTextList) {
		if (vecitem.strLocation == vec[0]) {
			bRet = true;
			break;
		}
	}
	if (bRet == false)
		return bRet;
#endif // !_DEBUG

	item.strLocation = vec[0];
	item.strWeather = vec[1];
	item.strWeatherEx = vec[1];
	item.strTemp = vec[2] + L"度";
	item.strTempEx = vec[3] + L"度";

	strTemp = vec[4];
	if (nIndex = strTemp.find(L"风") != -1) {
		strTemp = strTemp.substr(0, nIndex + 1);
		item.strWind = strTemp;
		item.strWindEx = strTemp;

		strTemp = vec[4];
		strTemp = strTemp.substr(nIndex + 1, strTemp.length() - nIndex);
	}

	auto pfunc_trans = [](std::wstring str) -> std::wstring {
		if (str == L"1") return L"一";
		if (str == L"2") return L"二";
		if (str == L"3") return L"三";
		if (str == L"4") return L"四";
		if (str == L"5") return L"五";
		if (str == L"6") return L"六";
		if (str == L"7") return L"七";
		if (str == L"8") return L"八";
		if (str == L"9") return L"九";
		if (str == L"10") return L"十";
		if (str == L"11") return L"十一";
		if (str == L"12") return L"十二";
		return L"";
	};

	if (nIndex = strTemp.find(L"级") != -1) {
		strTemp = strTemp.substr(0, nIndex);

		strTemp = pfunc_trans(strTemp);
		if (!strTemp.empty()) {
			strTemp += L"级";
		}
		else {
			strTemp = L"空";
		}
		item.strWindLv = strTemp;
		item.strWindLvEx = strTemp;
	}

	ModifyCtxItem(item);
	return true;
}

void AutoMainWnd::ModifyCtxItem(ContentListItem& item)
{
	if (m_vCtxTextList.size() == 0) return;

    bool bSelect = ui.checkBox->isChecked() ? true : false;

	for (auto& vecitem : m_vCtxTextList) {
		if (vecitem.strLocation == item.strLocation) {
			//vecitem.strWeather = item.strWeather;
			//vecitem.strWeatherEx = item.strWeatherEx;
            // 只复制一下组合天气给strWeather就行了
            vecitem.strWeather = GetCompWeatherWithIndexs(item.strWeather, item.strWeatherEx);
			vecitem.strTemp = item.strTemp;
			vecitem.strTempEx = item.strTempEx;
			if (bSelect) {
				vecitem.strWind = item.strWind;
				vecitem.strWindEx = item.strWindEx;
				vecitem.strWindLv = item.strWindLv;
				vecitem.strWindLvEx = item.strWindLvEx;
			}
			break;
		}
	}
	ChangeTextVecToIndexVec();
	ShowContentList();
}

/*
    温度微调
*/
void AutoMainWnd::on_btn_modify_clicked()
{
    if (m_pTempWnd == nullptr) {
        m_pTempWnd = new TempWnd();
    }
    m_pTempWnd->UpdateTempContent();
    m_pTempWnd->show();
}

/*
    tablewidget 单击
*/
void AutoMainWnd::slot_tablewidget_item_clicked(QTableWidgetItem* item)
{
	int nIndex = item->row();
    if (nIndex < 0) return;
	m_curIndex = nIndex;
	m_CopyedItem = m_vCtxTextList[nIndex];
}

/*
    tablewidget 双击
*/
void AutoMainWnd::slot_tablewidget_item_dbclicked(QTableWidgetItem* item)
{
	int nIndex = item->row();
	m_curIndex = nIndex;

	// keep wavname recname and location name
	m_vCtxTextList[nIndex].strWeather = m_CopyedItem.strWeather;
	m_vCtxTextList[nIndex].strWeatherEx = m_CopyedItem.strWeatherEx;
	m_vCtxTextList[nIndex].strTemp = m_CopyedItem.strTemp;
	m_vCtxTextList[nIndex].strTempEx = m_CopyedItem.strTempEx;
	m_vCtxTextList[nIndex].strWind = m_CopyedItem.strWind;
	m_vCtxTextList[nIndex].strWindEx = m_CopyedItem.strWindEx;
	m_vCtxTextList[nIndex].strWindLv = m_CopyedItem.strWindLv;
	m_vCtxTextList[nIndex].strWindLvEx = m_CopyedItem.strWindLvEx;
	m_vCtxTextList[nIndex].strPrecipitation = m_CopyedItem.strPrecipitation;

	// 复制的时候根据配置微调温度
	for (auto& item : CItemInit::Instance()->m_scTempModify) {
		if (_wtoi(item.strIndex.c_str()) == nIndex) {
			ModifyTempWithConfig(m_vCtxTextList[nIndex], item);
		}
	}

	ShowContentList();
	ChangeTextVecToIndexVec();
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

    connect(ui.tableWidget, &QTableWidget::itemClicked, this, &AutoMainWnd::slot_tablewidget_item_clicked);
    connect(ui.tableWidget, &QTableWidget::itemDoubleClicked, this, &AutoMainWnd::slot_tablewidget_item_dbclicked);
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

/*
    自动打开
*/
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
        strText = GetCompWeatherWithIndexs(vec[i].strWeather, vec[i].strWeatherEx);
        m_vCtxTextList[i].strWeather = strText;
        /*
        strText = findfunc(CItemInit::Instance()->g_scWeatherName, vec[i].strWeather);
        m_vCtxTextList[i].strWeather = strText;
        strText = findfunc(CItemInit::Instance()->g_scWeatherName, vec[i].strWeatherEx);
        m_vCtxTextList[i].strWeatherEx = strText;
        */
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

/*
    根据m_vCtxTextList中的weather字段 找出来weather和weatherex值
*/
stWeatherInfo AutoMainWnd::findWeatherAndExWithCompWeather(std::wstring strWeather) {

    stWeatherInfo out;

    //weathersCombination:组合的天气情况
    //weatherNormals：常规的情况
    std::map<std::wstring, stWeatherInfo> weathersCombination = CItemInit::Instance()->g_scWeatherMap;
    std::vector<std::wstring>& weatherNormals = CItemInit::Instance()->g_scWeatherName;

    auto iter = weathersCombination.find(strWeather);
    if (iter != weathersCombination.end()) {
        out = iter->second;
        return out;
    }

    // 如果没有选择前面19的组合键，那么肯定选择的事19之后的。
    out.strWeather = strWeather;
    out.strWeatherEx = strWeather;

    // 这里就没必要查了
    /*
    for (int i = 0; i < weatherNormals.size(); i++) {
        if (weatherNormals[i] == strText) {
            if (i > 19) {
                out.strWeather = strText;
                out.strWeatherEx = strText;
            }
        }
    }
    */
    return out;
}

/*
    根据两个索引获取的组合过的天气字符串，位了展示用
*/
std::wstring AutoMainWnd::GetCompWeatherWithIndexs(std::wstring strWeatherIndex1, std::wstring strWeatherIndex2)
{
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


    int nw1 = 0, nw2 = 0;
    nw1 = _wtoi(strWeatherIndex1.c_str());
    nw2 = _wtoi(strWeatherIndex2.c_str());

    std::wstring strTmp = (CItemInit::Instance()->g_scWeatherName)[nw1];
    std::wstring strTmpEx = (CItemInit::Instance()->g_scWeatherName)[nw2];
    std::wstring strText;

	if ((nw1 > 19 && nw2 < 19) || (nw1 < 19 && nw2 > 19)) {
		strText = compfunc(strTmp, strTmpEx, L"X");
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
    return strText;
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
        // 因为组合后的天气存储在strweather 中，所以
        // 天气特殊处理，从map中查找到对应weather和weatherex  然后重新赋值
        // 如果没有找到，说明选择的事常规的天气。在g_scWeatherName中找
        stWeatherInfo weatherInfo = findWeatherAndExWithCompWeather(m_vCtxTextList[i].strWeather);
        
        m_vCtxIndexList[i].strWeather = findfuncex(CItemInit::Instance()->g_scWeatherName, weatherInfo.strWeather);
        m_vCtxIndexList[i].strWeatherEx = findfuncex(CItemInit::Instance()->g_scWeatherName, weatherInfo.strWeatherEx);
        
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
        /*
        if (nIndex > 19)
            m_vCtxTextList[nCtxIndex].strWeatherEx = strItemText;
        */
    }
        break;

    // type_weatherex不会出发  因为weather和weatherex 组合成了一个
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

    QStringList slisttmp;
    std::wstring strText;

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

        strText = m_vCtxTextList[i].strWeather;
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