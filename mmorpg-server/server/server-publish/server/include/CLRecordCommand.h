#ifndef _CL_RECORD_COMMAND_H_
#define _CL_RECORD_COMMAND_H_

#include <AvalonNetStream.h>
#include <AvalonObjectPool.h>
#include <AvalonMutex.h>
#include <vector>
#include "CLDatabaseDefine.h"
#include "CLProtocolDefine.h"

/**
 *@brief record命令基类
 */
class CLRecordCommand
{
public:
	CLRecordCommand(UInt32 _zoneId, const std::string& _tablename)
		:zoneId(_zoneId),tablename(_tablename){}
	virtual ~CLRecordCommand(){}

public:
	/**
	 *@brief 获取类型
	 */
	virtual UInt32 GetType() const = 0;

	/**
	 *@brief 获取表名
	 */
	const std::string& GetTableName() const{ return tablename; }

	/**
	 *@brief 获取区id                                                                     
	 */
	UInt32 GetZoneId() const{ return zoneId; }

	/**
	 *@brief 判断命令是否有效
	 */
	virtual bool IsValid() const{ return !tablename.empty(); }

	/**
	*@brief sql防注入
	*/
	bool SqlDefendInfusion(std::string& str);


public:

	/**
	 *@brief 编码这个命令
	 */
	virtual bool Encode(Avalon::NetOutputStream& stream);

private:
	//区id
	UInt32	zoneId;
	//数据库表名
	std::string tablename;
};

/**
 *@brief 保存命令
 */
class CLSaveCommand : public CLRecordCommand
{
public:
	typedef std::vector<std::pair<std::string,CLDatabaseData> >	DataMap;

public:
	CLSaveCommand(UInt32 _zoneId, const std::string& _tablename, DBKey_t _key)
		:CLRecordCommand(_zoneId, _tablename),key(_key){}
	virtual ~CLSaveCommand(){}

public:

#define CL_DEFINE_DBCMD_PUTDATA(Type) \
	void PutData(const std::string& name,Type val) \
	{ \
		datas.push_back(std::make_pair(name,CLDatabaseData(val))); \
	}

	CL_DEFINE_DBCMD_PUTDATA(Int8)
	CL_DEFINE_DBCMD_PUTDATA(UInt8)
	CL_DEFINE_DBCMD_PUTDATA(Int16)
	CL_DEFINE_DBCMD_PUTDATA(UInt16)
	CL_DEFINE_DBCMD_PUTDATA(Int32)
	CL_DEFINE_DBCMD_PUTDATA(UInt32)
	CL_DEFINE_DBCMD_PUTDATA(Int64)
	CL_DEFINE_DBCMD_PUTDATA(UInt64)
	CL_DEFINE_DBCMD_PUTDATA(Real32)
	CL_DEFINE_DBCMD_PUTDATA(Real64)
	
	void PutData(const std::string& name,const std::string& str)
	{
		datas.push_back(std::make_pair(name,CLDatabaseData(str)));
	}

	void PutData(const std::string& name,const char* ptr,size_t size)
	{
		datas.push_back(std::make_pair(name,CLDatabaseData(ptr,size)));
	}

	bool IsValid() const{ return CLRecordCommand::IsValid() && key != 0; }

	DBKey_t GetKey() const { return key; }

	UInt32 GetDataNum() const{ return datas.size(); }

public:

	bool Encode(Avalon::NetOutputStream& stream);

protected:

	//主键值
	DBKey_t		key;
	//数据
	DataMap datas;
};

/**
 *@brief 插入命令
 */
class CLInsertCommand : public CLSaveCommand
{
	AVALON_DEFINE_OBJECTPOOL(CLInsertCommand, Avalon::Mutex)

public:
	CLInsertCommand(UInt32 _zoneId, const std::string& _tablename, DBKey_t _key)
		:CLSaveCommand(_zoneId, _tablename, _key){}
	virtual ~CLInsertCommand(){}

public:
	UInt32 GetType() const{ return CLProtocol::RECORD_INSERT_REQ; }
};

/**
 *@brief 更新命令
 */
