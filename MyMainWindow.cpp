#include"MyMainWindow.h"
#include"xtp_trader_api.h"
#include"MyDataStruct.h"
#include<fstream>
#include <QTimer>
#include <QThread>
using namespace std;


//主窗口初始化
MyMainWindow::MyMainWindow(QWidget* parent) :QMainWindow(parent) {

	m_bIsSetCompleter = false;

	InitDataStructPointers();

	CreateWidget();
	SetMainLayout();

	CreateProcessTaskThread();
	CreateUpdateTimer();
	//ConnectToTradeApi();
	StartStrategyTradingThread();
}

MyMainWindow::~MyMainWindow() {


}
void MyMainWindow::InitDataStructPointers()
{
	m_pMyMarketDataStruct = new MyMarketDataStruct;
	m_pMyInfoTaskQueueStruct = new MyInfoTaskQueueStruct;
	m_pMyProcessInfoTaskThread = new MyProcessInfoTaskThread;
	m_pMyStrategyTotalStruct = new MyStrategyTotalStruct;
	m_pMyStrategyTotalStruct->currentID = 0;

	 m_pMyAccountDataStruct = new MyAccountDataStruct;
	 /*
	 strcpy_s(m_pMyAccountDataStruct->investorID, "300212020030");
	 strcpy_s(m_pMyAccountDataStruct->password, "0T3NYJ4P");
	 strcpy_s(m_pMyAccountDataStruct->key, "b8aa7173bba3470e390d787219b2112e");
	 strcpy_s(m_pMyAccountDataStruct->marketDataFrontAddr, "61.152.102.108");
	 m_pMyAccountDataStruct->marketDataPort = 7709;
	 strcpy_s(m_pMyAccountDataStruct->tradeFrontAddr, "61.152.102.125  ");
	 m_pMyAccountDataStruct->tradePort = 8603;
	 m_pMyAccountDataStruct->clientID = 9;

	 */
	 strcpy_s(m_pMyAccountDataStruct->investorID,"253191001936");
	 strcpy_s(m_pMyAccountDataStruct->password,"EL5ZXKae");
	 strcpy_s(m_pMyAccountDataStruct->key,"b8aa7173bba3470e390d787219b2112e");
	 strcpy_s(m_pMyAccountDataStruct->marketDataFrontAddr,"119.3.103.38");
	 m_pMyAccountDataStruct->marketDataPort = 6002; 
	 strcpy_s(m_pMyAccountDataStruct->tradeFrontAddr,"122.112.139.0");
	 m_pMyAccountDataStruct->tradePort = 6102;
	 m_pMyAccountDataStruct->clientID = 9;
	 

}

