#ifndef _CL_SYS_RECORD_PROTOCOL_H_
#define _CL_SYS_RECORD_PROTOCOL_H_

#include <AvalonProtocol.h>
#include "CLProtocolDefine.h"
#include "CLSysRecordDefine.h"

namespace CLProtocol
{
	struct ProtoSysRecord
	{
		ProtoSysRecord()
		{
			memset(this, 0, sizeof(*this));
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & param & totalNum & remainNum;
		}

		// 参数
		UInt32		param;
		// 总数量
		UInt32		totalNum;
		// 剩余数量
		UInt32		remainNum;
	};

	/*
	*@brief scene->world sysrecord数据请求
	*/
	class SceneSysRecordDataReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYS_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ownerId & sysRecordEvent & rareControlType & params & extraParam;
		}

		ObjID_t ownerId;
		// 枚举SysRecordEvent
		UInt8  sysRecordEvent;
		// 稀有控制类型
		UInt8  rareControlType;
		// 稀有控制参数
		std::vector<UInt32>	params;
		// 请求额外参数
		std::vector<UInt32> extraParam;
	};

	/*
	*@brief world->scene sysrecord数据返回
	*/
	class SceneSysRecordDataRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYS_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ownerId & sysRecordEvent & records & extraParam;
		}

		ObjID_t ownerId;
		// 枚举SysRecordEvent
		UInt8  sysRecordEvent;
		// 稀有控制记录
		std::vector<ProtoSysRecord> records;
		// 额外参数
		std::vector<UInt32> extraParam;
	};

	/*
	*@brief scene->world sysrecord数据同步
	*/
	class SceneSysRecordDataSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYS_RECORD_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ownerId & type & param1 & addNum;
		}

		ObjID_t ownerId;
		// 枚举RareControlType
		UInt8 type;
		// 参数
		UInt32 param1;
		// 添加数值
		int addNum;
	};
}

#endif

