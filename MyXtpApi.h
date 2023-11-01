#pragma once

#include"MyDataStruct.h"

//交易Api
class MyXTPTradeSpi:public XTP::API::TraderSpi {
public:
	string m_investorID;

	MyAccountDataStruct *m_pMyAccountDataStruct;
	MyInfoTaskQueueStruct* m_pMyInfoTaskQueueStruct;

	
	///成交通知
	///@param trade_info 成交回报的具体信息，用户可以通过trade_info.order_xtp_id来管理订单，通过GetClientIDByXTPID() == client_id来过滤自己的订单。
	/////对于上交所，exec_id可以唯一标识一笔成交。当发现2笔成交回报拥有相同的exec_id，则可以认为此笔交易自成交了。对于深交所，exec_id是唯一的，暂时无此判断机制。report_index+market字段可以组成唯一标识表示成交回报。
	///@param session_id 资金账户对应的session_id，登录时得到
	///@remark 订单有成交发生的时候，会被调用，需要快速返回，否则会堵塞后续消息，
	///当堵塞严重时，会触发断线。所有登录了此用户的客户端都将收到此用户的成交回报。相关订单为部成状态，需要用户通过成交回报的成交数量来确定，OnOrderEvent()不会推送部成状态。
	virtual void OnTradeEvent(XTPTradeReport* trade_info, uint64_t session_id) ;

	///报单通知
    ///@param order_info 订单响应具体信息，用户可以通过order_info.order_xtp_id来管理订单，
	//通过GetClientIDByXTPID() == client_id来过滤自己的订单，order_info.qty_left字段在订单为未成交、部成、全成、废单状态时，表示此订单还没有成交的数量，在部撤、全撤状态时，表示此订单被撤的数量。order_info.order_cancel_xtp_id为其所对应的撤单ID，不为0时表示此单被撤成功
	///@param error_info 订单被拒绝或者发生错误时错误代码和错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
	///@param session_id 资金账户对应的session_id，登录时得到
	///@remark 每次订单状态更新时，都会被调用，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线，
	//在订单未成交、全部成交、全部撤单、部分撤单、已拒绝这些状态时会有响应，对于部分成交的情况，请由订单的成交回报来自行确认。所有登录了此用户的客户端都将收到此用户的订单响应
	virtual void OnOrderEvent(XTPOrderInfo* order_info, XTPRI* error_info, uint64_t session_id);
	
	///当客户端的某个连接与交易后台通信连接断开时，该方法被调用。
	///@param reason 错误原因，请与错误代码表对应
	///@param session_id 资金账户对应的session_id，登录时得到
	///@remark 用户主动调用logout导致的断线，不会触发此函数。api不会自动重连，当断线发生时，请用户自行选择后续操作，可以在此函数中调用Login重新登录，并更新session_id，此时用户收到的数据跟断线之前是连续的
	virtual void OnDisconnected(uint64_t session_id, int reason) {};

	///错误应答
	///@param error_info 当服务器响应发生错误时的具体的错误代码和错误信息,当error_info为空，或者error_info.error_id为0时，表明没有错误
	///@remark 此函数只有在服务器发生错误时才会调用，一般无需用户处理
	virtual void OnError(XTPRI* error_info) ;



private:
	void SendNewMessage(string message);

	bool IsErrorRspInfo(XTPRI* error_info);
};

//行情Spi
class MyXTPMarketDataSpi :public XTP::API::QuoteSpi {
public:
	string m_investorID;

	MyAccountDataStruct *m_pMyAccountDataStruct;
	MyMarketDataStruct *m_pMyMarketDataStruct;
	MyInfoTaskQueueStruct *m_pMyInfoTaskQueueStruct;
	
	///查询合约部分静态信息的应答
	///@param ticker_info 合约部分静态信息
	///@param error_info 查询合约部分静态信息时发生错误时返回的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
	///@param is_last 是否此次查询合约部分静态信息的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
	virtual void OnQueryAllTickers(XTPQSI* ticker_info, XTPRI* error_info, bool is_last) ;

	virtual void OnQueryAllTickersFullInfo(XTPQFI* ticker_info, XTPRI* error_info, bool is_last);

	///订阅行情应答，包括股票、指数和期权
	///@param ticker 详细的合约订阅情况
	///@param error_info 订阅合约发生错误时的错误信息，当error_info为空，或者error_info.error_id为0时，表明没有错误
	///@param is_last 是否此次订阅的最后一个应答，当为最后一个的时候为true，如果为false，表示还有其他后续消息响应
	///@remark 每条订阅的合约均对应一条订阅应答，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
	virtual void OnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last) ;
	virtual void OnSubTickByTick(XTPST* ticker, XTPRI* error_info, bool is_last) ;

	///深度行情通知，包含买一卖一队列
	///@param market_data 行情数据
	///@param bid1_qty 买一队列数据
	///@param bid1_count 买一队列的有效委托笔数，即bid1_qty数组的长度，最大为50
	///@param max_bid1_count 买一队列总委托笔数
	///@param ask1_qty 卖一队列数据
	///@param ask1_count 卖一队列的有效委托笔数，即ask1_qty数组的长度，最大为50
	///@param max_ask1_count 卖一队列总委托笔数
	///@remark 需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
	virtual void OnDepthMarketData(XTPMD* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) ;


	///逐笔行情通知，包括股票、指数和期权
			///@param tbt_data 逐笔行情数据，包括逐笔委托和逐笔成交，此为共用结构体，需要根据type来区分是逐笔委托还是逐笔成交，需要快速返回，否则会堵塞后续消息，当堵塞严重时，会触发断线
	virtual void OnTickByTick(XTPTBT* tbt_data) ;

	///当客户端的某个连接与交易后台通信连接断开时，该方法被调用。
	///@param reason 错误原因，请与错误代码表对应
	///@remark 用户主动调用logout导致的断线，不会触发此函数。api不会自动重连，当断线发生时，请用户自行选择后续操作，可以在此函数中调用Login重新登录，并更新session_id，此时用户收到的数据跟断线之前是连续的
	virtual void OnDisconnected( int reason) ;
	///错误应答
	///@param error_info 当服务器响应发生错误时的具体的错误代码和错误信息,当error_info为空，或者error_info.error_id为0时，表明没有错误
	///@remark 此函数只有在服务器发生错误时才会调用，一般无需用户处理
	virtual void OnError(XTPRI* error_info) ;
private:
	bool createDir(std::string path);
	std::string string_To_UTF8(const std::string& str);
	const char* EnumToString(XTP_EXCHANGE_TYPE c);
	void SendNewMessage(string message);
	bool IsErrorRspInfo(XTPRI* error_info);
};