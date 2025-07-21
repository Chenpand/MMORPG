#ifndef _RS_RECORD_H_
#define _RS_RECORD_H_

#include "CLDatabaseDefine.h"

#include <map>
#include <AvalonDBConnection.h>

/**
 *@brief ���ݿ��¼
 */
class RSRecord
{ 
public:
	typedef std::map<int, CLDatabaseData> DataMap;
	typedef DataMap::const_iterator DataIterator;

public:
	explicit RSRecord():m_UpdateTime(0){}
	~RSRecord(){}

	/**
	 *@brief ����һ������
	 *@param stream ������
	 */
	void PutData(int index, Avalon::NetInputStream& stream){
		m_Datas[index].Input(stream);
	}

	/**
	 *@brief ��ȡһ������
	 */
	CLDatabaseData* FindData(int index){
		DataMap::iterator iter = m_Datas.find(index);
		return iter != m_Datas.end() ? &iter->second : NULL;
	}

	/**
	 *@brief ��ȡdata��
	 */
	UInt32 GetDataNum() const{ return m_Datas.size(); }

	/**
	 *@brief ���ø���ʱ��
	 */
	void SetUpdateTime(UInt32 time){ m_UpdateTime = time; }
	UInt32 GetUpdateTime() const { return m_UpdateTime; }

	/**
	 *@brief ��ȡ������
	 */
	DataIterator begin() const{ return m_Datas.begin(); }
	DataIterator end() const{ return m_Datas.end(); }

private:
	 //����
	DataMap	m_Datas;
	//����ʱ��
	UInt32	m_UpdateTime;
};

#endif
