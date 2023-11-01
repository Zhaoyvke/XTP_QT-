#include"MyXtpApi.h"
#include<Windows.h>
#include<iostream>
#include<io.h>
#include<direct.h>
#include <utility>
#include <functional>
#include <fstream>
#include <stdlib.h>
#include<string>
#include <wchar.h>
using namespace std;

/**********23/4/6**********/
 char targetpath[] = "D:\CTP\Project_csv";
//股票信息数据
 map<string, MyStockInfoDataStruct*>map_allAvailableStockInfo;

//可用股票代码列表  （模糊查询用）
 vector<string>vec_allAvailableInstrumentID;

///行情查询返回股票（含盘口）
 map<string, MyBidAskDataStruct*>map_marketData;
//CSV
 map<string, string>instrumentCsvPathMap;
/**********23/4/6**********/
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
/**********23/4/6/begin**********/
bool MyXTPMarketDataSpi::createDir(std::string path)
{
	if (access(path.c_str(), 0)==-1) {//确定文件是否存在
		CreateDirectory(path.c_str(), NULL);
		//mkdir(path.c_str());
		/*if (mkdir(path.c_str()) == 0) //0 成功
		{
			return true;
		}
		return false;*/
	}
	return true;
}


std::string MyXTPMarketDataSpi::string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

const char*  MyXTPMarketDataSpi:: EnumToString(XTP_EXCHANGE_TYPE c)
{
	switch (c) {
	case XTP_EXCHANGE_SH:   return "XTP_EXCHANGE_SH ";
	case XTP_EXCHANGE_SZ:  return "XTP_EXCHANGE_SZ";
	case XTP_EXCHANGE_UNKNOWN:  return "XTP_EXCHANGE_UNKNOWN";
	default: return "";
	}
}
/**********23/4/6**********/

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

		v_pMyStockInfoDataStruct->instrumentName = ticker_info->ticker;
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
		
		/*
		char p_buff[] = { "90011726"};
		char* v_buff = p_buff;
		m_pMyAccountDataStruct->pMarketDataApi->SubscribeMarketData(v_buff, 1, XTP_EXCHANGE_SZ);
		*/
		
		
		/*
		for (int i = 0; i < ticker_count; i++) {
			delete[] ppInstrumentID[i];
			ppInstrumentID[i] = NULL;
		}
		delete[] ppInstrumentID;
		ppInstrumentID = NULL;
		*/
		/**********23/4/6**********/
		//创建存放文件的根目录
		/*
		std::string rootDir = targetpath;
		createDir(rootDir);
		std::string exchange_id = EnumToString(ticker_info->exchange_id);
		
		std::string marketDir = rootDir + exchange_id;
		createDir(marketDir);
		// 创建各个文件的子目录
		std::string productDir = marketDir + "//" + ticker_info->ticker;
		
		
		if (XTP_TICKER_TYPE_OPTION == ticker_info->ticker_type&& ticker_info->exchange_id== XTP_EXCHANGE_SZ) {

			//SendNewMessage(ticker_info->ticker);
			createDir(productDir);
			
		}
		
		if (instrumentCsvPathMap.find(ticker_info->ticker) == instrumentCsvPathMap.end())
		{
			instrumentCsvPathMap.insert(make_pair(ticker_info->ticker, productDir));
		}*/
		/**********23/4/6**********/
	}

	//所有可用的合约代码分两次查询，先上证后深证，以深证查询完毕为结尾
	if (is_last && ticker_info->exchange_id == XTP_EXCHANGE_SH)
	{
		//is_last&& ticker_info->exchange_id == XTP_EXCHANGE_SZ;
		m_pMyAccountDataStruct->bIsMarketDataApiConnected = true;

		SendNewMessage("sh行情Api连接完成!");
	}
}

