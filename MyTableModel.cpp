#include"MyTableModel.h"

#include<qbrush.h>

MyTableModel::MyTableModel(QObject* parent) :QAbstractTableModel(parent) {

}

MyTableModel::~MyTableModel() {

}



void MyTableModel::GetData(MyStrategyTotalStruct *pMyStrategyTotalStruct) {

	m_pMyStrategyTotalStruct = pMyStrategyTotalStruct;

	m_vec_horizontalHeader = { QStringLiteral("编号"),QStringLiteral("交易状态"),QStringLiteral("股票代码"),QStringLiteral("股票名称"),
	QStringLiteral("触发阈值（%）"),QStringLiteral("下单量(手)"),QStringLiteral("前进tick数") };
	
	m_rowCount = m_vec_strategySetting.size();
	m_colCount = m_vec_horizontalHeader.size();
}

//总共的行数
int MyTableModel::rowCount(const QModelIndex&)const {
	return m_rowCount;
}

//总共的列数
int MyTableModel::columnCount(const QModelIndex&)const {
	return m_colCount;
}

//表头数据改正规则
QVariant MyTableModel::headerData(int section, Qt::Orientation orientation, int role)const {
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Vertical)
		{
			return "";
		}
		else if (orientation == Qt::Horizontal) {
			return m_vec_horizontalHeader[section];
		}
	}
	return QVariant();
}

//表内容数据更新规则
QVariant MyTableModel::data(const QModelIndex& index, int role)const {
	if (!index.isValid()) {
		return QVariant();
	}
	if (role == Qt::DisplayRole) {
		return CalculateData(index.row(), index.column());
	}
	if (role == Qt::TextAlignmentRole) {
		return Qt::AlignCenter;
	}
	return QVariant();
}

//获取中文格式的策略状态
QString MyTableModel::GetChineseStrategyStatus(bool bIsStratTrading)const {
	if (bIsStratTrading) {
		return QStringLiteral("正常交易");
	}
	else {
		return QStringLiteral("已停止");
	}
}

QString MyTableModel::CalculateData(const int& rowCount, const int& colCount)const {
	QString v_data;

	MyStrategyDataStruct* v_pRow = m_vec_strategySetting[rowCount];

	//编号
	if (colCount == 0) {
		v_data = QString::number(v_pRow->id);
	}
	//交易状态
	else if (colCount == 1) {
		v_data = GetChineseStrategyStatus(v_pRow->bIsStartTrading);
	}
	//股票代码
	else if (colCount == 2) {
		
		v_data = QString::fromStdString(v_pRow->instrumentID);

	}
	//股票名称
	else if (colCount == 3) {
		v_data = QString::fromStdString(v_pRow->instrumentName);
	}
	//触发阈值
	else if (colCount == 4) {
		v_data = QString::number(v_pRow->triggerPercent);
	}
	//下单量
	else if (colCount == 5) {
		v_data = QString::number(v_pRow->forwardTickNum);
	}

	return v_data;
}

void MyTableModel::UpdateData() {

	m_vec_strategySetting.clear();

	m_pMyStrategyTotalStruct->mutex_strategySetting.lock();

	for (MyStrategyDataStruct* v_pMyStrategyDataStruct : m_pMyStrategyTotalStruct->vec_strategySetting) {
		m_vec_strategySetting.push_back(v_pMyStrategyDataStruct);
	}
	m_pMyStrategyTotalStruct->mutex_strategySetting.unlock();
	m_rowCount = m_vec_strategySetting.size();

	beginResetModel();

	endResetModel();
}