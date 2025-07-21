#ifndef _CL_RECORD_CALLBACK_H_
#define _CL_RECORD_CALLBACK_H_

#include "CLDatabaseDefine.h"

/**
 *@brief record命令回调
 */
class CLRecordCallback
{
	typedef std::map<std::string, size_t> ColumnMap;
	typedef std::vector<UInt8>	TypeVec;
	typedef std::vector<std::pair<DBKey_t, CLDatabaseData* > >	RowVec;

public:
	CLRecordCallback();
	virtual ~CLRecordCallback();

public:
	/**
	 *@brief 完成
	 */
	virtual void OnFinished(){}

	/**
	 *@brief 失败
	 */
	virtual void OnFailed(UInt32 errorcode){}

	/**
	 *@brief 移动到下一行
	 *@return true 成功 false没有下一行
	 */
	bool NextRow();

	/**
	 *@brief 获取当前行主键
	 */
	DBKey_t GetKey() const;

	/**
	 *@brief 获取当前行一个数据
	 */
	const CLDatabaseData& GetData(const std::string& name) const;

public:
	/**
	 *@brief 执行
	 */
	void Execute();

	/**
	 *@brief 设置结果
	 */
	void SetResult(UInt32 result){ m_Result = result; }

	/**
	 *@brief 设置请求时间
	 */
	void SetRequestTime(UInt32 time){ m_RequestTime = time; }
	UInt32 GetRequestTime() const{ return m_RequestTime; }

	/**
	 *@brief 解码列信息
	 */
	bool DecodeColumns(Avalon::NetInputStream& stream);

	/**
	 *@brief 解码一行数据
	 */
	bool DecodeRow(Avalon::NetInputStream& stream);

private:
	//列索引
	ColumnMap	m_Columns;
	//列类型
	TypeVec		m_Types;
	//行数据
	RowVec		m_Rows;
	//当前行索引
	int		m_Index;

	//请求时间
	UInt32	m_RequestTime;
	//结果
	UInt32	m_Result;
};

#endif
