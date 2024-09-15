#pragma once

#include "stdafx.h"
#include <QCompleter>
#include <QWidget>
#include "ui_CLineComboxComplete.h"

class CLineComboxComplete : public QWidget
{
    Q_OBJECT
signals:
    void signal_select(QString strText, int index, ModifyType type);
public:
    CLineComboxComplete(QWidget *parent = nullptr);
    ~CLineComboxComplete();

    void SetComboxItems(QStringList& slist);
    void SetPlaceholderText(QString strText);
    void SetModifyType(ModifyType type);
private:
    Ui::CLineComboxCompleteClass ui;
    QStringList m_slist;
    bool m_bConnected = false;
    ModifyType m_type;
};