void MyXTPMarketDataSpi::OnQueryAllTickersFullInfo(XTPQFI* ticker_info, XTPRI* error_info, bool is_last) {

	string v_instrumentID = ticker_info->ticker;
	

	//初始化股票信息结构体
	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = new MyStockInfoDataStruct;

	v_pMyStockInfoDataStruct->instrumentName = ticker_info->ticker;
	v_pMyStockInfoDataStruct->exchangeID = ticker_info->exchange_id;
	//v_pMyStockInfoDataStruct->instrumentType = ticker_info->security_type;
	v_pMyStockInfoDataStruct->priceTick = ticker_info->price_tick;//变动价位

	m_pMyMarketDataStruct->map_allAvailableStockInfo[v_instrumentID] = v_pMyStockInfoDataStruct;
	m_pMyMarketDataStruct->map_allAvailableStockInfo[v_instrumentID]= ("90011638")

	m_pMyMarketDataStruct->vec_allAvailableInstrumentID.push_back("90011638");
	//m_pMyMarketDataStruct->vec_allAvailableInstrumentID.push_back("90011678");
	//m_pMyMarketDataStruct->vec_allAvailableInstrumentID.push_back("90011726");

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

	std::string rootDir = targetpath;
	createDir(rootDir);
	std::string exchange_id = EnumToString(ticker_info->exchange_id);

	std::string marketDir = rootDir + exchange_id;
	createDir(marketDir);
	// 创建各个文件的子目录
	std::string productDir = marketDir + "//" + ticker_info->ticker;


	if (255 == ticker_info->security_type && 30 == ticker_info->security_type && 29 == ticker_info->security_type && ticker_info->exchange_id == XTP_EXCHANGE_SZ) {

		//SendNewMessage(ticker_info->ticker);
		createDir(productDir);

	}

	if (instrumentCsvPathMap.find(ticker_info->ticker) == instrumentCsvPathMap.end())
	{
		instrumentCsvPathMap.insert(make_pair(ticker_info->ticker, productDir));
	}


	if (is_last && ticker_info->exchange_id == XTP_EXCHANGE_SZ)
	{
		//is_last&& ticker_info->exchange_id == XTP_EXCHANGE_SZ;
		m_pMyAccountDataStruct->bIsMarketDataApiConnected = true;

		SendNewMessage("sz行情Api连接完成!");
		
		int ticker_count = 3;//需要订阅行情的证券代码数量，可根据实际订阅需求改动
		char** ppInstrumentID = new char* [ticker_count];
		for (int i = 0; i < ticker_count; i++)
		{
			ppInstrumentID[i] = new char[XTP_TICKER_LEN];
		}
		strcpy_s(ppInstrumentID[0], XTP_TICKER_LEN, "000020");
		strcpy_s(ppInstrumentID[1], XTP_TICKER_LEN, "000010");
		strcpy_s(ppInstrumentID[2], XTP_TICKER_LEN, "000001");
	
		m_pMyAccountDataStruct->pMarketDataApi->SubscribeTickByTick(ppInstrumentID, ticker_count, XTP_EXCHANGE_SH);
		//m_pMyAccountDataStruct->pMarketDataApi->SubscribeMarketData(ppInstrumentID, ticker_count, XTP_EXCHANGE_SH);
		
	}
}


void MyXTPMarketDataSpi::OnSubTickByTick(XTPST* ticker, XTPRI* error_info, bool is_last) {
	if (!IsErrorRspInfo(error_info)) {

		string v_instrumentID = ticker->ticker;

		SendNewMessage("合约" + v_instrumentID + "行情订阅成功！");
	}
}
//订阅行情应答
	 void MyXTPMarketDataSpi::OnSubMarketData(XTPST * ticker, XTPRI * error_info, bool is_last) {
		 if (!IsErrorRspInfo(error_info)) {

			 string v_instrumentID = ticker->ticker;

			 SendNewMessage("合约" + v_instrumentID + "行情订阅成功！");
		 }
	 }	