//创建设置布局
void MyMainWindow::CreateWidget() {

	m_pCentralWidget = new QWidget;  //初始化总体的Widget
	QVBoxLayout* v_pCentralLayout = new QVBoxLayout;  //垂直布局

	QHBoxLayout* v_pLine1 = new QHBoxLayout;  //第一行的布局
	QHBoxLayout* v_pLine2 = new QHBoxLayout;	//第二行的布局
	QHBoxLayout* v_pLine3 = new QHBoxLayout;


	m_pConnectToAccountPushButton = new QPushButton(QStringLiteral("连接账户"));     //创建按钮
	connect(m_pConnectToAccountPushButton, SIGNAL(clicked()), this, SLOT(slot_ClickedConnectToAccountPushButton()));

	v_pLine1->addWidget(m_pConnectToAccountPushButton);
	v_pLine1->addStretch();

	//添加策略部分
	QLabel* v_pQLabell = new QLabel(QStringLiteral("股票代码"));
	QLabel* v_pQLabel2 = new QLabel(QStringLiteral("股票名称"));
	QLabel* v_pQLabel3 = new QLabel(QStringLiteral("触发阈值"));
	QLabel* v_pQLabel4 = new QLabel(QStringLiteral("下单量（手）"));
	QLabel* v_pQLabel5 = new QLabel(QStringLiteral("前进Tick数"));

	m_pInstrumentIDLineEdit = new QLineEdit;
	m_pInstrumentIDLineEdit->setFixedSize(MyQLineEditWidth, MyQLineEditHeight);
	m_pInstrumentIDLineEdit->setAlignment(Qt::AlignCenter);
	connect(m_pInstrumentIDLineEdit, SIGNAL(editingFinished()), this, SLOT(slot_InstrumentIDLineEditEditingFinished()));

	m_pInstrumentNameLabel = new QLabel;
	m_pInstrumentNameLabel->setFixedSize(MyQLineEditWidth, MyQLineEditHeight);
	m_pInstrumentNameLabel->setAlignment(Qt::AlignCenter);

	m_pTriggerPercentLineEdit = new QLineEdit;
	m_pTriggerPercentLineEdit->setFixedSize(MyQLineEditWidth, MyQLineEditHeight);
	m_pTriggerPercentLineEdit->setAlignment(Qt::AlignCenter);

	m_pOrderVolumeLineEdit = new QLineEdit;
	m_pOrderVolumeLineEdit->setFixedSize(MyQLineEditWidth, MyQLineEditHeight);
	m_pOrderVolumeLineEdit->setAlignment(Qt::AlignCenter);

	m_pForwardTickNumLineEdit = new QLineEdit;
	m_pForwardTickNumLineEdit->setFixedSize(MyQLineEditWidth, MyQLineEditHeight);
	m_pForwardTickNumLineEdit->setAlignment(Qt::AlignCenter);


	m_pAddStockPushButton = new QPushButton(QStringLiteral("添加股票"));
	connect(m_pAddStockPushButton, SIGNAL(clicked()), this, SLOT(slot_ClickedAddStockPushButton()));


	v_pLine2->addWidget(v_pQLabell);
	v_pLine2->addWidget(m_pInstrumentIDLineEdit);
	v_pLine2->addWidget(v_pQLabel2);
	v_pLine2->addWidget(m_pInstrumentNameLabel);
	v_pLine2->addWidget(v_pQLabel3);
	v_pLine2->addWidget(m_pTriggerPercentLineEdit);
	v_pLine2->addWidget(v_pQLabel4);
	v_pLine2->addWidget(m_pOrderVolumeLineEdit);
	v_pLine2->addWidget(v_pQLabel5);
	v_pLine2->addWidget(m_pForwardTickNumLineEdit);
	v_pLine2->addWidget(m_pAddStockPushButton);
	v_pLine2->addStretch();

	//运行日志
	m_pTextEdit = new QTextEdit;
	m_pTextEdit->setFontPointSize(14);
	m_pTextEdit->setReadOnly(true);

	//将行1 行2 添加到总体布局里
	v_pCentralLayout->addLayout(v_pLine1);
	v_pCentralLayout->addLayout(v_pLine2);
	v_pCentralLayout->addWidget(m_pTextEdit);

	m_pCentralWidget->setLayout(v_pCentralLayout);



	//编辑策略
	QLabel* v_pQLabel6 = new QLabel(QStringLiteral("编号"));

	m_pPickedIDLineEdit = new QLineEdit;
	m_pPickedIDLineEdit->setFixedSize(MyQLineEditWidth, MyQLineEditHeight);
	m_pPickedIDLineEdit->setAlignment(Qt::AlignCenter);

	m_pDeleteStockPushButton = new QPushButton(QStringLiteral("删除股票"));
	connect(m_pDeleteStockPushButton, SIGNAL(clicked()), this, SLOT(slot_ClickedDeleStockPushButton()));

	v_pLine3->addWidget(v_pQLabel6);
	v_pLine3->addWidget(m_pPickedIDLineEdit);
	v_pLine3->addWidget(m_pDeleteStockPushButton);
	v_pLine3->addStretch();

	//MVC
	m_pMyTableView = new QTableView;
	m_pMyTableView->setSortingEnabled(true);

	m_pMyTableModel = new MyTableModel(0);
	//需要通过增加一层QSortFilterProxyModel来实现排序功能
	QSortFilterProxyModel* v_pProxyModel = new QSortFilterProxyModel;
	v_pProxyModel->setSourceModel(m_pMyTableModel);

	m_pMyTableModel->GetData(m_pMyStrategyTotalStruct);
	m_pMyTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_pMyTableView->setModel(v_pProxyModel);

	//点击表头触发排序
	connect(m_pMyTableView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(slot_ClickedHorizeonHeader(int)));

	m_pMyTableModel->UpdateData();

	v_pCentralLayout->addLayout(v_pLine1);
	v_pCentralLayout->addLayout(v_pLine2);
	v_pCentralLayout->addWidget(m_pTextEdit);
	v_pCentralLayout->addLayout(v_pLine3);
	v_pCentralLayout->addWidget(m_pMyTableView);

	m_pCentralWidget->setLayout(v_pCentralLayout);


}


