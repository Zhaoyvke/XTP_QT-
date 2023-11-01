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

//账户信息结构体

struct MyAccountDataStruct {
	//登录信息
	char investorID[30];
	char password[30]  ;
	char key[100];
	char marketDataFrontAddr[30];
	int  marketDataPort;
	char tradeFrontAddr[30];
	int  tradePort;
	
	int clientID;//客户号 区分是否为当前程序操作
	int requestID;  //请求ID
	bool bIsTradeApiConnected;//交易Api是否连接成功
	bool bIsMarketDataApiConnected;//行情Api是否连接成功
	
	XTP::API::TraderApi *pTradeApi;
	XTP::API::QuoteApi* pMarketDataApi;

	//连接账户后获取的数据
	uint64_t sessionID;

};

//保存股票的信息结构体
struct MyStockInfoDataStruct
{
	XTP_EXCHANGE_TYPE exchangeID;
	XTP_TICKER_TYPE instrumentType;
	string instrumentName;
	double priceTick;       //最小变动价位
};


//盘口行情数据结构体
struct MyBidAskDataStruct {

	mutex mutex_bidAskData;
	
	string updateTime;
	
	double preLastPrice;
	double currentLastPrice;

	double bidPrice1;
	double askPrice1;

	uint64_t bidVolumel;   //买一量
	uint64_t askVolumel;	//卖一量

};

//行情数据结构体
struct MyMarketDataStruct {
	//最新行情数据map
	map<string, MyBidAskDataStruct*>map_marketData;

	//股票信息数据
	map<string, MyStockInfoDataStruct*>map_allAvailableStockInfo;

	//可用股票代码列表  （模糊查询用）
	vector<string>vec_allAvailableInstrumentID;
};

//策略数据结构体
struct MyStrategyDataStruct {
	int id;
	bool bIsStartTrading;
	string instrumentID;
	string instrumentName;
	double triggerPercent;
	int orderVolume;
	int forwardTickNum;
};

//包含所有策略参数的结构体
struct MyStrategyTotalStruct {
	int currentID;// 当前编号

	mutex mutex_strategySetting;
	vector<MyStrategyDataStruct*>vec_strategySetting;

};



//信息任务队列
struct MyInfoTaskQueueStruct
{
	queue<string>queue_message;
	mutex mutex_message;
};