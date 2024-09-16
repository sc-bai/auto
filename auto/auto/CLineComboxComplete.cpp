/*
    模糊查询
*/

#include <qlistview.h>
#include "CLineComboxComplete.h"

CLineComboxComplete::CLineComboxComplete(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    //ui.comboBox->setStyleSheet("QComboBox::drop-down {  subcontrol - origin: padding;   subcontrol - position: top right;width: 15px;border:none;}");
    ui.comboBox->setStyleSheet("QComboBox {  border:none;}");
    ui.comboBox->setVisible(false);
}

CLineComboxComplete::~CLineComboxComplete()
{}

void CLineComboxComplete::SetTextContent(QString strText)
{
    ui.lineEdit->setText(strText);
}

void CLineComboxComplete::SetTextContent(std::wstring strText)
{
    QString strQText = QString::fromStdWString(strText);
    SetTextContent(strQText);
}
QCompleter* CLineComboxComplete::BuilderCompleter(QStringList& slist) {

    QStringListModel* stringListModel = new QStringListModel(this);
    if (m_pCompleter) {
        delete m_pCompleter ;
        m_pCompleter = nullptr;
    }
    m_pCompleter = new QCompleter(slist);
    QListView* listView = new QListView;
    listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pCompleter->setPopup(listView);
    m_pCompleter->setFilterMode(Qt::MatchStartsWith);
    //m_pCompleter->setCaseSensitivity(Qt::CaseInsensitive);//大小写匹配
    QAbstractItemView* view = m_pCompleter->popup();
    view->setItemDelegate(new ComboBoxDelegate()); //设置行高
    view->setStyleSheet(QString("QListView { font-size: 13px;}"));
    //设置Eidt的模糊查询对象

    return m_pCompleter;
}

void CLineComboxComplete::SetComboxItems(QStringList& slist) {
    m_slist = slist;
    BuilderCompleter(slist);
    ui.lineEdit->setCompleter(m_pCompleter);



    connect(ui.lineEdit, &QLineEdit::textChanged, [&](QString strText) {
        //proxyModel->SetFilterString(strText);
        return;
        if (strText.isEmpty()) {
            BuilderCompleter(m_slist);
            ui.lineEdit->setCompleter(m_pCompleter);
            return;
        }
        // 英文首字母匹配
        QStringList sListShow;
        std::string strAsci = strText.toStdString();
        std::wstring strWText;
        char szEnglishText[MAX_PATH] = { 0 };
        unsigned short szChineseText[MAX_PATH] = { 0 };
        memcpy(szEnglishText, strAsci.c_str(), strAsci.length());
        int j = 0;
        for (int i = 0; i < strlen(szEnglishText); i++) {
            if (szEnglishText[i] != 0) {
                tool::ChinaStringHandle::EnglishToChinese(szEnglishText, szChineseText, i);
                j++;
            }
            else
                break;
        }

        char szlistItem[MAX_PATH] = { 0 };
        for (int i = 0; i < m_slist.size(); i++) {
            strWText = m_slist[i].toStdWString();
            strAsci = tool::CodeHelper::WStr2Str(strWText);
            ZeroMemory(szlistItem, sizeof(szlistItem));
            memcpy(szlistItem, strAsci.c_str(), strAsci.length());
            if (tool::ChinaStringHandle::Filter(szlistItem, szChineseText, j) == 0) {
                sListShow.push_back(m_slist[i]);
            }
        }


        BuilderCompleter(sListShow);
        ui.lineEdit->setCompleter(m_pCompleter);
        
    });
}


/*
void CLineComboxComplete::SetComboxItems(QStringList& slist)
{
    m_slist = slist;
    ui.comboBox->addItems(slist);
    ui.comboBox->setView(new QListView());
    ui.comboBox->setLineEdit(ui.lineEdit);
    ui.comboBox->setMaxVisibleItems(10);//下拉列表显示item数

    QCompleter* pComplete = new QCompleter(slist, this);
    pComplete->setFilterMode(Qt::MatchContains);

    ui.lineEdit->setCompleter(pComplete);
    pComplete->setCaseSensitivity(Qt::CaseInsensitive);
    ui.comboBox->setCompleter(pComplete);
    ui.comboBox->setCurrentIndex(-1);
    m_bIgnore= true;
    ui.lineEdit->clear();

    // 保证绑定一次
    if(m_bConnected) return ;
    connect(ui.lineEdit, &QLineEdit::textChanged, [&](QString strText) {
        if (m_bIgnore) {
            m_bIgnore = false;
            return;
        }
        
        // 如果时候文字匹配
        if (strText.isEmpty()) {
            emit signal_select(strText, -1, m_type);
            UpdateCombox(m_slist);
            return;
        }
       
       if(strText.isEmpty()) return;
        for (int i = 0; i < m_slist.size(); i++) {
            if (m_slist[i] == strText) {
                emit signal_select(strText, i, m_type);
                UpdateCombox(m_slist);
                return;
            }
        }
         
        // 英文首字母匹配
        QStringList sListShow;
        std::string strAsci = strText.toStdString();
        std::wstring strWText ;
        char szEnglishText[MAX_PATH] = { 0 };
        unsigned short szChineseText[MAX_PATH] = { 0 };
        memcpy(szEnglishText, strAsci.c_str(), strAsci.length());
        int j = 0;
        for (int i = 0; i < strlen(szEnglishText); i++) {
            if (szEnglishText[i] != 0) {
                tool::ChinaStringHandle::EnglishToChinese(szEnglishText, szChineseText, i);
                j++;
            }
            else
                break;
        }
       
        char szlistItem[MAX_PATH] = { 0 };
        for (int i = 0; i < m_slist.size(); i++) {
            strWText = m_slist[i].toStdWString();
            strAsci = tool::CodeHelper::WStr2Str(strWText);
            ZeroMemory(szlistItem, sizeof(szlistItem));
            memcpy(szlistItem, strAsci.c_str(), strAsci.length());
            if (tool::ChinaStringHandle::Filter(szlistItem, szChineseText, j) == 0) {
                sListShow.push_back(m_slist[i]);
            }
        }
        UpdateCombox(sListShow);
    });
    m_bConnected = true;
}
*/
void CLineComboxComplete::SetPlaceholderText(QString strText)
{
    ui.lineEdit->setPlaceholderText(strText);
}

void CLineComboxComplete::SetModifyType(ModifyType type)
{
    m_type = type;
}

void CLineComboxComplete::UpdateCombox(QStringList& slist)
{
    if (ui.comboBox->count() == slist.size()) {
        return;
    }
    m_bIgnore = true;
    ui.comboBox->clear();
    ui.comboBox->addItems(slist);
    ui.comboBox->setCurrentIndex(-1);
}