void MyMainWindow::SetMainLayout() {

	this->setCentralWidget(m_pCentralWidget);
	this->setMinimumSize(QSize(1650, 750));

	this->setWindowTitle(QStringLiteral("**XTP**股票交易软件"));
}


//点击登录交易账户
void MyMainWindow::slot_ClickedConnectToAccountPushButton() {

	//只可点击一次
	m_pConnectToAccountPushButton->setEnabled(false);

	//连接Api
	if (ConnectToTradeApi())//先连接交易  如果连接成功  再连接行情  
	{
		ConnectToMarketDataApi();
	}
}
//连接交易API
bool MyMainWindow::ConnectToTradeApi() {

	//初始化Api
	m_pMyAccountDataStruct->pTradeApi = XTP::API::TraderApi::CreateTraderApi(m_pMyAccountDataStruct->clientID, save_file_path.c_str(), log_level);
	
	m_pMyAccountDataStruct->pTradeApi->SubscribePublicTopic(XTP_TERT_QUICK);

	//设置key
	m_pMyAccountDataStruct->pTradeApi->SetSoftwareKey(m_pMyAccountDataStruct->key);
	//设置版本号 自定义
	m_pMyAccountDataStruct->pTradeApi->SetSoftwareVersion("1.0");
	//设置心跳区间 单位为秒
	m_pMyAccountDataStruct->pTradeApi->SetHeartBeatInterval(15);

	//注册Spi
	MyXTPTradeSpi* v_pMyXTPTradeSpi = new MyXTPTradeSpi;

	v_pMyXTPTradeSpi->m_investorID = m_pMyAccountDataStruct->investorID;
	v_pMyXTPTradeSpi->m_pMyAccountDataStruct = m_pMyAccountDataStruct;
	v_pMyXTPTradeSpi->m_pMyInfoTaskQueueStruct = m_pMyInfoTaskQueueStruct;

	m_pMyAccountDataStruct->pTradeApi->RegisterSpi(v_pMyXTPTradeSpi);

	//交易Api始终使用tcp   (登录函数是同步的，是直接获取的）
	uint64_t v_err = m_pMyAccountDataStruct->pTradeApi->Login(m_pMyAccountDataStruct->tradeFrontAddr,
		m_pMyAccountDataStruct->tradePort, m_pMyAccountDataStruct->investorID, m_pMyAccountDataStruct->password, XTP_PROTOCOL_TCP);

	//登陆账户，若登录失败则返回0 成功返回sessonID 
	if (v_err != 0) {

		m_pMyAccountDataStruct->sessionID = v_err;
		m_pMyAccountDataStruct->bIsTradeApiConnected = true;

		slot_PrintMessages(QStringLiteral("交易Api登录成功！"));

		return true;
	}
	else {
		//获取错误信息
		
		XTPRI* v_p = m_pMyAccountDataStruct->pTradeApi->GetApiLastError();
		slot_PrintMessages(QStringLiteral("交易Api登录失败：") + QString::number(v_p->error_id) + ""+QString::fromStdString(v_p->error_msg));

		return false;
	}
}




