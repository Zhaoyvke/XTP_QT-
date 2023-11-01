#pragma once
#include"MyDataStruct.h"
#include<qthread.h>

//处理info相关的任务线程 (一般用来打印一些消息)
class MyProcessInfoTaskThread :public QThread {
	Q_OBJECT;

public:
	MyProcessInfoTaskThread(); 
	MyInfoTaskQueueStruct* m_pMyInfoTaskQueueStruct;

	void run();

private:

signals://信号
	void sendPrintMessages(QString);

};