#ifndef _CL_GLOBAL_ACTIVITY_SERVER_DEFINE_H_
#define _CL_GLOBAL_ACTIVITY_SERVER_DEFINE_H_

#include <AvalonStringUtil.h>
#include "CLDefine.h"
#include "CLActivityDefine.h"


/**
*@brief ���ת����Ϣ
*/
struct PlayerTranmitInfo
{
	PlayerTranmitInfo() 
	{
		roleId = 0;
		accId = 0;
		serverZoneId = 0;
		memset(sourceNode, 0, sizeof(sourceNode));
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & accId & name & platform & serverZoneId & serverName;
		stream.SeriaArray(sourceNode, SERVER_TYPE_MAX);
	}

	//���id
	ObjID_t roleId;
	//����˺�
	UInt32  accId;
	//�������
	std::string name;
	//���ƽ̨
	std::string platform;
	//������ڷ���id
	UInt32 serverZoneId;
	//������ڷ�����
	std::string serverName;
	//Դ�ڵ�
	UInt32 sourceNode[SERVER_TYPE_MAX];
};

/**
*@brief ��Ӫ���������
*/
struct OpActivityInfo
{
	AVALON_DEFINE_OBJECTPOOL(OpActivityInfo, Avalon::NullMutex)

	OpActivityInfo()
	{
		id = 0;
		type = OAT_NONE;
		prepareTime = 0;
		startTime = 0;
		endTime = 0;
		playerLevelLimit = 0;
		parm = 0;

		status = AS_END;
		dataSource = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & type & name & prepareTime & startTime & endTime & playerLevelLimit & description & ruleDesc 
			& parm & parm2 & status & dataSource & tasks & taskDesc;
	}

	bool operator!=(const OpActivityInfo* rhs)
	{
		if (!rhs) return false;

		if (name != rhs->name)
			return true;

		if (prepareTime != rhs->prepareTime)
			return true;

		if (startTime != rhs->startTime)
			return true;

		if (endTime != rhs->endTime)
			return true;

		if (playerLevelLimit != rhs->playerLevelLimit)
			return true;

		if (description != rhs->description)
			return true;

		if (ruleDesc != rhs->ruleDesc)
			return true;

		return false;
	}

	// �����ɱ�����
	void CopyChangeableData(const OpActivityInfo* rhs)
	{
		if (!rhs) return;

		name = rhs->name;
		prepareTime = rhs->prepareTime;
		startTime = rhs->startTime;
		endTime = rhs->endTime;
		playerLevelLimit = rhs->playerLevelLimit;
		description = rhs->description;
		ruleDesc = rhs->ruleDesc;
	}

	// �id
	UInt32  id;
	// ���(OpActivityType)
	UInt32	type;
	// ���
	std::string name;
	// ׼��ʱ��
	UInt32  prepareTime;
	// ��ʼʱ��
	UInt32	startTime;
	// ����ʱ��
	UInt32	endTime;
	// �����ȼ�����(��ҵȼ�)
	UInt16  playerLevelLimit;
	// ����
	std::string description;
	// ��������
	std::string ruleDesc;
	// ��չ����
	UInt32	parm;
	// ��չ����2
	std::vector<UInt32>	parm2;

	// ״̬
	UInt8	status;
	// ������Դ(OpActivityDataSource)
	UInt8  dataSource;
	//������Ϣ
	std::vector<OpActTaskData>	tasks;
	// ��������
	std::string	taskDesc;
};

// ��Ӫ�
typedef std::vector<OpActivityInfo*> OpActivityInfoVec;
typedef std::vector<OpActivityInfo> OpActivityInfoObjectVec;

inline std::string EncodeGASPlayerUId(const std::string& pf, UInt32 serverZoneId, UInt32 accid, ObjID_t roleid)
{
	std::string uid;
	uid.append(pf).append("_")
		.append(Avalon::StringUtil::ConvertToString<UInt32>(serverZoneId)).append("_")
		.append(Avalon::StringUtil::ConvertToString<UInt32>(accid)).append("_")
		.append(Avalon::StringUtil::ConvertToString<ObjID_t>(roleid));
	return uid;
}

inline std::string DecodePlatformFromGASPlayerUId(const std::string& uid)
{
	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(uid, strVec, "_");
	if (strVec.size() != 4) return "";
	return strVec.front();
}

inline UInt32 DecodeServerZoneIdFromGASPlayerUId(const std::string& uid)
{
	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(uid, strVec, "_");
	if (strVec.size() != 4) return 0;
	return Avalon::StringUtil::ConvertToValue<UInt32>(strVec[1]);
}

inline UInt32 DecodeAccIdFromGASPlayerUId(const std::string& uid)
{
	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(uid, strVec, "_");
	if (strVec.size() != 4) return 0;
	return Avalon::StringUtil::ConvertToValue<UInt32>(strVec[2]);
}

inline ObjID_t DecodeRoleIdFromGASPlayerUId(const std::string& uid)
{
	std::vector<std::string> strVec;
	Avalon::StringUtil::Split(uid, strVec, "_");
	if (strVec.size() != 4) return 0;
	return Avalon::StringUtil::ConvertToValue<ObjID_t>(strVec[3]);
}

#endif