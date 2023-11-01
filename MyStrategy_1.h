#pragma once

#include"MyDataStruct.h"

//创建策略交易线程的函数
void CreateStrategyThread(MyStrategyTotalStruct* pMyStrategyTotalStruct,
	MyAccountDataStruct* pMyAccountDataStruct, MyMarketDataStruct* pMyMarketDataStruct, MyInfoTaskQueueStruct* pMyInfoTaskQueueStruct);

//策略对象
class MyStrategy {
public:
	MyStrategy();
	~MyStrategy();

	//从各个线程以及MianWindow传递来的结构体指针
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