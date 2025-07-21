#ifndef _CL_DB_LOGGER_H_
#define _CL_DB_LOGGER_H_

#include "CLDefine.h"

#include <AvalonObjectPool.h>
#include <AvalonStringUtil.h>
#include <map>
#include <string>

/**
 *@brief ���ݿ���־
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
	 *@brief ����һ����־
	 */
	static DBLogger* Create(const std::string& logName, UInt32 accid, ObjID_t playerid, const std::string& playername)
	{
		return new DBLogger(logName, accid, playerid, playername);
	}
	static void Send(DBLogger*& logger);

public:
	//��־��
	std::string	name;
	//�˺�id
	UInt32		accid;
	//��ɫid
	ObjID_t		playerid;
	//��ɫ��
	std::string	playername;
	//ʱ���
	UInt32		timestamp;
	//����
	std::map<std::string, std::string>	params;
};


#endif
