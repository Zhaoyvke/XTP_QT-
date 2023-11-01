#include"MyQtThreadObject.h"

using namespace std;
/////////////MyProcessInfoTaskThread/////////////




MyProcessInfoTaskThread::MyProcessInfoTaskThread() {


}
//重载Qthread的start函数，执行Api

void MyProcessInfoTaskThread::run() 
{

	while (true)
	{
		this_thread::sleep_for(chrono::milliseconds(1000));
		m_pMyInfoTaskQueueStruct->mutex_message.lock();

		if (!m_pMyInfoTaskQueueStruct->queue_message.empty()) {
			string v_message = m_pMyInfoTaskQueueStruct->queue_message.front(); 
			m_pMyInfoTaskQueueStruct->queue_message.pop(); 
		
			 QString q_message = QString::fromStdString(v_message);
			//emit sendPrintMessages(QString::fromStdString(q_message));
			 q_message = QString(QString::fromLocal8Bit(v_message.c_str()));
			emit sendPrintMessages(q_message);
			
		}
		m_pMyInfoTaskQueueStruct->mutex_message.unlock();
		//return;
	}
}