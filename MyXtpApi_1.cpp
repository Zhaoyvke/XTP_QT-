#include"MyXtpApi.h"
#include<Windows.h>

using namespace std;

//////////////////////////////////MyXTPTradeSPi////////////////////////////
///发送消息封装函数
void MyXTPTradeSpi::SendNewMessage(string message) {

	m_pMyInfoTaskQueueStruct->mutex_message.lock();
	m_pMyInfoTaskQueueStruct->queue_message.push(message);
	m_pMyInfoTaskQueueStruct->mutex_message.unlock();
}

//检查XTP返回的信息中是否有错
bool MyXTPTradeSpi::IsErrorRspInfo(XTPRI* error_info) {
	if (error_info != NULL) {
		if (error_info->error_id != 0) {

			SendNewMessage(__FUNCTION__);
			SendNewMessage(to_string(error_info->error_id));
			SendNewMessage(error_info->error_msg);
			return true;
		}
	}
	return false;
}

//成交回报
void MyXTPTradeSpi::OnTradeEvent(XTPTradeReport* trade_info, uint64_t session_id) {
	char v_buff[1024] = { 0 };
	snprintf(v_buff, 1024, "成交回报：合约代码：%s 单号：%i 成交量：%i  成交价:%f",trade_info->ticker,trade_info->order_xtp_id,trade_info->quantity,trade_info->price);
	SendNewMessage(v_buff);
}

//下单回报，当前关注撤单及报错单

void MyXTPTradeSpi::OnOrderEvent(XTPOrderInfo* order_info, XTPRI* error_info, uint64_t session_id) {

	if (!IsErrorRspInfo(error_info)) {

		//撤单
		if (order_info->order_status == XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING || order_info->order_status == XTP_ORDER_STATUS_CANCELED) {

			char v_buff[1024] = { 0 };
			snprintf(v_buff, 1024, "撤单回报：合约代码：%s 单号:%i 撤单量:%i", order_info->ticker, order_info->order_xtp_id, order_info->qty_left);
			SendNewMessage(v_buff);
		}
	}
	//报错单
	else {
		
		char v_buff[1024] = { 0 };
		snprintf(v_buff,1024, "错误单回报：错误单状态：%c 合约代码:%s  单号：%i 错误单量:%i", order_info->order_status, order_info->ticker, order_info->order_xtp_id, order_info->qty_left);
		SendNewMessage(v_buff);
	}
}


//服务器错误信息
void MyXTPTradeSpi::OnError(XTPRI* error_info) {
	if (error_info != NULL) {

		if (error_info->error_id != 0) {

			SendNewMessage(__FUNCTION__);
			SendNewMessage(error_info->error_msg);
		}
	}
}



///////////////////////////MyXTPMarketDataSpi////////////////////////////////////////
//发送消息封装函数

void MyXTPMarketDataSpi::SendNewMessage(string message) {

	m_pMyInfoTaskQueueStruct->mutex_message.lock();
	m_pMyInfoTaskQueueStruct->queue_message.push(message);
	m_pMyInfoTaskQueueStruct->mutex_message.unlock();
}


//检查是否有错误信息
bool MyXTPMarketDataSpi::IsErrorRspInfo(XTPRI* error_info) {
	if (error_info != NULL)
	{
		if (error_info->error_id != 0) {
			SendNewMessage(__FUNCTION__);
			SendNewMessage(error_info->error_msg);
			return true;
		}
	}
	return false;
}


//查询所有可用合约信息回调
void MyXTPMarketDataSpi::OnQueryAllTickers(XTPQSI* ticker_info, XTPRI* error_info, bool is_last) {
	if (!IsErrorRspInfo(error_info)) {

		string v_instrumentID = ticker_info->ticker;

		//初始化股票信息结构体
		MyStockInfoDataStruct* v_pMyStockInfoDataStruct = new MyStockInfoDataStruct;

		v_pMyStockInfoDataStruct->instrumentName = ticker_info->ticker_name;
		v_pMyStockInfoDataStruct->exchangeID = ticker_info->exchange_id;
		v_pMyStockInfoDataStruct->instrumentType = ticker_info->ticker_type;
		v_pMyStockInfoDataStruct->priceTick = ticker_info->price_tick;//变动价位

		m_pMyMarketDataStruct->map_allAvailableStockInfo[v_instrumentID] = v_pMyStockInfoDataStruct;

		//初始化行情数据结构体
		MyBidAskDataStruct* v_pMyBidAskDataStruct = new MyBidAskDataStruct;
		v_pMyBidAskDataStruct->preLastPrice = 0;
		v_pMyBidAskDataStruct->currentLastPrice = 0;
		v_pMyBidAskDataStruct->bidPrice1 = 0;
		v_pMyBidAskDataStruct->askPrice1 = 0;
		v_pMyBidAskDataStruct->bidVolumel = 0;
		v_pMyBidAskDataStruct->askVolumel = 0;

		m_pMyMarketDataStruct->map_marketData[v_instrumentID] = v_pMyBidAskDataStruct;

		//股票代码列表
		m_pMyMarketDataStruct->vec_allAvailableInstrumentID.push_back(v_instrumentID);
	}

	//所有可用的合约代码分两次查询，先上证后深证，以深证查询完毕为结尾
	if (is_last && ticker_info->exchange_id == XTP_EXCHANGE_SZ)
	{
		m_pMyAccountDataStruct->bIsMarketDataApiConnected = true;

		SendNewMessage("行情Api连接完成!");
	}
}



//订阅行情应答
void MyXTPMarketDataSpi::OnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last) {
	if (!IsErrorRspInfo(error_info)) {

		string v_instrumentID = ticker->ticker;

		SendNewMessage("合约" + v_instrumentID + "行情订阅成功！");
	}
}

//深度行情通知
void MyXTPMarketDataSpi::OnDepthMarketData(XTPMD* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) {

	string v_instrumentID = market_data->ticker;

	//更新行情数据
	MyBidAskDataStruct* v_pMyBidAskDataStruct = m_pMyMarketDataStruct->map_marketData[v_instrumentID];

	v_pMyBidAskDataStruct->mutex_bidAskData.lock();

	v_pMyBidAskDataStruct->updateTime = market_data->data_time;
	v_pMyBidAskDataStruct->preLastPrice = v_pMyBidAskDataStruct->currentLastPrice;  //历史数据
	v_pMyBidAskDataStruct->currentLastPrice = market_data->last_price;				//最新数据
	v_pMyBidAskDataStruct->bidPrice1 = market_data->bid[0];
	v_pMyBidAskDataStruct->askPrice1 = market_data->ask[0];
	v_pMyBidAskDataStruct->bidVolumel = market_data->bid_qty[0];
	v_pMyBidAskDataStruct->askVolumel = market_data->ask_qty[0];

	v_pMyBidAskDataStruct->mutex_bidAskData.unlock();

	char v_buff[1024] = { 0 };
	snprintf(v_buff, 1024, "行情数据更新：合约代码：%s 买一价:%f 卖一价：%f",
		market_data->ticker, market_data->bid[0], market_data->ask[0]);
	SendNewMessage(v_buff);

}


//行情连接中断
void MyXTPMarketDataSpi::OnDisconnected(int reason) {

	SendNewMessage("行情Api连接中断！");
}

//服务器错误信息
void MyXTPMarketDataSpi::OnError(XTPRI* error_info) {
	if (error_info != NULL) {
		
		if (error_info != NULL) {

			if (error_info->error_id != 0) {
				SendNewMessage(__FUNCTION__);
				SendNewMessage(error_info->error_msg);
			}
		}
	}
}


