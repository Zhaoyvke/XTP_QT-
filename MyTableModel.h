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


	//以下四个是继承tablemodel的类必须要实现的     用tablemodel做列表的功能
	int rowCount(const QModelIndex& parent = QModelIndex())const;
	int columnCount(const QModelIndex& parent = QModelIndex())const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)const;		//对应数据如何更新
	QVariant headerData(int section, Qt::Orientation orientation, int role)const;  //表头部分

	void GetData(MyStrategyTotalStruct *pMyStrategyTotalStruct);
	void UpdateData();
	

private:
	
	int m_rowCount, m_colCount;
	vector<QString> m_vec_horizontalHeader;  //表头的列表

	MyStrategyTotalStruct *m_pMyStrategyTotalStruct;
	vector<MyStrategyDataStruct*>m_vec_strategySetting;

	QString GetChineseStrategyStatus(bool bIsStarTrading)const;    //中文转换
	QString CalculateData(const int& rowCount, const int& colCount)const;  //计算数据
	
};