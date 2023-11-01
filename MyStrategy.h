#pragma once

#include"MyDataStruct.h"

//�������Խ����̵߳ĺ���
void CreateStrategyThread(MyStrategyTotalStruct* pMyStrategyTotalStruct,
	MyAccountDataStruct* pMyAccountDataStruct, MyMarketDataStruct* pMyMarketDataStruct, MyInfoTaskQueueStruct* pMyInfoTaskQueueStruct);

//���Զ���
class MyStrategy {
public:
	MyStrategy();
	~MyStrategy();

	//�Ӹ����߳��Լ�MianWindow�������Ľṹ��ָ��
	MyStrategyTotalStruct *m_pMyStrategyTotalStruct;
	MyAccountDataStruct *m_pMyAccountDataStruct;
	MyMarketDataStruct *m_pMyMarketDataStruct;

	MyInfoTaskQueueStruct* m_pMyInfoTaskQueueStruct;

	void StarWatching();

private:
	void SendNewMessage(string message);
	XTP_MARKET_TYPE GetXtpMarketType(XTP_EXCHANGE_TYPE exchangeID);

	void CreateTradeThread(MyStrategyDataStruct* pMyStrageDataStruct, MyBidAskDataStruct* pMyBidAskDataStruct);
	int InsertOrder(string instrumentID, int volume, double price, MyTradeDirectionType tradeDiectionType);
	void CancelOrder(int orderRef, string instrumentID);
};