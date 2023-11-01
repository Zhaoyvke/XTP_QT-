#include"MyMainWindow.h"
#include"xtp_trader_api.h"
#include"MyDataStruct.h"
#include<fstream>
#include <QTimer>
#include <QThread>
using namespace std;


//�����ڳ�ʼ��
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

//�������ò���
void MyMainWindow::CreateWidget() {

	m_pCentralWidget = new QWidget;  //��ʼ�������Widget
	QVBoxLayout* v_pCentralLayout = new QVBoxLayout;  //��ֱ����

	QHBoxLayout* v_pLine1 = new QHBoxLayout;  //��һ�еĲ���
	QHBoxLayout* v_pLine2 = new QHBoxLayout;	//�ڶ��еĲ���
	QHBoxLayout* v_pLine3 = new QHBoxLayout;


	m_pConnectToAccountPushButton = new QPushButton(QStringLiteral("�����˻�"));     //������ť
	connect(m_pConnectToAccountPushButton, SIGNAL(clicked()), this, SLOT(slot_ClickedConnectToAccountPushButton()));

	v_pLine1->addWidget(m_pConnectToAccountPushButton);
	v_pLine1->addStretch();

	//��Ӳ��Բ���
	QLabel* v_pQLabell = new QLabel(QStringLiteral("��Ʊ����"));
	QLabel* v_pQLabel2 = new QLabel(QStringLiteral("��Ʊ����"));
	QLabel* v_pQLabel3 = new QLabel(QStringLiteral("������ֵ"));
	QLabel* v_pQLabel4 = new QLabel(QStringLiteral("�µ������֣�"));
	QLabel* v_pQLabel5 = new QLabel(QStringLiteral("ǰ��Tick��"));

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


	m_pAddStockPushButton = new QPushButton(QStringLiteral("��ӹ�Ʊ"));
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

	//������־
	m_pTextEdit = new QTextEdit;
	m_pTextEdit->setFontPointSize(14);
	m_pTextEdit->setReadOnly(true);

	//����1 ��2 ��ӵ����岼����
	v_pCentralLayout->addLayout(v_pLine1);
	v_pCentralLayout->addLayout(v_pLine2);
	v_pCentralLayout->addWidget(m_pTextEdit);

	m_pCentralWidget->setLayout(v_pCentralLayout);



	//�༭����
	QLabel* v_pQLabel6 = new QLabel(QStringLiteral("���"));

	m_pPickedIDLineEdit = new QLineEdit;
	m_pPickedIDLineEdit->setFixedSize(MyQLineEditWidth, MyQLineEditHeight);
	m_pPickedIDLineEdit->setAlignment(Qt::AlignCenter);

	m_pDeleteStockPushButton = new QPushButton(QStringLiteral("ɾ����Ʊ"));
	connect(m_pDeleteStockPushButton, SIGNAL(clicked()), this, SLOT(slot_ClickedDeleStockPushButton()));

	v_pLine3->addWidget(v_pQLabel6);
	v_pLine3->addWidget(m_pPickedIDLineEdit);
	v_pLine3->addWidget(m_pDeleteStockPushButton);
	v_pLine3->addStretch();

	//MVC
	m_pMyTableView = new QTableView;
	m_pMyTableView->setSortingEnabled(true);

	m_pMyTableModel = new MyTableModel(0);
	//��Ҫͨ������һ��QSortFilterProxyModel��ʵ��������
	QSortFilterProxyModel* v_pProxyModel = new QSortFilterProxyModel;
	v_pProxyModel->setSourceModel(m_pMyTableModel);

	m_pMyTableModel->GetData(m_pMyStrategyTotalStruct);
	m_pMyTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_pMyTableView->setModel(v_pProxyModel);

	//�����ͷ��������
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

	this->setWindowTitle(QStringLiteral("**XTP**��Ʊ�������"));
}


