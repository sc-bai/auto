#pragma once

#include "stdafx.h"
#include <QCompleter>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QWidget>
#include "ui_CLineComboxComplete.h"

#include <QItemDelegate>
#include <QListView>
#include <QLineEdit>
class QListView;
class QStringListModel;
class QModelIndex;

class CLineComboxComplete : public QLineEdit {

    Q_OBJECT

signals:
    void signal_select(QString strText, ModifyType type);

public:
    CLineComboxComplete(QWidget* parent = 0);
    CLineComboxComplete(QStringList words, QWidget* parent = 0);

    void SetTextContent(std::wstring strText);
    void SetWordList(QStringList& slist);
    void SetModifyType(ModifyType type);
public slots:
    void setCompleter(const QString& text); // 动态的显示完成列表
    void completeText(const QModelIndex& index); // 点击完成列表中的项，使用此项自动完成输入的单词

protected:
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void focusOutEvent(QFocusEvent* e);
    virtual void focusInEvent(QFocusEvent* e);
private:

    QStringList words; // 整个完成列表的单词
    QListView* listView; // 完成列表
    QStringListModel* model; // 完成列表的model

private:
    ModifyType m_type;
    bool m_bFirstInit = false;
};
