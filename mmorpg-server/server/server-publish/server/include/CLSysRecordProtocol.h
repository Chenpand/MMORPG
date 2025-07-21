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

		// ����
		UInt32		param;
		// ������
		UInt32		totalNum;
		// ʣ������
		UInt32		remainNum;
	};

	/*
	*@brief scene->world sysrecord��������
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
		// ö��SysRecordEvent
		UInt8  sysRecordEvent;
		// ϡ�п�������
		UInt8  rareControlType;
		// ϡ�п��Ʋ���
		std::vector<UInt32>	params;
		// ����������
		std::vector<UInt32> extraParam;
	};

	/*
	*@brief world->scene sysrecord���ݷ���
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
		// ö��SysRecordEvent
		UInt8  sysRecordEvent;
		// ϡ�п��Ƽ�¼
		std::vector<ProtoSysRecord> records;
		// �������
		std::vector<UInt32> extraParam;
	};

	/*
	*@brief scene->world sysrecord����ͬ��
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
		// ö��RareControlType
		UInt8 type;
		// ����
		UInt32 param1;
		// �����ֵ
		int addNum;
	};
}

#endif

