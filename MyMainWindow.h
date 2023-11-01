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

	std::string save_file_path = "C:\QT5.9\MyXTPTrade\log";//����xtp api��־��·������Ҫ�пɶ�дȨ��
	XTP_LOG_LEVEL log_level = XTP_LOG_LEVEL_TRACE;//xtp api��־��������𣬽������ʱʹ��debug������������ʱʹ��info����

	//�������������Ľṹ��ָ�룬��������������thread
	MyAccountDataStruct *m_pMyAccountDataStruct;//�˻���Ϣ�ṹ��
	MyMarketDataStruct *m_pMyMarketDataStruct;//��������ṹ��
	MyInfoTaskQueueStruct *m_pMyInfoTaskQueueStruct;//��Ϣ�������
	MyStrategyTotalStruct *m_pMyStrategyTotalStruct;//������Ϣ�ṹ��

	MyProcessInfoTaskThread* m_pMyProcessInfoTaskThread;//��������Ϣ��Ϣ������


	//MyStrategyDataStruct* v_pMyStrategyDataStruct = new MyStrategyDataStruct;
	
private:
	bool m_bIsSetCompleter;

	//��ʱ���½�������ݵ�Timer
	QTimer *m_pUpdateTimer;

	//����widgets
	QWidget *m_pCentralWidget;
	QTextEdit *m_pTextEdit;    //������־
	QPushButton *m_pConnectToAccountPushButton;

	//��Ӳ���
	QLineEdit *m_pInstrumentIDLineEdit;  //��Լ������д��
	QLabel* m_pInstrumentNameLabel;		//��Լ�������ʾ
	QLineEdit *m_pTriggerPercentLineEdit;//������ֵ
	QLineEdit *m_pOrderVolumeLineEdit;		//�µ���
	QLineEdit *m_pForwardTickNumLineEdit;	//ǰ��Tick
	QPushButton *m_pAddStockPushButton;    //��ӹ�Ʊ��ť
	
	//�޸Ĳ���
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

	
	void SetCompleter(); //ģ����ѯ


	//MainWindow����ܳ�ʼ������
	//��ʼ�����ݽṹ
	void InitDataStructPointers();

	//���ӽ���
	bool ConnectToTradeApi();
	//��������Api
	bool ConnectToMarketDataApi();

	//����Ϊ�ر�MainWindow�����ı������ݺ���
	void RecordTextEdit();

private slots:

	void slot_ClickedConnectToAccountPushButton();  //�����¼����

	void slot_InstrumentIDLineEditEditingFinished();
	void slot_ClickedAddStockPushButton();
	void slot_ClickedDeleStockPushButton();

	void slot_ClickedHorizeonHeader(int section);
	void slot_PrintMessages(QString);
	void slot_UpdateTimerTimeOut();
	
	void closeEvent(QCloseEvent* event);
signals://�ź�
	void sendPrintMessage(QString);
};