#pragma once

#include"MyDataStruct.h"

//����Api
class MyXTPTradeSpi:public XTP::API::TraderSpi {
public:
	string m_investorID;

	MyAccountDataStruct *m_pMyAccountDataStruct;
	MyInfoTaskQueueStruct* m_pMyInfoTaskQueueStruct;

	
	///�ɽ�֪ͨ
	///@param trade_info �ɽ��ر��ľ�����Ϣ���û�����ͨ��trade_info.order_xtp_id����������ͨ��GetClientIDByXTPID() == client_id�������Լ��Ķ�����
	/////�����Ͻ�����exec_id����Ψһ��ʶһ�ʳɽ���������2�ʳɽ��ر�ӵ����ͬ��exec_id���������Ϊ�˱ʽ����Գɽ��ˡ����������exec_id��Ψһ�ģ���ʱ�޴��жϻ��ơ�report_index+market�ֶο������Ψһ��ʶ��ʾ�ɽ��ر���
	///@param session_id �ʽ��˻���Ӧ��session_id����¼ʱ�õ�
	///@remark �����гɽ�������ʱ�򣬻ᱻ���ã���Ҫ���ٷ��أ���������������Ϣ��
	///����������ʱ���ᴥ�����ߡ����е�¼�˴��û��Ŀͻ��˶����յ����û��ĳɽ��ر�����ض���Ϊ����״̬����Ҫ�û�ͨ���ɽ��ر��ĳɽ�������ȷ����OnOrderEvent()�������Ͳ���״̬��
	virtual void OnTradeEvent(XTPTradeReport* trade_info, uint64_t session_id) ;

	///����֪ͨ
    ///@param order_info ������Ӧ������Ϣ���û�����ͨ��order_info.order_xtp_id����������
	//ͨ��GetClientIDByXTPID() == client_id�������Լ��Ķ�����order_info.qty_left�ֶ��ڶ���Ϊδ�ɽ������ɡ�ȫ�ɡ��ϵ�״̬ʱ����ʾ�˶�����û�гɽ����������ڲ�����ȫ��״̬ʱ����ʾ�˶���������������order_info.order_cancel_xtp_idΪ������Ӧ�ĳ���ID����Ϊ0ʱ��ʾ�˵������ɹ�
	///@param error_info �������ܾ����߷�������ʱ�������ʹ�����Ϣ����error_infoΪ�գ�����error_info.error_idΪ0ʱ������û�д���
	///@param session_id �ʽ��˻���Ӧ��session_id����¼ʱ�õ�
	///@remark ÿ�ζ���״̬����ʱ�����ᱻ���ã���Ҫ���ٷ��أ���������������Ϣ������������ʱ���ᴥ�����ߣ�
	//�ڶ���δ�ɽ���ȫ���ɽ���ȫ�����������ֳ������Ѿܾ���Щ״̬ʱ������Ӧ�����ڲ��ֳɽ�����������ɶ����ĳɽ��ر�������ȷ�ϡ����е�¼�˴��û��Ŀͻ��˶����յ����û��Ķ�����Ӧ
	virtual void OnOrderEvent(XTPOrderInfo* order_info, XTPRI* error_info, uint64_t session_id);
	
	///���ͻ��˵�ĳ�������뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á�
	///@param reason ����ԭ��������������Ӧ
	///@param session_id �ʽ��˻���Ӧ��session_id����¼ʱ�õ�
	///@remark �û���������logout���µĶ��ߣ����ᴥ���˺�����api�����Զ������������߷���ʱ�����û�����ѡ����������������ڴ˺����е���Login���µ�¼��������session_id����ʱ�û��յ������ݸ�����֮ǰ��������
	virtual void OnDisconnected(uint64_t session_id, int reason) {};

	///����Ӧ��
	///@param error_info ����������Ӧ��������ʱ�ľ���Ĵ������ʹ�����Ϣ,��error_infoΪ�գ�����error_info.error_idΪ0ʱ������û�д���
	///@remark �˺���ֻ���ڷ�������������ʱ�Ż���ã�һ�������û�����
	virtual void OnError(XTPRI* error_info) ;



private:
	void SendNewMessage(string message);

	bool IsErrorRspInfo(XTPRI* error_info);
};