class  CLUpdateCommand : public CLSaveCommand
{
	AVALON_DEFINE_OBJECTPOOL(CLUpdateCommand, Avalon::Mutex)

public:
	CLUpdateCommand(UInt32 _zoneId, const std::string& _tablename, DBKey_t _key, bool flush)
		:CLSaveCommand(_zoneId, _tablename, _key), bflush(flush), bCond(false){}
	CLUpdateCommand(UInt32 _zoneId, const std::string& _tablename, const std::string& _cond)
		:CLSaveCommand(_zoneId, _tablename, 0), bflush(true), cond(_cond), bCond(true){}
	virtual ~CLUpdateCommand(){}

public:
	UInt32 GetType() const{ return CLProtocol::RECORD_UPDATE_REQ; }

	bool IsValid() const
	{
		if(!CLRecordCommand::IsValid()) return false;
		if(!bCond && key == 0) return false;
		return bflush || GetDataNum() > 0;
	}

public:
	/**
	 *@brief 将命令编码进流中
	 */
	bool Encode(Avalon::NetOutputStream& stream);

private:
	//是否回写数据
	bool bflush;
	//条件
	std::string cond;
	//是否条件更新
	bool bCond;
};

/**
 *@brief 删除命令
 */
class CLDeleteCommand : public CLRecordCommand
{
	AVALON_DEFINE_OBJECTPOOL(CLDeleteCommand, Avalon::Mutex)

public:
	CLDeleteCommand(UInt32 _zoneId, const std::string& _name, DBKey_t _key)
		:CLRecordCommand(_zoneId, _name),key(_key){}
	CLDeleteCommand(UInt32 _zoneId, const std::string& _name, const std::string& _cond)
		:CLRecordCommand(_zoneId, _name),key(0),cond(_cond){}
	virtual ~CLDeleteCommand(){}

public:
	UInt32 GetType() const{ return CLProtocol::RECORD_DELETE_REQ; }

public:
	bool Encode(Avalon::NetOutputStream& stream);

private:
	//主键
	DBKey_t key;
	//条件
	std::string	cond;
};

/**
 *@brief 查询命令
 */
class CLSelectCommand : public CLRecordCommand
{
	AVALON_DEFINE_OBJECTPOOL(CLSelectCommand, Avalon::Mutex)

	typedef std::vector<std::string> ColumnVec;
    struct SelectCond
    {
        std::string     condname;
        CLDatabaseData  data;
        std::string     op;
    };
	typedef std::vector<SelectCond >	CondMap;

public:
	explicit CLSelectCommand(UInt32 _zoneId, const std::string& _tablename)
		:CLRecordCommand(_zoneId, _tablename){}
	virtual ~CLSelectCommand(){}

public:
	UInt32 GetType() const{ return CLProtocol::RECORD_SELECT_REQ; }

public:

#define CL_DEFINE_DBCMD_PUTCOND(Type) \
	void PutCond(const std::string& name,Type val, const std::string op = "=") \
	{ \
        SelectCond cond;                \
        cond.condname = name;           \
        cond.data = CLDatabaseData(val);\
        cond.op = op;                   \
		conds.push_back(cond); \
	}

	CL_DEFINE_DBCMD_PUTCOND(Int8)
	CL_DEFINE_DBCMD_PUTCOND(UInt8)
	CL_DEFINE_DBCMD_PUTCOND(Int16)
	CL_DEFINE_DBCMD_PUTCOND(UInt16)
	CL_DEFINE_DBCMD_PUTCOND(Int32)
	CL_DEFINE_DBCMD_PUTCOND(UInt32)
	CL_DEFINE_DBCMD_PUTCOND(Int64)
	CL_DEFINE_DBCMD_PUTCOND(UInt64)
	CL_DEFINE_DBCMD_PUTCOND(Real32)
	CL_DEFINE_DBCMD_PUTCOND(Real64)

	void PutCond(const std::string& name,const std::string& str, const std::string op = "=")
	{
        SelectCond cond;
        cond.condname = name;
        cond.data = CLDatabaseData(str);
        cond.op = op;
		conds.push_back(cond);
	}

	void PutColumn(const std::string& name)
	{
		columns.push_back(name);
	}