//�����¼�����˻�
void MyMainWindow::slot_ClickedConnectToAccountPushButton() {

	//ֻ�ɵ��һ��
	m_pConnectToAccountPushButton->setEnabled(false);

	//����Api
	if (ConnectToTradeApi())//�����ӽ���  ������ӳɹ�  ����������  
	{
		ConnectToMarketDataApi();
	}
}
//���ӽ���API
bool MyMainWindow::ConnectToTradeApi() {

	//��ʼ��Api
	m_pMyAccountDataStruct->pTradeApi = XTP::API::TraderApi::CreateTraderApi(m_pMyAccountDataStruct->clientID, save_file_path.c_str(), log_level);
	
	m_pMyAccountDataStruct->pTradeApi->SubscribePublicTopic(XTP_TERT_QUICK);

	//����key
	m_pMyAccountDataStruct->pTradeApi->SetSoftwareKey(m_pMyAccountDataStruct->key);
	//���ð汾�� �Զ���
	m_pMyAccountDataStruct->pTradeApi->SetSoftwareVersion("1.0");
	//������������ ��λΪ��
	m_pMyAccountDataStruct->pTradeApi->SetHeartBeatInterval(15);

	//ע��Spi
	MyXTPTradeSpi* v_pMyXTPTradeSpi = new MyXTPTradeSpi;

	v_pMyXTPTradeSpi->m_investorID = m_pMyAccountDataStruct->investorID;
	v_pMyXTPTradeSpi->m_pMyAccountDataStruct = m_pMyAccountDataStruct;
	v_pMyXTPTradeSpi->m_pMyInfoTaskQueueStruct = m_pMyInfoTaskQueueStruct;

	m_pMyAccountDataStruct->pTradeApi->RegisterSpi(v_pMyXTPTradeSpi);

	//����Apiʼ��ʹ��tcp   (��¼������ͬ���ģ���ֱ�ӻ�ȡ�ģ�
	uint64_t v_err = m_pMyAccountDataStruct->pTradeApi->Login(m_pMyAccountDataStruct->tradeFrontAddr,
		m_pMyAccountDataStruct->tradePort, m_pMyAccountDataStruct->investorID, m_pMyAccountDataStruct->password, XTP_PROTOCOL_TCP);

	//��½�˻�������¼ʧ���򷵻�0 �ɹ�����sessonID 
	if (v_err != 0) {

		m_pMyAccountDataStruct->sessionID = v_err;
		m_pMyAccountDataStruct->bIsTradeApiConnected = true;

		slot_PrintMessages(QStringLiteral("����Api��¼�ɹ���"));

		return true;
	}
	else {
		//��ȡ������Ϣ
		
		XTPRI* v_p = m_pMyAccountDataStruct->pTradeApi->GetApiLastError();
		slot_PrintMessages(QStringLiteral("����Api��¼ʧ�ܣ�") + QString::number(v_p->error_id) + ""+QString::fromStdString(v_p->error_msg));

		return false;
	}
}




