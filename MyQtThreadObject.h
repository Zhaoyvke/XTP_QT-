#pragma once
#include"MyDataStruct.h"
#include<qthread.h>

//����info��ص������߳� (һ��������ӡһЩ��Ϣ)
class MyProcessInfoTaskThread :public QThread {
	Q_OBJECT;

public:
	MyProcessInfoTaskThread(); 
	MyInfoTaskQueueStruct* m_pMyInfoTaskQueueStruct;

	void run();

private:

signals://�ź�
	void sendPrintMessages(QString);

};