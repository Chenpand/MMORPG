#ifndef _CL_GLOBAL_ACTIVITY_SERVER_DEFINE_H_
#define _CL_GLOBAL_ACTIVITY_SERVER_DEFINE_H_

#include <AvalonStringUtil.h>
#include "CLDefine.h"
#include "CLActivityDefine.h"


/**
*@brief 玩家转发信息
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

	//玩家id
	ObjID_t roleId;
	//玩家账号
	UInt32  accId;
	//玩家名字
	std::string name;
	//玩家平台
	std::string platform;
	//玩家所在服区id
	UInt32 serverZoneId;
	//玩家所在服名字
	std::string serverName;
	//源节点
	UInt32 sourceNode[SERVER_TYPE_MAX];
};

/**
*@brief 运营活动配置数据
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

	// 拷贝可变数据
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

	// 活动id
	UInt32  id;
	// 类别(OpActivityType)
	UInt32	type;
	// 活动名
	std::string name;
	// 准备时间
	UInt32  prepareTime;
	// 开始时间
	UInt32	startTime;
	// 结束时间
	UInt32	endTime;
	// 开启等级限制(玩家等级)
	UInt16  playerLevelLimit;
	// 描述
	std::string description;
	// 规则描述
	std::string ruleDesc;
	// 扩展参数
	UInt32	parm;
	// 扩展参数2
	std::vector<UInt32>	parm2;

	// 状态
	UInt8	status;
	// 数据来源(OpActivityDataSource)
	UInt8  dataSource;
	//任务信息
	std::vector<OpActTaskData>	tasks;
	// 任务描述
	std::string	taskDesc;
};

// 运营活动
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