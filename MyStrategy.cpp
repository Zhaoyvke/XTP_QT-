#include"MyStrategy.h"
#include"MyDataStruct.h"
#include<algorithm>
#include<qdatastream.h>
#include"MyXtpApi.h"
#include<Windows.h>
#include <iostream>


//创建策略线程的函数
void CreateStrategyThread(MyStrategyTotalStruct* pMyStrategyTotalStruct, MyAccountDataStruct* pMyAccountDataStruct, MyMarketDataStruct* pMyMarketDataStruct,
	MyInfoTaskQueueStruct* pMyInfoTaskQueueStruct) {
	
	MyStrategy* v_pMyStrategy = new MyStrategy;

	v_pMyStrategy->m_pMyStrategyTotalStruct = pMyStrategyTotalStruct;
	v_pMyStrategy->m_pMyAccountDataStruct = pMyAccountDataStruct;
	v_pMyStrategy->m_pMyMarketDataStruct = pMyMarketDataStruct;
	v_pMyStrategy->m_pMyInfoTaskQueueStruct = pMyInfoTaskQueueStruct;

	v_pMyStrategy->StarWatching();
}

//////////////////////////MyStrategy///////////////////////////
MyStrategy::MyStrategy() {
	
}
MyStrategy::~MyStrategy() {

}

//发送消息的封装函数

void MyStrategy::SendNewMessage(string message) {

	
	m_pMyInfoTaskQueueStruct->mutex_message.lock();
	m_pMyInfoTaskQueueStruct->queue_message.push(message);
	m_pMyInfoTaskQueueStruct->mutex_message.unlock();
}

//获取市场类型
XTP_MARKET_TYPE MyStrategy::GetXtpMarketType(XTP_EXCHANGE_TYPE exchangeID) {

	if (exchangeID == XTP_EXCHANGE_SH) {

		return XTP_MKT_SH_A;
	}
	return XTP_MKT_SZ_A;
}


//交易线程
void MyStrategy::CreateTradeThread(MyStrategyDataStruct* pMyStrategyDataStruct, MyBidAskDataStruct* pMyBidAskDataStruct) {

	SendNewMessage("股票代码" + pMyStrategyDataStruct->instrumentID + "触发交易！");

	//切换状态
	pMyStrategyDataStruct->bIsStartTrading = false;//(保证同一个股票不会连续触发)

	//获取合约信息
	
	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[pMyStrategyDataStruct->instrumentID];
	//对手价前进N个tick下单
	double v_priceToOrder = pMyBidAskDataStruct->askPrice1 + pMyStrategyDataStruct->forwardTickNum *v_pMyStockInfoDataStruct->priceTick;

	//return;

	int v_orderRef = InsertOrder(pMyStrategyDataStruct->instrumentID, pMyStrategyDataStruct->orderVolume,
		v_priceToOrder, Buy_MyTradeDirectionType);

	//下单失败
	if (v_orderRef == 0) {
		return;
	}
	//定时撤单
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	CancelOrder(v_orderRef, pMyStrategyDataStruct->instrumentID);
}



//下单请求
int MyStrategy::InsertOrder(string instrumentID, int volume, double price, MyTradeDirectionType tradeDirection) {

	//获取合约信息
	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[instrumentID];

	//////////////////////////公共的报单部分///////////////////////////////////
	XTPOrderInsertInfo v_req;
	memset(&v_req, 0, sizeof(XTPOrderInsertInfo));

	strcpy_s(v_req.ticker, instrumentID.c_str());
	v_req.market = GetXtpMarketType(v_pMyStockInfoDataStruct->exchangeID);
	v_req.price = price;
	v_req.quantity = volume * 100;
	v_req.position_effect = XTP_POSITION_EFFECT_INIT;
	v_req.business_type = XTP_BUSINESS_TYPE_CASH;
	v_req.price_type = XTP_PRICE_LIMIT;

	if (tradeDirection == Buy_MyTradeDirectionType) {

		v_req.side = XTP_SIDE_BUY;
	}
	else {
		v_req.side = XTP_SIDE_SELL;
	}
	//下单并返回单号，若返回值为0则说明下单失败
	uint64_t v_orderRef = m_pMyAccountDataStruct->pTradeApi->InsertOrder(&v_req, m_pMyAccountDataStruct->sessionID);

	if (v_orderRef == 0) {

		SendNewMessage("合约" + instrumentID + "下单失败");
	}
	else {

		SendNewMessage("合约" + instrumentID + "下单成功！");
	}
	return v_orderRef;
}

//撤单请求
void MyStrategy::CancelOrder(int orderRef, string instrumentID) {

	uint64_t v_err = m_pMyAccountDataStruct->pTradeApi->CancelOrder(orderRef, m_pMyAccountDataStruct->sessionID);	

	if (v_err == 0) {
		SendNewMessage("合约" + instrumentID + "撤单请求发送失败！");
	}
	else {
		SendNewMessage("合约" + instrumentID + "撤单请求发送成功！");
	}
}



//主交易函数
void MyStrategy::StarWatching()
{
	SendNewMessage("策略交易线程启动成功！");
	
	
		while (true) 
		{

			if (!m_pMyAccountDataStruct->bIsTradeApiConnected || !m_pMyAccountDataStruct->bIsMarketDataApiConnected) 
			{
				continue;
			}
			
			//定时1秒驱动
			std::this_thread::sleep_for(std::chrono::seconds(1));

			//依次遍历所有股票池
			m_pMyStrategyTotalStruct->mutex_strategySetting.lock();
			if (m_pMyStrategyTotalStruct->vec_strategySetting.empty()) {
				//return ;
			}

			for (MyStrategyDataStruct* v_pMyStrategyDataStruct : m_pMyStrategyTotalStruct->vec_strategySetting) 
			{
				
				if (!v_pMyStrategyDataStruct->bIsStartTrading) {
					continue;
				}

				//获取行情
				MyBidAskDataStruct* v_pMyBidAskDataStruct = m_pMyMarketDataStruct->map_marketData[v_pMyStrategyDataStruct->instrumentID];
				//当前价格=0，说明可能出现异常情况   
				if (v_pMyBidAskDataStruct->currentLastPrice == 0|| v_pMyBidAskDataStruct->preLastPrice == 0) {
					continue;
				}
				
				if (v_pMyBidAskDataStruct == NULL) {
					continue;
				}

				//判断是否触发
				if (v_pMyBidAskDataStruct->currentLastPrice / v_pMyBidAskDataStruct->preLastPrice - 1 < v_pMyStrategyDataStruct->triggerPercent / 100) {


					continue;
				}

				SendNewMessage("股票代码" + v_pMyStrategyDataStruct->instrumentID + "触发交易！");
				//触发交易完  产生结束点。
				v_pMyStrategyDataStruct->bIsStartTrading = false;

				//创建交易线程
				std::thread v_thread(&MyStrategy::CreateTradeThread, this, v_pMyStrategyDataStruct, v_pMyBidAskDataStruct);
				v_thread.detach();

				//获取股票信息
				/*
				MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[v_pMyStrategyDataStruct->instrumentID];

				InsertOrder(m_pMyStrategyDataStruct->instrumentID, v_pMyStrategyuDataStruct->orderVolume,
					v_pMyBidAskDataStruct->askPricel + v_pMyStockInfoDataStruct->priceTick * v_pMyStrategy);*/

				
			}
			m_pMyStrategyTotalStruct->mutex_strategySetting.unlock();
		}
}