//��������Api
bool MyMainWindow::ConnectToMarketDataApi() {

	//��ʼ��Api
	m_pMyAccountDataStruct->pMarketDataApi = XTP::API::QuoteApi::CreateQuoteApi(m_pMyAccountDataStruct->clientID, save_file_path.c_str(), log_level);
	//������������ ��λΪ��
	m_pMyAccountDataStruct->pMarketDataApi->SetHeartBeatInterval(15);
	//����UDP��������С����λΪmb
	m_pMyAccountDataStruct->pMarketDataApi->SetUDPBufferSize(128);

	//ע��Spi
	MyXTPMarketDataSpi* v_pMyXTPMarketDataSpi = new MyXTPMarketDataSpi;

	v_pMyXTPMarketDataSpi->m_investorID = m_pMyAccountDataStruct->investorID;
	v_pMyXTPMarketDataSpi->m_pMyAccountDataStruct = m_pMyAccountDataStruct;
	v_pMyXTPMarketDataSpi->m_pMyMarketDataStruct = m_pMyMarketDataStruct;
	v_pMyXTPMarketDataSpi->m_pMyInfoTaskQueueStruct = m_pMyInfoTaskQueueStruct;

	m_pMyAccountDataStruct->pMarketDataApi->RegisterSpi(v_pMyXTPMarketDataSpi);

	//��������Api��level1����ʹ��tcp��level2����ʹ��udp
	int v_err = m_pMyAccountDataStruct->pMarketDataApi->Login(m_pMyAccountDataStruct->marketDataFrontAddr, m_pMyAccountDataStruct->marketDataPort,
		m_pMyAccountDataStruct->investorID, m_pMyAccountDataStruct->password, XTP_PROTOCOL_TCP);

	//����API���ӳɹ�����0
	if (v_err == 0)
	{
		slot_PrintMessages(QStringLiteral("����Api��¼�ɹ���"));

		//��ѯ���п��ú�Լ��Ϣ
		m_pMyAccountDataStruct->pMarketDataApi->QueryAllTickers(XTP_EXCHANGE_SH);
		//m_pMyAccountDataStruct->pMarketDataApi->QueryAllTickers(XTP_EXCHANGE_SZ);
		m_pMyAccountDataStruct->pMarketDataApi->QueryAllTickersFullInfo(XTP_EXCHANGE_SZ);
	//	int ret = m_pMyAccountDataStruct->pMarketDataApi->SubscribeAllOptionTickByTick(XTP_EXCHANGE_SH);
		

	}
	else {
		//��ȡ������Ϣ
		XTPRI* v_p = m_pMyAccountDataStruct->pMarketDataApi->GetApiLastError();
		slot_PrintMessages(QStringLiteral("����Api��¼ʧ��:") + QString::number(v_p->error_id) + "" + QString::fromStdString(v_p->error_msg));

		return false;
	}
}



		

//������ʼ���߳�
//�������������߳�
void MyMainWindow::CreateProcessTaskThread() {

	//��Ϣ���������߳�
	m_pMyProcessInfoTaskThread = new MyProcessInfoTaskThread;
	m_pMyProcessInfoTaskThread->m_pMyInfoTaskQueueStruct = m_pMyInfoTaskQueueStruct;

	connect(m_pMyProcessInfoTaskThread, SIGNAL(sendPrintMessages(QString)), this, SLOT(slot_PrintMessages(QString)));


	m_pMyProcessInfoTaskThread->start();

}

//������ʱ���½�������ݵ�timer
void MyMainWindow::CreateUpdateTimer() {
	m_pUpdateTimer = new QTimer(this);
	connect(m_pUpdateTimer, SIGNAL(timeout()), this, SLOT(slot_UpdateTimerTimeOut()));
	m_pUpdateTimer->start(3000);
}

//�������Խ����߳�
void MyMainWindow::StartStrategyTradingThread() {
	std::thread v_thread(CreateStrategyThread, m_pMyStrategyTotalStruct,
		m_pMyAccountDataStruct, m_pMyMarketDataStruct, m_pMyInfoTaskQueueStruct);

	v_thread.detach();
}

/////////////////////////////////////////�������߳����õ��źŲ�////////////////////////////////
//��ӡ��Ϣ
void MyMainWindow::slot_PrintMessages(QString messages)
{
	QString v_qsCurrentTime = QDateTime::currentDateTime().toString(("hh:mm:ss"));
	m_pTextEdit->append(v_qsCurrentTime + " :" + messages);
}


//��Ӻ�Լ����ģ����ѯ
void MyMainWindow::SetCompleter() {

	//���ù�Ʊ��Լ��������
	QStringList v_instrumentIDList;

	for (string v_instrumentID : m_pMyMarketDataStruct->vec_allAvailableInstrumentID) {

		v_instrumentIDList.push_back(QString::fromStdString(v_instrumentID));
	}

	QCompleter* v_instrumentIDCompleter = new QCompleter(v_instrumentIDList);
	m_pInstrumentIDLineEdit->setCompleter(v_instrumentIDCompleter);
	slot_PrintMessages(QStringLiteral("ģ����ѯ�������!"));
}