void MyXTPMarketDataSpi::OnTickByTick(XTPTBT* tbt_data)
{

	std::string s = string_To_UTF8(tbt_data->state.flag);
	SendNewMessage("运行");
	//string s(tbt_data->state.flag);
	SendNewMessage(s);
	//cout << tbt_data->state.flag << endl;
}

//深度行情通知
void MyXTPMarketDataSpi::OnDepthMarketData(XTPMD* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) {

	string v_instrumentID = market_data->ticker;


	//更新行情数据
	MyBidAskDataStruct* v_pMyBidAskDataStruct = m_pMyMarketDataStruct->map_marketData[v_instrumentID];
	char v_buff[2048] = { 0 };
	std::string data_time = to_string(market_data->data_time);
	std::string time_yyyymmdd = data_time.substr(0, 8);
	const char* chartime = data_time.c_str();

	snprintf(v_buff, 2048, "行情数据更新：合约代码：%s 买一价:%f 卖一价：%f  更新时间：%s 历史数据： % f 当前交易状态：%s",
		market_data->ticker, market_data->bid[0], market_data->ask[0], chartime, market_data->last_price, market_data->ticker_status);

	SendNewMessage(v_buff);

	
	v_pMyBidAskDataStruct->mutex_bidAskData.lock();

	v_pMyBidAskDataStruct->updateTime = market_data->data_time;
	v_pMyBidAskDataStruct->preLastPrice = v_pMyBidAskDataStruct->currentLastPrice;  //历史数据
	v_pMyBidAskDataStruct->currentLastPrice = market_data->last_price;				//最新数据
	v_pMyBidAskDataStruct->bidPrice1 = market_data->bid[0];
	v_pMyBidAskDataStruct->askPrice1 = market_data->ask[0];
	v_pMyBidAskDataStruct->bidVolumel = market_data->bid_qty[0];
	v_pMyBidAskDataStruct->askVolumel = market_data->ask_qty[0];


	if (instrumentCsvPathMap.find(market_data->ticker) == instrumentCsvPathMap.end())
	{
		v_pMyBidAskDataStruct->mutex_bidAskData.unlock();
		return;
	}
	/*
	std::string csvDir = instrumentCsvPathMap.find(market_data->ticker)->second;
	std::string data_time = to_string(market_data->data_time);
	std::string time_yyyymmdd = data_time.substr(0, 8);
	const char* chartime = data_time.c_str();

	std::string csvPath = csvDir + "\\" + market_data->ticker + "_" + time_yyyymmdd + ".csv";
	if (_access(csvPath.c_str(), 0) == -1) //判断该路径下文件是否存在  存在返回0 不存在返回-1
	{
		ofstream outfile;
		outfile.open(csvPath, ios::app);
		outfile << "交易所代码," << "合约代码," << "最新价," << "昨收盘," << "今开盘," << "最高价," << "最低价," << "今收盘," << "涨停价," << "跌停价," << "时间," << "数量," << "成交金额," << "十档申买价," << "十档申卖价," << "十档申买量," << "十档申卖量," << "成交笔数," << "当前交易状态说明," << endl;
		outfile.close();

	}
	ofstream outfile;
	outfile.open(csvPath, ios::app);
	outfile << market_data->exchange_id << "," << market_data->ticker << "," << market_data->last_price << "," << market_data->pre_close_price << "," << market_data->open_price << "," <<
		market_data->high_price << "," << market_data->low_price << "," << market_data->close_price << "," << market_data->upper_limit_price << "," << market_data->lower_limit_price << "," << market_data->data_time << "," <<
		market_data->qty << "," << market_data->turnover << "," << market_data->bid << "," << market_data->ask << "," <<
		market_data->bid_qty << "," << market_data->ask_qty << "," << market_data->trades_count << "," << market_data->ticker_status << "\n";
	outfile.close();

	*/
	v_pMyBidAskDataStruct->mutex_bidAskData.unlock();
	
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


