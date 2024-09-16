#pragma once

#include "stdafx.h"
#include <QCompleter>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QWidget>
#include "ui_CLineComboxComplete.h"

#include <QItemDelegate>

class ComboBoxDelegate : public QItemDelegate
{
public:
    explicit ComboBoxDelegate(QObject* parent = nullptr) : QItemDelegate(parent) {}
    ~ComboBoxDelegate() = default;

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        QSize size = QItemDelegate::sizeHint(option, index);
        size.setHeight(30);
        return size;
    }
};


class CLineComboxComplete : public QWidget
{
    Q_OBJECT
signals:
    void signal_select(QString strText, int index, ModifyType type);
public:
    CLineComboxComplete(QWidget *parent = nullptr);
    ~CLineComboxComplete();

    void SetTextContent(QString strText);
    void SetTextContent(std::wstring strText);
    void SetComboxItems(QStringList& slist);
    void SetPlaceholderText(QString strText);
    void SetModifyType(ModifyType type);

private:
    void UpdateCombox(QStringList& slist);
    QCompleter* BuilderCompleter(QStringList& slist);
private:
    Ui::CLineComboxCompleteClass ui;
    QCompleter* m_pCompleter = nullptr;

    QStringList m_slist;
    bool m_bConnected = false;
    ModifyType m_type;

    bool m_bIgnore = false; // QComboBox 添加item的时候会出发一次LineEdit 改变。这时候 不处理lineeidt的变动情况
};