//���½��������
void MyMainWindow::slot_UpdateTimerTimeOut() {

	//��δ����ģ����ѯ��������
	if (!m_bIsSetCompleter) {

		//��Ҫ����Api�������
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

//��ӹ�Ʊ
void MyMainWindow::slot_ClickedAddStockPushButton() {

	//���鼰����Api�������ǰ���ɲ���
	if (!m_pMyAccountDataStruct->bIsTradeApiConnected || !m_pMyAccountDataStruct->bIsMarketDataApiConnected) {
		return;
	}

	string v_instrumentID = m_pInstrumentIDLineEdit->text().toStdString();

	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[v_instrumentID];
	
	//��Ʊ�������
	if (v_pMyStockInfoDataStruct == NULL) {

		slot_PrintMessages(QStringLiteral("��Ʊ�������"));
		return;
	}		

	MyStrategyDataStruct *v_pMyStrategyDataStruct = new MyStrategyDataStruct;
	v_pMyStrategyDataStruct->id = ++m_pMyStrategyTotalStruct->currentID;
	v_pMyStrategyDataStruct->bIsStartTrading = true;				//����״̬����Ϊ��ʼ
	v_pMyStrategyDataStruct->instrumentID = v_instrumentID;
	v_pMyStrategyDataStruct->instrumentName = m_pInstrumentNameLabel->text().toStdString();
	v_pMyStrategyDataStruct->triggerPercent = m_pTriggerPercentLineEdit->text().toDouble();
	v_pMyStrategyDataStruct->orderVolume = m_pOrderVolumeLineEdit->text().toInt();
	v_pMyStrategyDataStruct->forwardTickNum = m_pForwardTickNumLineEdit->text().toInt();
	
	
	m_pMyStrategyTotalStruct->mutex_strategySetting.lock();
	m_pMyStrategyTotalStruct->vec_strategySetting.push_back(v_pMyStrategyDataStruct);
	m_pMyStrategyTotalStruct->mutex_strategySetting.unlock();

	//return;

	//���Ĺ�Ʊ����
	
	char *v_cInstrumentID = (char*)(v_instrumentID).data();
	char **v_ppInstrumentID = &v_cInstrumentID;  //��Լ����Ķ���ָ��

	int v_error = m_pMyAccountDataStruct->pMarketDataApi->SubscribeMarketData(v_ppInstrumentID, 1, v_pMyStockInfoDataStruct->exchangeID);

	slot_PrintMessages(QStringLiteral("��Ʊ��ӳɹ���"));
}

//ɾ����Ʊ
void MyMainWindow::slot_ClickedDeleStockPushButton()
{
	//����idƥ��
	int v_pickedID = m_pPickedIDLineEdit->text().toInt();

	m_pMyStrategyTotalStruct->mutex_strategySetting.lock();

	for (vector<MyStrategyDataStruct*>::iterator iter = m_pMyStrategyTotalStruct->vec_strategySetting.begin(); iter != m_pMyStrategyTotalStruct->vec_strategySetting.end();
		++iter) {
		if (v_pickedID == (*iter)->id) {
			m_pMyStrategyTotalStruct->vec_strategySetting.erase(iter);
			slot_PrintMessages(QStringLiteral("��Ʊɾ���ɹ���"));

			break;
		}
	}
	m_pMyStrategyTotalStruct->mutex_strategySetting.unlock();
}

//�����ͷ��������
void MyMainWindow::slot_ClickedHorizeonHeader(int section)
{
	m_pMyTableView->sortByColumn(section);
}



void MyMainWindow::closeEvent(QCloseEvent* event)
{
}

/////////////////MainWindow��widget���õ��źŲ�//////////////////



//��Ʊ����༭��ϣ����¹�Ʊ����Label
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
		v_instrumentName = QStringLiteral("�������");
	}
	else {
		//XTPʹ��UTF-8�����ʽ
		v_instrumentName = QString::fromStdString(v_pMyStockInfoDataStruct->instrumentName);
	}

	m_pInstrumentNameLabel->setText(v_instrumentName);
}


