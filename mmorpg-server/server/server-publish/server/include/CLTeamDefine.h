#ifndef _TEAM_DEFINE_H_
#define _TEAM_DEFINE_H_

#include "CLDefine.h"
#include "CLGameDefine.h"

//最大队伍人数
#define MAX_TEAM_MEMBER_NUM 5

/**
 *@brief 队伍分配模式
 */
enum TeamDistributeMode
{
	TEAM_DISTRIBUTE_FREE = 0,		//自由分配模式
	TEAM_DISTRIBUTE_TAKETURNS = 1,	//轮流分配模式
	TEAM_DISTRIBUTE_RANDOM = 2,		//随机
};

/**
*@brief 队员属性
*/
enum TeamMemberProperty
{
	// 等级
	TEAM_MEMBER_PROPERTY_LEVEL,
	// 公会ID
	TEAM_MEMBER_PROPERTY_GUIDID,
	// 剩余次数
	TEAM_MEMBER_PROPERTY_REMAIN_TIMES,
	// 职业
	TEAM_MEMBER_PROPERTY_OCCU,
	// 状态
	TEAM_MEMBER_PROPERTY_STATUS_MASK,
	// vip等级
	TEAM_MEMBER_PROPERTY_VIP_LEVEL,
	// 抗磨值
	TEAM_MEMBER_PROPERTY_RESIST_MAGIC,
};

// 成员状态掩码
enum TeamMemberStatusMask
{
    // 是否在线
    TEAM_MEMBER_MASK_ONLINE     = 1 << 0,
    // 准备
    TEAM_MEMBER_MASK_READY      = 1 << 1,
	// 助战
	TEAM_MEMBER_MASK_ASSIST		= 1 << 2,
	// 是否在战斗中
	TEAM_MEMBER_MASK_RACING		= 1 << 3,
};

/**
*@brief 队伍成员信息
*/
struct TeammemberBaseInfo
{
	TeammemberBaseInfo() :id(0), level(0), occu(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & playerLabelInfo;
	}

	//id
	ObjID_t	id;
	//名字
	std::string name;
	//等级
	UInt16	level;
	//职业
	UInt8	occu;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};

typedef TeammemberBaseInfo TeamRequester;

/**
 *@brief 队伍成员信息
 */
struct TeammemberInfo : TeammemberBaseInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & level & occu & statusMask & avatar & remainTimes & guildId & vipLevel & resistMagic & playerLabelInfo;
	}

	// 状态掩码（对应枚举TeamMemberStatusMask）
    UInt8			statusMask;
	// 外观信息
	PlayerAvatar	avatar;
	// 剩余次数
	UInt32			remainTimes;
	// 公会ID
	ObjID_t			guildId;
	// vip等级
	UInt8			vipLevel;
	// 抗魔值
	UInt32			resistMagic;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};

struct TeamBaseInfo
{
    AVALON_DEFINE_NETSERIALIZATION()
    {
		stream & teamId & target & masterInfo & memberNum & maxMemberNum;
    }

    // 队伍编号
    UInt32					teamId;
    // 队伍目标
	UInt32					target;
    // 队长
    TeammemberBaseInfo		masterInfo;
    // 成员数量
    UInt8					memberNum;
    // 最多数量
    UInt8					maxMemberNum;
};

struct SceneTeamMember
{
    SceneTeamMember() : roleId(0), statusMask(0) {}

    AVALON_DEFINE_NETSERIALIZATION()
    {
        stream & roleId & statusMask;
    }

    bool isReady() const
    {
        return (statusMask & TEAM_MEMBER_MASK_READY) != 0;
    }

    ObjID_t roleId;
    UInt8   statusMask;
};

// 队伍选项修改类型
enum TeamOptionOperType
{
    // 目标
    TEAM_OPTION_TARGET,
	// 自动同意
	TEAM_OPTION_AUTO_AGREE,
	// 深渊自动结束
	TEAM_OPTION_HELL_AUTO_CLOSE,
};

// 组队选项
enum TeamOption
{
	// 开启深渊自动结束功能
	TO_HELL_AUTO_CLOSE = 1 << 0,

	// 队伍选项默认值
	TO_INIT = 0
};

#endif