//����Spi
class MyXTPMarketDataSpi :public XTP::API::QuoteSpi {
public:
	string m_investorID;

	MyAccountDataStruct *m_pMyAccountDataStruct;
	MyMarketDataStruct *m_pMyMarketDataStruct;
	MyInfoTaskQueueStruct *m_pMyInfoTaskQueueStruct;
	
	///��ѯ��Լ���־�̬��Ϣ��Ӧ��
	///@param ticker_info ��Լ���־�̬��Ϣ
	///@param error_info ��ѯ��Լ���־�̬��Ϣʱ��������ʱ���صĴ�����Ϣ����error_infoΪ�գ�����error_info.error_idΪ0ʱ������û�д���
	///@param is_last �Ƿ�˴β�ѯ��Լ���־�̬��Ϣ�����һ��Ӧ�𣬵�Ϊ���һ����ʱ��Ϊtrue�����Ϊfalse����ʾ��������������Ϣ��Ӧ
	virtual void OnQueryAllTickers(XTPQSI* ticker_info, XTPRI* error_info, bool is_last) ;

	virtual void OnQueryAllTickersFullInfo(XTPQFI* ticker_info, XTPRI* error_info, bool is_last);

	///��������Ӧ�𣬰�����Ʊ��ָ������Ȩ
	///@param ticker ��ϸ�ĺ�Լ�������
	///@param error_info ���ĺ�Լ��������ʱ�Ĵ�����Ϣ����error_infoΪ�գ�����error_info.error_idΪ0ʱ������û�д���
	///@param is_last �Ƿ�˴ζ��ĵ����һ��Ӧ�𣬵�Ϊ���һ����ʱ��Ϊtrue�����Ϊfalse����ʾ��������������Ϣ��Ӧ
	///@remark ÿ�����ĵĺ�Լ����Ӧһ������Ӧ����Ҫ���ٷ��أ���������������Ϣ������������ʱ���ᴥ������
	virtual void OnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last) ;
	virtual void OnSubTickByTick(XTPST* ticker, XTPRI* error_info, bool is_last) ;

	///�������֪ͨ��������һ��һ����
	///@param market_data ��������
	///@param bid1_qty ��һ��������
	///@param bid1_count ��һ���е���Чί�б�������bid1_qty����ĳ��ȣ����Ϊ50
	///@param max_bid1_count ��һ������ί�б���
	///@param ask1_qty ��һ��������
	///@param ask1_count ��һ���е���Чί�б�������ask1_qty����ĳ��ȣ����Ϊ50
	///@param max_ask1_count ��һ������ί�б���
	///@remark ��Ҫ���ٷ��أ���������������Ϣ������������ʱ���ᴥ������
	virtual void OnDepthMarketData(XTPMD* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count) ;


	///�������֪ͨ��������Ʊ��ָ������Ȩ
			///@param tbt_data ����������ݣ��������ί�к���ʳɽ�����Ϊ���ýṹ�壬��Ҫ����type�����������ί�л�����ʳɽ�����Ҫ���ٷ��أ���������������Ϣ������������ʱ���ᴥ������
	virtual void OnTickByTick(XTPTBT* tbt_data) ;

	///���ͻ��˵�ĳ�������뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á�
	///@param reason ����ԭ��������������Ӧ
	///@remark �û���������logout���µĶ��ߣ����ᴥ���˺�����api�����Զ������������߷���ʱ�����û�����ѡ����������������ڴ˺����е���Login���µ�¼��������session_id����ʱ�û��յ������ݸ�����֮ǰ��������
	virtual void OnDisconnected( int reason) ;
	///����Ӧ��
	///@param error_info ����������Ӧ��������ʱ�ľ���Ĵ������ʹ�����Ϣ,��error_infoΪ�գ�����error_info.error_idΪ0ʱ������û�д���
	///@remark �˺���ֻ���ڷ�������������ʱ�Ż���ã�һ�������û�����
	virtual void OnError(XTPRI* error_info) ;
private:
	bool createDir(std::string path);
	std::string string_To_UTF8(const std::string& str);
	const char* EnumToString(XTP_EXCHANGE_TYPE c);
	void SendNewMessage(string message);
	bool IsErrorRspInfo(XTPRI* error_info);
};