//连接行情Api
bool MyMainWindow::ConnectToMarketDataApi() {

	//初始化Api
	m_pMyAccountDataStruct->pMarketDataApi = XTP::API::QuoteApi::CreateQuoteApi(m_pMyAccountDataStruct->clientID, save_file_path.c_str(), log_level);
	//设置心跳区间 单位为秒
	m_pMyAccountDataStruct->pMarketDataApi->SetHeartBeatInterval(15);
	//设置UDP缓冲区大小，单位为mb
	m_pMyAccountDataStruct->pMarketDataApi->SetUDPBufferSize(128);

	//注册Spi
	MyXTPMarketDataSpi* v_pMyXTPMarketDataSpi = new MyXTPMarketDataSpi;

	v_pMyXTPMarketDataSpi->m_investorID = m_pMyAccountDataStruct->investorID;
	v_pMyXTPMarketDataSpi->m_pMyAccountDataStruct = m_pMyAccountDataStruct;
	v_pMyXTPMarketDataSpi->m_pMyMarketDataStruct = m_pMyMarketDataStruct;
	v_pMyXTPMarketDataSpi->m_pMyInfoTaskQueueStruct = m_pMyInfoTaskQueueStruct;

	m_pMyAccountDataStruct->pMarketDataApi->RegisterSpi(v_pMyXTPMarketDataSpi);

	//连接行情Api，level1行情使用tcp，level2行情使用udp
	int v_err = m_pMyAccountDataStruct->pMarketDataApi->Login(m_pMyAccountDataStruct->marketDataFrontAddr, m_pMyAccountDataStruct->marketDataPort,
		m_pMyAccountDataStruct->investorID, m_pMyAccountDataStruct->password, XTP_PROTOCOL_TCP);

	//行情API连接成功返回0
	if (v_err == 0)
	{
		slot_PrintMessages(QStringLiteral("行情Api登录成功！"));

		//查询所有可用合约信息
		m_pMyAccountDataStruct->pMarketDataApi->QueryAllTickers(XTP_EXCHANGE_SH);
		//m_pMyAccountDataStruct->pMarketDataApi->QueryAllTickers(XTP_EXCHANGE_SZ);
		m_pMyAccountDataStruct->pMarketDataApi->QueryAllTickersFullInfo(XTP_EXCHANGE_SZ);
	//	int ret = m_pMyAccountDataStruct->pMarketDataApi->SubscribeAllOptionTickByTick(XTP_EXCHANGE_SH);
		

	}
	else {
		//获取错误信息
		XTPRI* v_p = m_pMyAccountDataStruct->pMarketDataApi->GetApiLastError();
		slot_PrintMessages(QStringLiteral("行情Api登录失败:") + QString::number(v_p->error_id) + "" + QString::fromStdString(v_p->error_msg));

		return false;
	}
}



		

//建立初始化线程
//创建任务处理总线程
void MyMainWindow::CreateProcessTaskThread() {

	//信息类任务处理线程
	m_pMyProcessInfoTaskThread = new MyProcessInfoTaskThread;
	m_pMyProcessInfoTaskThread->m_pMyInfoTaskQueueStruct = m_pMyInfoTaskQueueStruct;

	connect(m_pMyProcessInfoTaskThread, SIGNAL(sendPrintMessages(QString)), this, SLOT(slot_PrintMessages(QString)));


	m_pMyProcessInfoTaskThread->start();

}

//创建定时更新界面和数据的timer
void MyMainWindow::CreateUpdateTimer() {
	m_pUpdateTimer = new QTimer(this);
	connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(slot_UpdateTimerTimeOut()));
	m_pUpdateTimer->start(3000);
}

//启动策略交易线程
void MyMainWindow::StartStrategyTradingThread() {
	std::thread v_thread(CreateStrategyThread, m_pMyStrategyTotalStruct,
		m_pMyAccountDataStruct, m_pMyMarketDataStruct, m_pMyInfoTaskQueueStruct);

	v_thread.detach();
}

/////////////////////////////////////////任务处理线程所用的信号槽////////////////////////////////
//打印消息
void MyMainWindow::slot_PrintMessages(QString messages)
{
	QString v_qsCurrentTime = QDateTime::currentDateTime().toString(("hh:mm:ss"));
	m_pTextEdit->append(v_qsCurrentTime + " :" + messages);
}


//添加合约代码模糊查询
void MyMainWindow::SetCompleter() {

	//可用股票合约代码设置
	QStringList v_instrumentIDList;

	for (string v_instrumentID : m_pMyMarketDataStruct->vec_allAvailableInstrumentID) {

		v_instrumentIDList.push_back(QString::fromStdString(v_instrumentID));
	}

	QCompleter* v_instrumentIDCompleter = new QCompleter(v_instrumentIDList);
	m_pInstrumentIDLineEdit->setCompleter(v_instrumentIDCompleter);
	slot_PrintMessages(QStringLiteral("模糊查询设置完毕!"));
}

//更新界面和数据
void MyMainWindow::slot_UpdateTimerTimeOut() {

	//若未设置模糊查询，则设置
	if (!m_bIsSetCompleter) {

		//需要行情Api连接完成
		if (m_pMyAccountDataStruct->bIsMarketDataApiConnected) {
			
			SetCompleter();

			m_bIsSetCompleter = true;
		}
	}

	m_pMyTableModel->UpdateData();
}




