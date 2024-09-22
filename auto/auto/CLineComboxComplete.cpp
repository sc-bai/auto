#include "CLineComboxComplete.h"
#include <QKeyEvent>
#include <QListView>
#include <QStringListModel>
#include <QDebug>
#include "stdafx.h"

CLineComboxComplete::CLineComboxComplete(QStringList words, QWidget* parent)
    : QLineEdit(parent), words(words) {
    listView = new QListView(this);
    listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    model = new QStringListModel(this);
    listView->setWindowFlags(Qt::ToolTip);
    //connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(setCompleter(const QString&)));
    connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(completeText(const QModelIndex&)));
    listView->hide();
}

CLineComboxComplete::CLineComboxComplete(QWidget* parent /*= 0*/)
    : QLineEdit(parent) {
        listView = new QListView(this);
        listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        model = new QStringListModel(this);
        listView->setWindowFlags(Qt::ToolTip);
       
        connect(listView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(completeText(const QModelIndex&)));
        listView->hide();
}

void CLineComboxComplete::SetTextContent(std::wstring strText)
{
    this->setText(QString::fromStdWString(strText));

    // �״β���
    if (m_bFirstInit == false) {
        connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(setCompleter(const QString&)));
        m_bFirstInit = true;
    }
}

void CLineComboxComplete::SetWordList(QStringList& slist)
{
    words = slist;
}

void CLineComboxComplete::SetModifyType(ModifyType type)
{
    m_type = type;
}

void CLineComboxComplete::focusOutEvent(QFocusEvent* e) {
    listView->hide();
    return QLineEdit::focusOutEvent(e);
}

void CLineComboxComplete::focusInEvent(QFocusEvent* e)
{
    //if (this->text().isEmpty())
    //{
    //    listView->show();
    //}
    return QLineEdit::focusInEvent(e);
}

void CLineComboxComplete::keyPressEvent(QKeyEvent* e) {
    if (!listView->isHidden()) {
        int key = e->key();
        int count = listView->model()->rowCount();
        QModelIndex currentIndex = listView->currentIndex();

        if (Qt::Key_Down == key) {
            // �����·����ʱ���ƶ����ѡ����һ������б��е���
            int row = currentIndex.row() + 1;
            if (row >= count) {
                row = 0;
            }

            QModelIndex index = listView->model()->index(row, 0);
            listView->setCurrentIndex(index);
        }
        else if (Qt::Key_Up == key) {
            // �����·����ʱ���ƶ����ѡ����һ������б��е���
            int row = currentIndex.row() - 1;
            if (row < 0) {
                row = count - 1;
            }
            QModelIndex index = listView->model()->index(row, 0);
            listView->setCurrentIndex(index);
        }
        else if (Qt::Key_Escape == key) {
            // ����Esc��ʱ����������б�
            listView->hide();
        }
        else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
            // ���»س���ʱ��ʹ������б���ѡ�е������������б�
            if (currentIndex.isValid()) {
                QString text = listView->currentIndex().data().toString();
                setText(text);
            }
            listView->hide();
        }
        else {
            // �����������������б���ʹ��QLineEdit�ļ��̰����¼�
            listView->hide();
            QLineEdit::keyPressEvent(e);
        }
    }
    else {
        QLineEdit::keyPressEvent(e);
    }
}

void CLineComboxComplete::setCompleter(const QString& text) {


    if ((text.length() > 1) && (!listView->isHidden())) {
        return;
    }
    /*
    // �������������б��е�ĳ�����ʰ���������ı��������Ҫ��ʾ������б���
    QStringList sl;
    foreach(QString word, words) {
        if (word.contains(text)) {
            sl << word;
        }
    }
    */
    QStringList sl;
    if (text.isEmpty()) {
        sl = words;
        //listView->hide();
    }
    else {
        // ������ʽ
        if (m_type == ModifyType::type_temp || m_type == ModifyType::type_tempex || m_type == ModifyType::type_precipitation || m_type == ModifyType::type_windlv || m_type == ModifyType::type_windlvex) {
            for (int i = 0; i < words.size(); i++) {
                if (words[i].contains(text)) {
                    sl.push_back(words[i]);
                }
            }
        }

        // ����ĸ������ʽ
        else {
			std::string strAsci = text.toStdString();
			std::wstring strWText;
			char szEnglishText[260] = { 0 };
			unsigned short szChineseText[260] = { 0 };
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
			for (int i = 0; i < words.size(); i++) {
				strWText = words[i].toStdWString();
				strAsci = tool::CodeHelper::WStr2Str(strWText);
				ZeroMemory(szlistItem, sizeof(szlistItem));
				memcpy(szlistItem, strAsci.c_str(), strAsci.length());
				if (tool::ChinaStringHandle::Filter(szlistItem, szChineseText, j) == 0) {
					if (!words[i].isEmpty()) {
						sl.push_back(words[i]);
					}
				}
			}
        }
        
    }

    model->setStringList(sl);
    listView->setModel(model);

    if (model->rowCount() == 0) {
        listView->hide();
        return;
    }

    // Position the text edit
    listView->setMinimumWidth(width());
    listView->setMaximumWidth(width());

    QPoint p(0, height());
    int x = mapToGlobal(p).x();
    int y = mapToGlobal(p).y() + 1;

    listView->move(x, y);
    listView->show();
}

void CLineComboxComplete::completeText(const QModelIndex& index) {

    QString text = index.data().toString();

    qDebug() << index.row();

    setText(text);
    emit signal_select(text, index.row(), m_type);

    listView->hide();

}