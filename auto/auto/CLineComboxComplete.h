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
    void setCompleter(const QString& text); // ��̬����ʾ����б�
    void completeText(const QModelIndex& index); // �������б��е��ʹ�ô����Զ��������ĵ���

protected:
    virtual void keyPressEvent(QKeyEvent* e);
    virtual void focusOutEvent(QFocusEvent* e);
    virtual void focusInEvent(QFocusEvent* e);
private:

    QStringList words; // ��������б�ĵ���
    QListView* listView; // ����б�
    QStringListModel* model; // ����б��model

private:
    ModifyType m_type;
    bool m_bFirstInit = false;
};
