#include "TempWnd.h"
#include <QIcon>
#include <QTableWidget>


TempWnd::TempWnd(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	OnUIInit();

	UpdateTempContent();
}

TempWnd::~TempWnd()
{
}

void TempWnd::UpdateTempContent()
{
	m_strOpenFilePath = PathHelper::Instance()->GetModifyTempFile();
	tool::FileHelp::ReadTempConfigFle(m_strOpenFilePath, m_vCtxTextList);

	CItemInit::Instance()->m_scTempModify = m_vCtxTextList;
	ShowContentList();
}

void TempWnd::on_btn_add_clicked()
{
	TempItem item;
	item.strIndex = ui.bum_edt->text().toStdWString();
	item.strTemp1 = ui.first_edt->text().toStdWString();
	item.strTemp2 = ui.second_edt->text().toStdWString();
	for (auto& t : m_vCtxTextList) {
		if (t.strIndex == item.strIndex) {
			t = item;
			goto exit;
		}
	}
	m_vCtxTextList.push_back(item);

exit:
	ShowContentList();
	OnSaveToFile();
	CItemInit::Instance()->m_scTempModify = m_vCtxTextList;
}

void TempWnd::on_btn_del_clicked()
{
	int nIndex = ui.tableWidget->currentRow();
	if (nIndex < 0) return;
	m_vCtxTextList.erase(m_vCtxTextList.begin() + nIndex);
	tool::FileHelp::WriteTempConfigFle(m_strOpenFilePath, m_vCtxTextList);
	ShowContentList();
}

void TempWnd::OnSaveToFile()
{
	m_vCtxTextList.clear();
	TempItem item;
	int nSize = ui.tableWidget->rowCount();
	QTableWidgetItem* pItem = nullptr;

	for (int i = 0; i < nSize; i++) {
		pItem = ui.tableWidget->item(i, 0);
		item.strIndex = pItem->text().toStdWString();
		pItem = ui.tableWidget->item(i, 1);
		item.strTemp1 = pItem->text().toStdWString();
		pItem = ui.tableWidget->item(i, 2);
		item.strTemp2 = pItem->text().toStdWString();

		m_vCtxTextList.push_back(item);
	}

	tool::FileHelp::WriteTempConfigFle(m_strOpenFilePath, m_vCtxTextList);
}

void TempWnd::OnUIInit()
{
	this->setWindowIcon(QIcon(":/icon/logo"));

	QStringList headers;
	headers << QStringLiteral("序号") << QStringLiteral("第一个温度") << QStringLiteral("第二个温度");
	ui.tableWidget->setColumnCount(headers.size());
	ui.tableWidget->setHorizontalHeaderLabels(headers);
	ui.tableWidget->setShowGrid(true);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setVisible(true);//是否显示水平表头
	ui.tableWidget->verticalHeader()->setVisible(false);//是否显示垂直表头
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->horizontalHeader()->setStretchLastSection(true);
	/*
	ui.tableWidget->setColumnWidth(0, 40);
	ui.tableWidget->setColumnWidth(1, 80);
	ui.tableWidget->setColumnWidth(2, 120);
	*/
}

void TempWnd::ShowContentList()
{
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(m_vCtxTextList.size());

	if (!m_vCtxTextList.size()) return;

	std::sort(m_vCtxTextList.begin(), m_vCtxTextList.end(), [&](const TempItem& t1, const TempItem& t2)->bool {
		return t1.strIndex < t2.strIndex;
	});

	for (int i = 0; i < m_vCtxTextList.size(); ++i) {
		ui.tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdWString(m_vCtxTextList[i].strIndex)));
		ui.tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdWString(m_vCtxTextList[i].strTemp1)));
		ui.tableWidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdWString(m_vCtxTextList[i].strTemp2)));
	}


	QTableWidgetItem* selectedItem = ui.tableWidget->item(0, 0);
	ui.tableWidget->setCurrentItem(selectedItem);
}
