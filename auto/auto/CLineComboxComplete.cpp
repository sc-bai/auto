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
}

CLineComboxComplete::~CLineComboxComplete()
{}

void CLineComboxComplete::SetComboxItems(QStringList& slist)
{
    m_slist = slist;
    ui.comboBox->addItems(slist);
    ui.comboBox->setView(new QListView());
    ui.comboBox->setLineEdit(ui.lineEdit);
    ui.comboBox->setMaxVisibleItems(10);//下拉列表显示item数

    QCompleter* pComplete = new QCompleter(slist, this);
    ui.lineEdit->setCompleter(pComplete);
    pComplete->setCaseSensitivity(Qt::CaseInsensitive);
    ui.comboBox->setCompleter(pComplete);
    ui.comboBox->setCurrentIndex(-1);
    ui.lineEdit->clear();

    // 保证绑定一次
    if(m_bConnected) return ;
    connect(ui.lineEdit, &QLineEdit::textChanged, [&](QString strText) {
        for (int i=0;i<m_slist.size();i++)  {
            if (m_slist[i] == strText) {
                emit signal_select(strText, i, m_type);
            }
        }
    });
    m_bConnected = true;
}

void CLineComboxComplete::SetPlaceholderText(QString strText)
{
    ui.lineEdit->setPlaceholderText(strText);
}

void CLineComboxComplete::SetModifyType(ModifyType type)
{
    m_type = type;
}
