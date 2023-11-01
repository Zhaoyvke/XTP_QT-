#include"MyTableModel.h"

#include<qbrush.h>

MyTableModel::MyTableModel(QObject* parent) :QAbstractTableModel(parent) {

}

MyTableModel::~MyTableModel() {

}



void MyTableModel::GetData(MyStrategyTotalStruct *pMyStrategyTotalStruct) {

	m_pMyStrategyTotalStruct = pMyStrategyTotalStruct;

	m_vec_horizontalHeader = { QStringLiteral("���"),QStringLiteral("����״̬"),QStringLiteral("��Ʊ����"),QStringLiteral("��Ʊ����"),
	QStringLiteral("������ֵ��%��"),QStringLiteral("�µ���(��)"),QStringLiteral("ǰ��tick��") };
	
	m_rowCount = m_vec_strategySetting.size();
	m_colCount = m_vec_horizontalHeader.size();
}

//�ܹ�������
int MyTableModel::rowCount(const QModelIndex&)const {
	return m_rowCount;
}

//�ܹ�������
int MyTableModel::columnCount(const QModelIndex&)const {
	return m_colCount;
}

//��ͷ���ݸ�������
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

//���������ݸ��¹���
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

//��ȡ���ĸ�ʽ�Ĳ���״̬
QString MyTableModel::GetChineseStrategyStatus(bool bIsStratTrading)const {
	if (bIsStratTrading) {
		return QStringLiteral("��������");
	}
	else {
		return QStringLiteral("��ֹͣ");
	}
}

QString MyTableModel::CalculateData(const int& rowCount, const int& colCount)const {
	QString v_data;

	MyStrategyDataStruct* v_pRow = m_vec_strategySetting[rowCount];

	//���
	if (colCount == 0) {
		v_data = QString::number(v_pRow->id);
	}
	//����״̬
	else if (colCount == 1) {
		v_data = GetChineseStrategyStatus(v_pRow->bIsStartTrading);
	}
	//��Ʊ����
	else if (colCount == 2) {
		
		v_data = QString::fromStdString(v_pRow->instrumentID);

	}
	//��Ʊ����
	else if (colCount == 3) {
		v_data = QString::fromStdString(v_pRow->instrumentName);
	}
	//������ֵ
	else if (colCount == 4) {
		v_data = QString::number(v_pRow->triggerPercent);
	}
	//�µ���
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