	void SetLimit(const std::string& _limit)
	{
		limit = _limit;
	}

    void SetOrder(const std::string& _order)
    {
        order = _order;
    }

public:
	bool Encode(Avalon::NetOutputStream& stream);

private:
	//数据库表
	std::string	tablename;
	//查询的列
	ColumnVec	columns;
	//查询条件
	CondMap		conds;
    //排序
    std::string order;
	//其他限制
	std::string	limit;
};

/**
 *@brief 回写数据
 */
class CLFlushCommand : public CLRecordCommand
{
	AVALON_DEFINE_OBJECTPOOL(CLFlushCommand, Avalon::Mutex)

public:
	explicit CLFlushCommand(UInt32 _zoneId, const std::string& _tablename)
		:CLRecordCommand(_zoneId, _tablename){}
	virtual ~CLFlushCommand(){}

public:
	UInt32 GetType() const{ return CLProtocol::RECORD_FLUSH_REQ; }
};

/**
 *@brief 改名
 */
class CLChangeNameCommand : public CLRecordCommand
{
	AVALON_DEFINE_OBJECTPOOL(CLChangeNameCommand, Avalon::Mutex)

public:
	explicit CLChangeNameCommand(UInt32 _zoneId, const std::string& _tablename, DBKey_t _key, const std::string& _name, UInt8 _isInsert)
		:CLRecordCommand(_zoneId, _tablename),key(_key),name(_name),isInsert(_isInsert){}
	virtual ~CLChangeNameCommand(){}

public:
	bool IsValid() const
	{
		if(!CLRecordCommand::IsValid()) return false;
		if(key == 0 || name.empty()) return false;
		return true;
	}

	UInt32 GetType() const{ return CLProtocol::RECORD_CHANGENAME_REQ; } 

public:
	bool Encode(Avalon::NetOutputStream& stream);

public:
	DBKey_t		key;
	std::string	name;
	UInt8		isInsert;	// 是否插入唯一名字表
};

/**
*@brief 主键自增插入命令
*/
class CLAutoGuidInsertCommand : public CLRecordCommand
{
	AVALON_DEFINE_OBJECTPOOL(CLAutoGuidInsertCommand, Avalon::Mutex)

	typedef std::vector<std::pair<std::string, CLDatabaseData> >	DataMap;

public:
	explicit CLAutoGuidInsertCommand(UInt32 _zoneId, const std::string& _tablename)
		:CLRecordCommand(_zoneId, _tablename){}
	virtual ~CLAutoGuidInsertCommand(){}

public:
#define CL_DEFINE_DBCMD_PUTDATA2(Type) \
	void PutData(const std::string& name, Type val) \
{ \
	datas.push_back(std::make_pair(name, CLDatabaseData(val))); \
}

	CL_DEFINE_DBCMD_PUTDATA2(Int8)
		CL_DEFINE_DBCMD_PUTDATA2(UInt8)
		CL_DEFINE_DBCMD_PUTDATA2(Int16)
		CL_DEFINE_DBCMD_PUTDATA2(UInt16)
		CL_DEFINE_DBCMD_PUTDATA2(Int32)
		CL_DEFINE_DBCMD_PUTDATA2(UInt32)
		CL_DEFINE_DBCMD_PUTDATA2(Int64)
		CL_DEFINE_DBCMD_PUTDATA2(UInt64)
		CL_DEFINE_DBCMD_PUTDATA2(Real32)
		CL_DEFINE_DBCMD_PUTDATA2(Real64)

	void PutData(const std::string& name, const std::string& str)
	{
		datas.push_back(std::make_pair(name, CLDatabaseData(str)));
	}

	void PutData(const std::string& name, const char* ptr, size_t size)
	{
		datas.push_back(std::make_pair(name, CLDatabaseData(ptr, size)));
	}

	UInt32 GetDataNum() const{ return datas.size(); }

public:

	bool Encode(Avalon::NetOutputStream& stream);

	DataMap GetDataMap() { return datas; }

	UInt32 GetType() const{ return CLProtocol::RECORD_AUTO_GUID_INSERT_REQ; }

protected:

	//数据
	DataMap datas;
};

#endif
