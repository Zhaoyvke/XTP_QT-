#pragma once
#include"MyDataStruct.h"
#include"MyQtWidgetStruct.h"
#include"MyXtpApi.h"
#include"MyTableModel.h"
#include"MyStrategy.h"
#include "MyQtThreadObject.h"
#include <QtGui/qwindowdefs.h>
#include <QTextEdit>
#include <QPushButton>
#include <QtWidgets/qapplication.h>
#include <QLabel>
#include<QHeaderView>
#include<QMainWindow>
#include <QVBoxLayout>
#include <winsock.h>
#include <QSortFilterProxyModel>
#include"qlineedit.h"
#include"MyTableModel.h"
#include <QMainWindow>
#include<QTableView>
#include<QDateTime>
#include<QCompleter>




class QAction;
class QMenu;
class MyMainWindow :public QMainWindow {
	Q_OBJECT;

public:

	MyMainWindow(QWidget *parent = 0);
	~MyMainWindow();

	std::string save_file_path = "C:\QT5.9\MyXTPTrade\log";//保存xtp api日志的路径，需要有可读写权限
	XTP_LOG_LEVEL log_level = XTP_LOG_LEVEL_TRACE;//xtp api日志的输出级别，建议调试时使用debug级别，正常运行时使用info级别

	//包含上述参数的结构体指针，用来传递至各个thread
	MyAccountDataStruct *m_pMyAccountDataStruct;//账户信息结构体
	MyMarketDataStruct *m_pMyMarketDataStruct;//保存行情结构体
	MyInfoTaskQueueStruct *m_pMyInfoTaskQueueStruct;//信息任务队列
	MyStrategyTotalStruct *m_pMyStrategyTotalStruct;//策略信息结构体

	MyProcessInfoTaskThread* m_pMyProcessInfoTaskThread;//负责处理消息信息类任务


	//MyStrategyDataStruct* v_pMyStrategyDataStruct = new MyStrategyDataStruct;
	
private:
	bool m_bIsSetCompleter;

	//定时更新界面和数据的Timer
	QTimer *m_pUpdateTimer;

	//总体widgets
	QWidget *m_pCentralWidget;
	QTextEdit *m_pTextEdit;    //运行日志
	QPushButton *m_pConnectToAccountPushButton;

	//添加策略
	QLineEdit *m_pInstrumentIDLineEdit;  //合约代码填写框
	QLabel* m_pInstrumentNameLabel;		//合约代码的显示
	QLineEdit *m_pTriggerPercentLineEdit;//触发阈值
	QLineEdit *m_pOrderVolumeLineEdit;		//下单量
	QLineEdit *m_pForwardTickNumLineEdit;	//前进Tick
	QPushButton *m_pAddStockPushButton;    //添加股票按钮
	
	//修改策略
	QLineEdit* m_pPickedIDLineEdit;
	QPushButton* m_pDeleteStockPushButton;

	//mvc
	QTableView *m_pMyTableView;
	MyTableModel* m_pMyTableModel;

	       
	void CreateWidget();

	void SetMainLayout();

	void CreateProcessTaskThread();

	void CreateUpdateTimer();

	void StartStrategyTradingThread();

	
	void SetCompleter(); //模糊查询


	//MainWindow主框架初始化函数
	//初始化数据结构
	void InitDataStructPointers();

	//连接交易
	bool ConnectToTradeApi();
	//连接行情Api
	bool ConnectToMarketDataApi();

	//以下为关闭MainWindow触发的保存数据函数
	void RecordTextEdit();

private slots:

	void slot_ClickedConnectToAccountPushButton();  //发起登录部分

	void slot_InstrumentIDLineEditEditingFinished();
	void slot_ClickedAddStockPushButton();
	void slot_ClickedDeleStockPushButton();

	void slot_ClickedHorizeonHeader(int section);
	void slot_PrintMessages(QString);
	void slot_UpdateTimerTimeOut();
	
	void closeEvent(QCloseEvent* event);
signals://信号
	void sendPrintMessage(QString);
};