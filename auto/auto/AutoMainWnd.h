#pragma once

#include <QtWidgets/QMainWindow>
#include <QSystemTrayIcon>
#include "CLineComboxComplete.h"
#include "ui_AutoMainWnd.h"
#include "stdafx.h"

// ��ʾ������
enum class TypeItem {
    type_unknow = 0,
    type_first,
    type_second,
    type_both,
};

class AutoMainWnd : public QMainWindow
{
    Q_OBJECT

public:
    AutoMainWnd(QWidget *parent = nullptr);
    ~AutoMainWnd();

private slots:
    void on_btn_open_clicked();
    void on_btn_save_clicked();
private:
    void OnUiInit();
    void InitTraySys();

private:
    void AutoOpenIniFile();
    void InitList();
    void InitContentList();	// ����

    void ShowContentList();
    void ChangeIndexVecToTextVec(std::vector<ContentListItem>& vec);
    void ChangeTextVecToIndexVec();

private slots:
    void slot_text_select(QString strText, int index, ModifyType type);
private:
    Ui::AutoMainWndClass ui;
    QSystemTrayIcon* m_pTrayIcon = nullptr;

private:
    bool			m_bTop = false;
    bool			m_bListInited = false;
    std::wstring m_strOpenFilePath;
    std::vector<ContentListItem> m_vCtxIndexList;	// ��ʾ���ݼ���
    std::vector<ContentListItem> m_vCtxTextList;	// ��ʾ��������

    ContentListItem m_CopyedItem;
    int m_curIndex = 0;
};

