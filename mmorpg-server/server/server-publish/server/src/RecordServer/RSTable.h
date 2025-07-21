#ifndef _RS_TABLE_H_
#define _RS_TABLE_H_

#include <map>
#include <sstream>
#include <AvalonRetryDBConnection.h>
#include <AvalonTime.h>
#include <AvalonNetConnection.h>
#include "RSRecord.h"
#include "DBThread.h"

#define RECORD_ID "guid"

/**
 *@brief 列信息
 */
class RSColumn
{
public:
	RSColumn():index(0),type(DBT_INVALID){}

	std::string	name;	//列名
	int			index;	//列索引
	int			type;	//列类型
};

/**
 *@brief 数据库表缓存
 */
class RSTable
{
public:
	typedef HashMap<std::string,RSColumn>	ColumnMap;
	typedef HashMap<DBKey_t,RSRecord>	RecordMap;

public:
	explicit RSTable();
	~RSTable();

public:
	/**
	 *@brief 初始化
	 */
	bool Init(const std::string& name, const std::string& uniqueNameCol);

	/**
	 *@brief 清空缓存
	 */
	void Final();

	/**
	*@brief 获取消息包Sequence
	*/
	UInt32 GenSequence();

public:
	/**
	 *@brief 获取表名
	 */
	const std::string& GetName() const{ return m_Name; }

	/**
	 *@brief 查找一列
	 */
	const RSColumn* FindColumn(const std::string& colname) const;
	const RSColumn* FindColumn(int index) const;

	/**
	 *@brief 获取唯一名列
	 */
	const RSColumn* GetUniqueNameColumn() const;

public:
	/**
	 *@brief 更新一行记录
	 */
	bool UpdateRecord(DBKey_t key, Avalon::NetInputStream& stream, bool bFlush);
	
	/**
	 *@brief 按条件更新记录
	 */
	bool UpdateRecord(Avalon::NetInputStream& stream);

	/**
	*@brief 更新一行记录
	*/
	bool NewUpdateRecord(UInt32 requestid, UInt32 connid, DBKey_t key, Avalon::NetInputStream& stream, bool bFlush);

	/**
	*@brief 按条件更新记录
	*/
	bool NewUpdateRecord(UInt32 requestid, UInt32 connid, Avalon::NetInputStream& stream);

	/**
	 *@brief 插入一行记录
	 */
	UInt32 InsertRecord(DBKey_t key, Avalon::NetInputStream& stream);
	UInt32 NewInsertRecord(UInt32 requestid, UInt32 connId, DBKey_t key, Avalon::NetInputStream& stream);
	static bool SaveGuidSeed(DBKey_t key);
	UInt32 InsertAutoGuidRecord(Avalon::NetInputStream& stream, UInt32 requestid, UInt32 connId);

	/**
	 *@brief 删除一行记录
	 */
	bool DeleteRecord(DBKey_t key);

	/**
	 *@brief 按条件删除记录
	 */
	bool DeleteRecord(const std::string& cond);

	/**
	 *@brief 查询记录并发送结果
	 */
	void SelectRecord(UInt32 srcZoneId, UInt32 srcNodeId, UInt32 requestid, DBKey_t key, Avalon::NetInputStream& stream, Avalon::NetConnectionPtr conn);

	/**
	*@brief 查询记录并发送结果
	*/
	void SelectRecordNew(UInt32 requestid, DBKey_t key, Avalon::NetInputStream& stream, Avalon::NetConnectionPtr conn);

	/**
	 *@brief 改名
	 */
	UInt32 ChangeName(DBKey_t key, const std::string& name, UInt8 isInsert);

	/**
	 *@brief tick
	 */
	bool OnTick(const Avalon::Time& now, UInt32 cachetime, bool bLog);

	/**
	 *@brief 回写所有数据
	 */
	void Flush();

private:
	/**
	 *@brief 直接插入一行到数据库
	 */
	bool UpdateRecordDirect(DBKey_t key, Avalon::NetInputStream& stream);

	/**
	*@brief 直接插入一行到数据库
	*/
	bool NewUpdateRecordDirect(UInt32 requestid, UInt32 connid, DBKey_t key, Avalon::NetInputStream& stream);

	/**
	 *@brief 数据转化为sql
	 */
	void ConvertToSql(Avalon::RetryDBConnection* conn, std::ostringstream& stream, const CLDatabaseData& data);

	/**
	 *@brief 从sql结果中获取列类型
	 */
	DBDataType GetColumnType(const std::string& typestr);

	/**
	 *@brief flush record
	 */
	bool FlushRecord(DBKey_t key, RSRecord& record);

	/**
	 *@brief batch flush record
	 */
	bool BatchFlushRecord(const std::vector<std::pair<DBKey_t, RSRecord*>>& records);

	/**
	 *@brief 批量SQL
	 */
	bool BatchFlushSql(const std::ostringstream& sql, const std::ostringstream& whereSql, const std::map<std::string, std::string>& columnSqls);

	/**
	*@brief 发送消息
	*/
	void SendPacket(UInt32 requestid, Avalon::NetConnectionPtr conn, Avalon::Packet* packet, UInt32 srcZoneId, UInt32 srcNodeId);

private:
	//表名
	std::string	m_Name;

	//列列表
	ColumnMap	m_Columns;
	//唯一名列索引
	int			m_NameIndex;
	//列数组,以index为索引
	std::vector<RSColumn>	m_ColumnVec;
	//表记录
	RecordMap	m_Records;
	//插入次数
	UInt32		m_InsertNum;
	//插入总时间
	UInt64		m_InsertTime;
	//更新次数
	UInt32		m_UpdateNum;
	//更新总时间
	UInt64		m_UpdateTime;
	//删除次数
	UInt32		m_DeleteNum;
	//删除总时间
	UInt64		m_DeleteTime;
	//查询次数
	UInt32		m_SelectNum;
	//查询总时间
	UInt64		m_SelectTime;
	//数据库连接线程
	DBThread*	m_DBThread;
};

#endif
