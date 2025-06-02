#pragma once

#include "stdafx.h"
#include <QtWidgets/QMainWindow>
#include <QSystemTrayIcon>
#include <vector>
#include <QTimer>
#include <atomic>

#include <thread>
#include "CLineComboxComplete.h"
#include "ui_AutoMainWnd.h"
#include "stdafx.h"
#include "TempWnd.h"
#include "TTSHelper.h"

// 显示的内容
enum class TypeItem {
    type_unknow = 0,
    type_first,
    type_second,
    type_both,
};

class AutoMainWnd : public QMainWindow
{
    Q_OBJECT

signals:
    void signal_start(int index);
    void signal_finish();
public:
    AutoMainWnd(QWidget *parent = nullptr);
    ~AutoMainWnd();

private slots:
    void on_btn_open_clicked();
    void on_btn_save_clicked();
    void on_btn_copy_clicked();
    void on_btn_read_clicked();
    void on_btn_modify_clicked();
    void on_btn_change_clicked();

    void slot_tablewidget_item_clicked(QTableWidgetItem* item);
    void slot_tablewidget_item_dbclicked(QTableWidgetItem* item);

private:
    void OnUiInit();
    void InitTraySys();

private:
    void AutoOpenIniFile();
    void InitList();
    void InitContentList();	// 内容

    void SetCurrentItemBackColor(int index);
    std::vector<std::string> GetTTSBuildFile(ContentListItem& item);
    std::string BuildTTSText(ContentListItem& item);
    void GetCurrentVoiceType();
    void read_accented_word();

    CLineComboxComplete* BuilderItem(ModifyType type);
    void ShowContentList();
    void ChangeIndexVecToTextVec(std::vector<ContentListItem>& vec);
    void ChangeTextVecToIndexVec();

    void ModifyTempWithConfigEx(std::vector<ContentListItem>& vec, std::vector<TempItem>& vectemp);
    void ModifyTempWithConfig(ContentListItem& item, TempItem& tempItem);

    stWeatherInfo findWeatherAndExWithCompWeather(std::wstring strWeather);

    std::wstring GetCompWeatherWithIndexs(std::wstring strWeatherIndex1, std::wstring strWeatherIndex2);

private: // 读预报文件
	bool HandleReportFile(std::wstring strLineText);
	void ModifyCtxItem(ContentListItem& item); // text


private slots:
    void slot_text_select(QString strText, ModifyType type);
private:
    Ui::AutoMainWndClass ui;
    QSystemTrayIcon* m_pTrayIcon = nullptr;

private:
    bool			m_bTop = false;
    bool			m_bListInited = false;
    std::wstring m_strOpenFilePath;

    // 正常存储方便后续的wav rec索引
    std::vector<ContentListItem> m_vCtxIndexList;	// 显示索引内容
    
    // m_vCtxTextList字段中不在存储weatherex的值，只用weather存储weather和weatherex的组合值，
    // 如果有需要用到 则调用findWeatherAndExWithCompWeather找到 weather和weatherex的值
    std::vector<ContentListItem> m_vCtxTextList;	// 显示内容集合
    
    ContentListItem m_CopyedItem;
    int m_curIndex = 0;

    TempWnd* m_pTempWnd = nullptr;
    bool m_doubleClicked = false;

    QTableWidgetItem* m_clickedItem = nullptr;


    QStringList ini_list_;
    uint64_t ini_index_ = 0;

    TTSConfig tts_config; // 一次性合成多个音色语音 音量 音速

    std::thread m_work_thread;
    std::atomic<bool> m_thread_running_ = false;

    QMap<QString, QString> accented_word_map_;
};

