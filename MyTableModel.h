#pragma once
#include<qabstracttablemodel>
#include"MyDataStruct.h"

using std::map;
using std::string;
using std::vector;

class MyTableModel :public QAbstractTableModel {
public:
	MyTableModel(QObject* parent);
	~MyTableModel();


	//�����ĸ��Ǽ̳�tablemodel�������Ҫʵ�ֵ�     ��tablemodel���б�Ĺ���
	int rowCount(const QModelIndex& parent = QModelIndex())const;
	int columnCount(const QModelIndex& parent = QModelIndex())const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)const;		//��Ӧ������θ���
	QVariant headerData(int section, Qt::Orientation orientation, int role)const;  //��ͷ����

	void GetData(MyStrategyTotalStruct *pMyStrategyTotalStruct);
	void UpdateData();
	

private:
	
	int m_rowCount, m_colCount;
	vector<QString> m_vec_horizontalHeader;  //��ͷ���б�

	MyStrategyTotalStruct *m_pMyStrategyTotalStruct;
	vector<MyStrategyDataStruct*>m_vec_strategySetting;

	QString GetChineseStrategyStatus(bool bIsStarTrading)const;    //����ת��
	QString CalculateData(const int& rowCount, const int& colCount)const;  //��������
	
};