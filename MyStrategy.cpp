#include"MyStrategy.h"
#include"MyDataStruct.h"
#include<algorithm>
#include<qdatastream.h>
#include"MyXtpApi.h"
#include<Windows.h>
#include <iostream>


//���������̵߳ĺ���
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

//������Ϣ�ķ�װ����

void MyStrategy::SendNewMessage(string message) {

	
	m_pMyInfoTaskQueueStruct->mutex_message.lock();
	m_pMyInfoTaskQueueStruct->queue_message.push(message);
	m_pMyInfoTaskQueueStruct->mutex_message.unlock();
}

//��ȡ�г�����
XTP_MARKET_TYPE MyStrategy::GetXtpMarketType(XTP_EXCHANGE_TYPE exchangeID) {

	if (exchangeID == XTP_EXCHANGE_SH) {

		return XTP_MKT_SH_A;
	}
	return XTP_MKT_SZ_A;
}


//�����߳�
void MyStrategy::CreateTradeThread(MyStrategyDataStruct* pMyStrategyDataStruct, MyBidAskDataStruct* pMyBidAskDataStruct) {

	SendNewMessage("��Ʊ����" + pMyStrategyDataStruct->instrumentID + "�������ף�");

	//�л�״̬
	pMyStrategyDataStruct->bIsStartTrading = false;//(��֤ͬһ����Ʊ������������)

	//��ȡ��Լ��Ϣ
	
	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[pMyStrategyDataStruct->instrumentID];
	//���ּ�ǰ��N��tick�µ�
	double v_priceToOrder = pMyBidAskDataStruct->askPrice1 + pMyStrategyDataStruct->forwardTickNum *v_pMyStockInfoDataStruct->priceTick;

	//return;

	int v_orderRef = InsertOrder(pMyStrategyDataStruct->instrumentID, pMyStrategyDataStruct->orderVolume,
		v_priceToOrder, Buy_MyTradeDirectionType);

	//�µ�ʧ��
	if (v_orderRef == 0) {
		return;
	}
	//��ʱ����
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	CancelOrder(v_orderRef, pMyStrategyDataStruct->instrumentID);
}



//�µ�����
int MyStrategy::InsertOrder(string instrumentID, int volume, double price, MyTradeDirectionType tradeDirection) {

	//��ȡ��Լ��Ϣ
	MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[instrumentID];

	//////////////////////////�����ı�������///////////////////////////////////
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
	//�µ������ص��ţ�������ֵΪ0��˵���µ�ʧ��
	uint64_t v_orderRef = m_pMyAccountDataStruct->pTradeApi->InsertOrder(&v_req, m_pMyAccountDataStruct->sessionID);

	if (v_orderRef == 0) {

		SendNewMessage("��Լ" + instrumentID + "�µ�ʧ��");
	}
	else {

		SendNewMessage("��Լ" + instrumentID + "�µ��ɹ���");
	}
	return v_orderRef;
}

//��������
void MyStrategy::CancelOrder(int orderRef, string instrumentID) {

	uint64_t v_err = m_pMyAccountDataStruct->pTradeApi->CancelOrder(orderRef, m_pMyAccountDataStruct->sessionID);	

	if (v_err == 0) {
		SendNewMessage("��Լ" + instrumentID + "����������ʧ�ܣ�");
	}
	else {
		SendNewMessage("��Լ" + instrumentID + "���������ͳɹ���");
	}
}



//�����׺���
void MyStrategy::StarWatching()
{
	SendNewMessage("���Խ����߳������ɹ���");
	
	
		while (true) 
		{

			if (!m_pMyAccountDataStruct->bIsTradeApiConnected || !m_pMyAccountDataStruct->bIsMarketDataApiConnected) 
			{
				continue;
			}
			
			//��ʱ1������
			std::this_thread::sleep_for(std::chrono::seconds(1));

			//���α������й�Ʊ��
			m_pMyStrategyTotalStruct->mutex_strategySetting.lock();
			if (m_pMyStrategyTotalStruct->vec_strategySetting.empty()) {
				//return ;
			}

			for (MyStrategyDataStruct* v_pMyStrategyDataStruct : m_pMyStrategyTotalStruct->vec_strategySetting) 
			{
				
				if (!v_pMyStrategyDataStruct->bIsStartTrading) {
					continue;
				}

				//��ȡ����
				MyBidAskDataStruct* v_pMyBidAskDataStruct = m_pMyMarketDataStruct->map_marketData[v_pMyStrategyDataStruct->instrumentID];
				//��ǰ�۸�=0��˵�����ܳ����쳣���   
				if (v_pMyBidAskDataStruct->currentLastPrice == 0|| v_pMyBidAskDataStruct->preLastPrice == 0) {
					continue;
				}
				
				if (v_pMyBidAskDataStruct == NULL) {
					continue;
				}

				//�ж��Ƿ񴥷�
				if (v_pMyBidAskDataStruct->currentLastPrice / v_pMyBidAskDataStruct->preLastPrice - 1 < v_pMyStrategyDataStruct->triggerPercent / 100) {


					continue;
				}

				SendNewMessage("��Ʊ����" + v_pMyStrategyDataStruct->instrumentID + "�������ף�");
				//����������  ���������㡣
				v_pMyStrategyDataStruct->bIsStartTrading = false;

				//���������߳�
				std::thread v_thread(&MyStrategy::CreateTradeThread, this, v_pMyStrategyDataStruct, v_pMyBidAskDataStruct);
				v_thread.detach();

				//��ȡ��Ʊ��Ϣ
				/*
				MyStockInfoDataStruct* v_pMyStockInfoDataStruct = m_pMyMarketDataStruct->map_allAvailableStockInfo[v_pMyStrategyDataStruct->instrumentID];

				InsertOrder(m_pMyStrategyDataStruct->instrumentID, v_pMyStrategyuDataStruct->orderVolume,
					v_pMyBidAskDataStruct->askPricel + v_pMyStockInfoDataStruct->priceTick * v_pMyStrategy);*/

				
			}
			m_pMyStrategyTotalStruct->mutex_strategySetting.unlock();
		}
}