void MyMainWindow::RecordTextEdit()
{
}

//添加股票
void MyMainWindow::slot_ClickedAddStockPushButton() {

	//行情及交易Api连接完成前不可操作
	if (!m_pMyAccountDataStruct->bIsTradeApiConnected || !m_pMyAccountDataStruct->bIsMarketDataApiConnected) {
		return;
	}

	string v_instrumentID = m_pInstrumentIDLineEdit->text().toStdString();

	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[v_instrumentID];
	
	//股票代码错误
	if (v_pMyStockInfoDataStruct == NULL) {

		slot_PrintMessages(QStringLiteral("股票代码错误！"));
		return;
	}		

	MyStrategyDataStruct *v_pMyStrategyDataStruct = new MyStrategyDataStruct;
	v_pMyStrategyDataStruct->id = ++m_pMyStrategyTotalStruct->currentID;
	v_pMyStrategyDataStruct->bIsStartTrading = true;				//交易状态设置为开始
	v_pMyStrategyDataStruct->instrumentID = v_instrumentID;
	v_pMyStrategyDataStruct->instrumentName = m_pInstrumentNameLabel->text().toStdString();
	v_pMyStrategyDataStruct->triggerPercent = m_pTriggerPercentLineEdit->text().toDouble();
	v_pMyStrategyDataStruct->orderVolume = m_pOrderVolumeLineEdit->text().toInt();
	v_pMyStrategyDataStruct->forwardTickNum = m_pForwardTickNumLineEdit->text().toInt();
	
	
	m_pMyStrategyTotalStruct->mutex_strategySetting.lock();
	m_pMyStrategyTotalStruct->vec_strategySetting.push_back(v_pMyStrategyDataStruct);
	m_pMyStrategyTotalStruct->mutex_strategySetting.unlock();

	//return;

	//订阅股票行情
	
	char *v_cInstrumentID = (char*)(v_instrumentID).data();
	char **v_ppInstrumentID = &v_cInstrumentID;  //合约代码的二级指针

	int v_error = m_pMyAccountDataStruct->pMarketDataApi->SubscribeMarketData(v_ppInstrumentID, 1, v_pMyStockInfoDataStruct->exchangeID);

	slot_PrintMessages(QStringLiteral("股票添加成功！"));
}

//删除股票
void MyMainWindow::slot_ClickedDeleStockPushButton()
{
	//根据id匹配
	int v_pickedID = m_pPickedIDLineEdit->text().toInt();

	m_pMyStrategyTotalStruct->mutex_strategySetting.lock();

	for (vector<MyStrategyDataStruct*>::iterator iter = m_pMyStrategyTotalStruct->vec_strategySetting.begin(); iter != m_pMyStrategyTotalStruct->vec_strategySetting.end();
		++iter) {
		if (v_pickedID == (*iter)->id) {
			m_pMyStrategyTotalStruct->vec_strategySetting.erase(iter);
			slot_PrintMessages(QStringLiteral("股票删除成功！"));

			break;
		}
	}
	m_pMyStrategyTotalStruct->mutex_strategySetting.unlock();
}

//点击表头触发排序
void MyMainWindow::slot_ClickedHorizeonHeader(int section)
{
	m_pMyTableView->sortByColumn(section);
}



void MyMainWindow::closeEvent(QCloseEvent* event)
{
}

/////////////////MainWindow中widget所用的信号槽//////////////////



//股票代码编辑完毕，更新股票名称Label
void MyMainWindow::slot_InstrumentIDLineEditEditingFinished() {
	string v_instrumentID = m_pInstrumentIDLineEdit->text().toStdString();
	
	if (v_instrumentID == "")
	{
		return;
	}
	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[v_instrumentID];

	QString v_instrumentName;

	if (v_pMyStockInfoDataStruct == NULL)
	{
		v_instrumentName = QStringLiteral("错误代码");
	}
	else {
		//XTP使用UTF-8编码格式
		v_instrumentName = QString::fromStdString(v_pMyStockInfoDataStruct->instrumentName);
	}

	m_pInstrumentNameLabel->setText(v_instrumentName);
}


