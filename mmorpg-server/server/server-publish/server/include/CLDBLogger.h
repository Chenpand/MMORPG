#ifndef _CL_DB_LOGGER_H_
#define _CL_DB_LOGGER_H_

#include "CLDefine.h"

#include <AvalonObjectPool.h>
#include <AvalonStringUtil.h>
#include <map>
#include <string>

/**
 *@brief 数据库日志
 */
struct DBLogger
{
	AVALON_DEFINE_OBJECTPOOL(DBLogger, Avalon::Mutex)

private:
	DBLogger():accid(0), playerid(0), timestamp(0){}
	DBLogger(const std::string& _logname, UInt32 _accid, ObjID_t _playerid, const std::string _playername)
		:name(_logname), accid(_accid), playerid(_playerid), playername(_playername)
	{
		timestamp = UInt32(CURRENT_TIME.Sec());
	}

public:
	void PutParam(const std::string& key, const char* value)
	{
		if(value == NULL) return;
		params.insert(std::make_pair(key, std::string(value)));
	}

	void PutParam(const std::string& key, const std::string& value)
	{
		params.insert(std::make_pair(key, value));
	}

	template<typename ValueT>
	void PutParam(const std::string& key, ValueT value)
	{
		params.insert(std::make_pair(key, Avalon::StringUtil::ConvertToString<ValueT>(value)));
	}

public:
	/**
	 *@brief 创建一个日志
	 */
	static DBLogger* Create(const std::string& logName, UInt32 accid, ObjID_t playerid, const std::string& playername)
	{
		return new DBLogger(logName, accid, playerid, playername);
	}
	static void Send(DBLogger*& logger);

public:
	//日志名
	std::string	name;
	//账号id
	UInt32		accid;
	//角色id
	ObjID_t		playerid;
	//角色名
	std::string	playername;
	//时间戳
	UInt32		timestamp;
	//参数
	std::map<std::string, std::string>	params;
};


#endif
