#pragma once

#include <QWidget>
#include "ui_TempWnd.h"

#include "stdafx.h"
class TempWnd : public QWidget
{
	Q_OBJECT

public:
	TempWnd(QWidget *parent = nullptr);
	~TempWnd();

	void UpdateTempContent();
private:
	void OnUIInit();
	void ShowContentList();
	void OnSaveToFile();
private slots:
	void on_btn_add_clicked();
	void on_btn_del_clicked();
private:
	Ui::TempWndClass ui;

	std::wstring m_strOpenFilePath;
	std::vector<TempItem> m_vCtxTextList;	// 显示内容集合

	ContentListItem m_CopyedItem;
	int m_curIndex = 0;
};
