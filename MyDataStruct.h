#pragma once

#include "MyDataType.h"

#include<string>
#include<map>
#include<vector>
#include<queue>
#include<mutex>
#include<condition_variable>

#include"xtp_trader_api.h"
#include"xtp_quote_api.h"

using namespace std;

//�˻���Ϣ�ṹ��

struct MyAccountDataStruct {
	//��¼��Ϣ
	char investorID[30];
	char password[30]  ;
	char key[100];
	char marketDataFrontAddr[30];
	int  marketDataPort;
	char tradeFrontAddr[30];
	int  tradePort;
	
	int clientID;//�ͻ��� �����Ƿ�Ϊ��ǰ�������
	int requestID;  //����ID
	bool bIsTradeApiConnected;//����Api�Ƿ����ӳɹ�
	bool bIsMarketDataApiConnected;//����Api�Ƿ����ӳɹ�
	
	XTP::API::TraderApi *pTradeApi;
	XTP::API::QuoteApi* pMarketDataApi;

	//�����˻����ȡ������
	uint64_t sessionID;

};

//�����Ʊ����Ϣ�ṹ��
struct MyStockInfoDataStruct
{
	XTP_EXCHANGE_TYPE exchangeID;
	XTP_TICKER_TYPE instrumentType;
	string instrumentName;
	double priceTick;       //��С�䶯��λ
};


//�̿��������ݽṹ��
struct MyBidAskDataStruct {

	mutex mutex_bidAskData;
	
	string updateTime;
	
	double preLastPrice;
	double currentLastPrice;

	double bidPrice1;
	double askPrice1;

	uint64_t bidVolumel;   //��һ��
	uint64_t askVolumel;	//��һ��

};

//�������ݽṹ��
struct MyMarketDataStruct {
	//������������map
	map<string, MyBidAskDataStruct*>map_marketData;

	//��Ʊ��Ϣ����
	map<string, MyStockInfoDataStruct*>map_allAvailableStockInfo;

	//���ù�Ʊ�����б�  ��ģ����ѯ�ã�
	vector<string>vec_allAvailableInstrumentID;
};

//�������ݽṹ��
struct MyStrategyDataStruct {
	int id;
	bool bIsStartTrading;
	string instrumentID;
	string instrumentName;
	double triggerPercent;
	int orderVolume;
	int forwardTickNum;
};

//�������в��Բ����Ľṹ��
struct MyStrategyTotalStruct {
	int currentID;// ��ǰ���

	mutex mutex_strategySetting;
	vector<MyStrategyDataStruct*>vec_strategySetting;

};



//��Ϣ�������
struct MyInfoTaskQueueStruct
{
	queue<string>queue_message;
	mutex mutex_message;
};