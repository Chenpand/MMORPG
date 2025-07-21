#ifndef _RS_RECORD_H_
#define _RS_RECORD_H_

#include "CLDatabaseDefine.h"

#include <map>
#include <AvalonDBConnection.h>

/**
 *@brief 数据库记录
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
	 *@brief 缓存一个数据
	 *@param stream 网络流
	 */
	void PutData(int index, Avalon::NetInputStream& stream){
		m_Datas[index].Input(stream);
	}

	/**
	 *@brief 获取一列数据
	 */
	CLDatabaseData* FindData(int index){
		DataMap::iterator iter = m_Datas.find(index);
		return iter != m_Datas.end() ? &iter->second : NULL;
	}

	/**
	 *@brief 获取data数
	 */
	UInt32 GetDataNum() const{ return m_Datas.size(); }

	/**
	 *@brief 设置更新时间
	 */
	void SetUpdateTime(UInt32 time){ m_UpdateTime = time; }
	UInt32 GetUpdateTime() const { return m_UpdateTime; }

	/**
	 *@brief 获取迭代器
	 */
	DataIterator begin() const{ return m_Datas.begin(); }
	DataIterator end() const{ return m_Datas.end(); }

private:
	 //数据
	DataMap	m_Datas;
	//更新时间
	UInt32	m_UpdateTime;
};

#endif
