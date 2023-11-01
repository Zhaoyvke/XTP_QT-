#include"MyXtpApi.h"
#include<Windows.h>

using namespace std;

//////////////////////////////////MyXTPTradeSPi////////////////////////////
///������Ϣ��װ����
void MyXTPTradeSpi::SendNewMessage(string message) {

	m_pMyInfoTaskQueueStruct->mutex_message.lock();
	m_pMyInfoTaskQueueStruct->queue_message.push(message);
	m_pMyInfoTaskQueueStruct->mutex_message.unlock();
}

//���XTP���ص���Ϣ���Ƿ��д�
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

//�ɽ��ر�
void MyXTPTradeSpi::OnTradeEvent(XTPTradeReport* trade_info, uint64_t session_id) {
	char v_buff[1024] = { 0 };
	snprintf(v_buff, 1024, "�ɽ��ر�����Լ���룺%s ���ţ�%i �ɽ�����%i  �ɽ���:%f",trade_info->ticker,trade_info->order_xtp_id,trade_info->quantity,trade_info->price);
	SendNewMessage(v_buff);
}

//�µ��ر�����ǰ��ע����������

void MyXTPTradeSpi::OnOrderEvent(XTPOrderInfo* order_info, XTPRI* error_info, uint64_t session_id) {

	if (!IsErrorRspInfo(error_info)) {

		//����
		if (order_info->order_status == XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING || order_info->order_status == XTP_ORDER_STATUS_CANCELED) {

			char v_buff[1024] = { 0 };
			snprintf(v_buff, 1024, "�����ر�����Լ���룺%s ����:%i ������:%i", order_info->ticker, order_info->order_xtp_id, order_info->qty_left);
			SendNewMessage(v_buff);
		}
	}
	//����
	else {
		
		char v_buff[1024] = { 0 };
		snprintf(v_buff,1024, "���󵥻ر�������״̬��%c ��Լ����:%s  ���ţ�%i ������:%i", order_info->order_status, order_info->ticker, order_info->order_xtp_id, order_info->qty_left);
		SendNewMessage(v_buff);
	}
}


//������������Ϣ
void MyXTPTradeSpi::OnError(XTPRI* error_info) {
	if (error_info != NULL) {

		if (error_info->error_id != 0) {

			SendNewMessage(__FUNCTION__);
			SendNewMessage(error_info->error_msg);
		}
	}
}



///////////////////////////MyXTPMarketDataSpi////////////////////////////////////////
//������Ϣ��װ����

void MyXTPMarketDataSpi::SendNewMessage(string message) {

	m_pMyInfoTaskQueueStruct->mutex_message.lock();
	m_pMyInfoTaskQueueStruct->queue_message.push(message);
	m_pMyInfoTaskQueueStruct->mutex_message.unlock();
}


//����Ƿ��д�����Ϣ
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


//��ѯ���п��ú�Լ��Ϣ�ص�
void MyXTPMarketDataSpi::OnQueryAllTickers(XTPQSI* ticker_info, XTPRI* error_info, bool is_last) {
	if (!IsErrorRspInfo(error_info)) {

		string v_instrumentID = ticker_info->ticker;

		//��ʼ����Ʊ��Ϣ�ṹ��
		MyStockInfoDataStruct* v_pMyStockInfoDataStruct = new MyStockInfoDataStruct;

		v_pMyStockInfoDataStruct->instrumentName = ticker_info->ticker_name;
		v_pMyStockInfoDataStruct->exchangeID = ticker_info->exchange_id;
		v_pMyStockInfoDataStruct->instrumentType = ticker_info->ticker_type;
		v_pMyStockInfoDataStruct->priceTick = ticker_info->price_tick;//�䶯��λ

		m_pMyMarketDataStruct->map_allAvailableStockInfo[v_instrumentID] = v_pMyStockInfoDataStruct;

		//��ʼ���������ݽṹ��
		MyBidAskDataStruct* v_pMyBidAskDataStruct = new MyBidAskDataStruct;
		v_pMyBidAskDataStruct->preLastPrice = 0;
		v_pMyBidAskDataStruct->currentLastPrice = 0;
		v_pMyBidAskDataStruct->bidPrice1 = 0;
		v_pMyBidAskDataStruct->askPrice1 = 0;
		v_pMyBidAskDataStruct->bidVolumel = 0;
		v_pMyBidAskDataStruct->askVolumel = 0;

		m_pMyMarketDataStruct->map_marketData[v_instrumentID] = v_pMyBidAskDataStruct;

		//��Ʊ�����б�
		m_pMyMarketDataStruct->vec_allAvailableInstrumentID.push_back(v_instrumentID);
	}

	//���п��õĺ�Լ��������β�ѯ������֤����֤������֤��ѯ���Ϊ��β
	if (is_last && ticker_info->exchange_id == XTP_EXCHANGE_SZ)
	{
		m_pMyAccountDataStruct->bIsMarketDataApiConnected = true;

		SendNewMessage("����Api�������!");
	}
}



//��������Ӧ��
void MyXTPMarketDataSpi::OnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last) {
	if (!IsErrorRspInfo(error_info)) {

		string v_instrumentID = ticker->ticker;

		SendNewMessage("��Լ" + v_instrumentID + "���鶩�ĳɹ���");
	}
}

//�������֪ͨ
void MyXTPMarketDataSpi::OnDepthMarketData(XTPMD* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) {

	string v_instrumentID = market_data->ticker;

	//������������
	MyBidAskDataStruct* v_pMyBidAskDataStruct = m_pMyMarketDataStruct->map_marketData[v_instrumentID];

	v_pMyBidAskDataStruct->mutex_bidAskData.lock();

	v_pMyBidAskDataStruct->updateTime = market_data->data_time;
	v_pMyBidAskDataStruct->preLastPrice = v_pMyBidAskDataStruct->currentLastPrice;  //��ʷ����
	v_pMyBidAskDataStruct->currentLastPrice = market_data->last_price;				//��������
	v_pMyBidAskDataStruct->bidPrice1 = market_data->bid[0];
	v_pMyBidAskDataStruct->askPrice1 = market_data->ask[0];
	v_pMyBidAskDataStruct->bidVolumel = market_data->bid_qty[0];
	v_pMyBidAskDataStruct->askVolumel = market_data->ask_qty[0];

	v_pMyBidAskDataStruct->mutex_bidAskData.unlock();

	char v_buff[1024] = { 0 };
	snprintf(v_buff, 1024, "�������ݸ��£���Լ���룺%s ��һ��:%f ��һ�ۣ�%f",
		market_data->ticker, market_data->bid[0], market_data->ask[0]);
	SendNewMessage(v_buff);

}


//���������ж�
void MyXTPMarketDataSpi::OnDisconnected(int reason) {

	SendNewMessage("����Api�����жϣ�");
}

//������������Ϣ
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


