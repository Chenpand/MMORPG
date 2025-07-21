#ifndef _CL_RELATION_DEFINE_H_
#define _CL_RELATION_DEFINE_H_

#include "CLDefine.h"
#include "CLItemDefine.h"
#include "CLRetinueDefine.h"
#include "CLPkStatisticDefine.h"
#include "CLPetDefine.h"

/**
 *@brief 关系类型
 */
enum RelationType
{
	RELATION_FRIEND =		1,	//好友关系
	RELATION_BLACKLIST =	2,	//黑名单
	RELATION_STRANGER = 3,		//陌生人
	RELATION_MASTER = 4,		//师傅
	RELATION_DISCIPLE = 5,		//徒弟

	RELATION_MAX,
};

/**
*@brief 推荐关系类型
*/
enum RelationFindType
{
	RFT_FRIEND = 1,
	RFT_TEAM = 2,
	RFT_MASTER = 3,
	RFT_DISCIPLE = 4,
	RFT_ROOM = 5,
};


/**
 *@brief 关系属性                                                                     
 */
enum RelationAttr
{
	RA_INVALID,
	RA_NAME,		//对方名字(std::string)
	RA_SEASONLV,			//对方段位(UInt32)
	RA_LEVEL,		//对方等级(UInt16)
	RA_OCCU,		//对方职业(UInt8)
	RA_LASTGIVETIME,		//上一次赠送时间(UInt32)
	RA_DAYGIFTNUM,	//每日赠送次数
	RA_TYPE,			//关系
	RA_CREATETIME,	//关系建立时间 (UInt32)
	RA_VIPLV,	//vip等级(UInt8)
	RA_STATUS,	//状态(UInt8)
	RA_MASTER_DAYGIFTNUM,	//师徒每日赠送剩余次数
	RA_OFFLINE_TIME,		//玩家离线时间

	RA_INTIMACY,	//亲密度(UInt16)
	RA_DAILY_MASTERTASK_STATE, //师门任务状态
	RA_REMARKS,			//	备注
	RA_IS_REGRESS,		//   是否是回归玩家
	RA_MARK,			//标志
	RA_HEAD_FRAME,		//头像框
	RA_GUILD_ID,		//公会ID
	RA_RETURN_YEAR_TITLE,//回归周年称号
	//RA_BONLINENOTIFY,	//是否上线提醒(UInt8)
	//RA_BCLOSEFRIEND,	//是否密友(UInt8)
};

/**
 *@brief 关系同步类型                                                                     
 */
enum RelationSyncType
{
	RST_FRIEND = 1 << RELATION_FRIEND,
	RST_BLACKLIST = 1 << RELATION_BLACKLIST,
	RST_STRANGER = 1 << RELATION_STRANGER,
	RST_MASTER = 1 << RELATION_MASTER,
	RST_DISCIPLE = 1 << RELATION_DISCIPLE,

};

//一次征友人数
#define QUICK_MAKE_FRIEND_NUM 6
//祝福瓶开启等级
#define MIN_BLESSING_FRIEND_LEVEL 20
//领取祝福瓶经验的最低等级
#define MIN_GET_BLESSINGEXP_LEVEL 38
//亲密度上限
#define MAX_INTIMACY 60000


/**
 *@brief 是否显示等级
 */
inline bool IsRelationShowLevel(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
*@brief 是否显示战斗力
*/
inline bool IsRelationShowSeasonLv(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
		return true;
	}
	return false;
}

/**
 *@brief 是否显示vip                                                                     
 */
inline bool IsRelationShowVip(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
*@brief 是否显示职业
*/
inline bool IsRelationShowOccu(int type)
{
	switch (type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
 *@brief 是否可设置上限提醒                                                                     
 */
inline bool IsSetOnlineNotify(int type)
{
	switch(type)
	{
	case RELATION_FRIEND:
	case RELATION_MASTER:
	case RELATION_DISCIPLE:
		return true;
	}
	return false;
}

/**
 *@brief 根据亲密度获取亲密度效果buff
 */
inline UInt16 GetIntimacyBuff(UInt16 intimacy)
{
	if(intimacy <= 30) return 0;
	else if(intimacy <= 100) return 2000;
	else if(intimacy <= 150) return 2001;
	else if(intimacy <= 200) return 2002;
	else if(intimacy <= 250) return 2003;
	return 2004;
}

/**
 *@brief 一键征友基本信息
 */
struct QuickFriendInfo
{
public:
	QuickFriendInfo() :playerId(0), occu(0), seasonLv(0), level(0), vipLv(0){}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & playerId & name & occu & seasonLv & level & vipLv & masterNote & avatar 
			& activeTimeType & masterType & regionId & declaration & playerLabelInfo;
	}

	//玩家id
	ObjID_t	playerId;
	//姓名
	std::string	name;
	//职业
	UInt8	occu;
	//性别
	UInt32	seasonLv;
	//等级
	UInt16	level;
	//vip等级
	UInt8	vipLv;
	//师傅公告
	std::string masterNote;
	//外观信息
	PlayerAvatar	avatar;
	//在线时间类型
	UInt8		activeTimeType; 
	//师傅类型
	UInt8		masterType;	
	//地区id
	UInt8		regionId;		
	//宣言
	std::string	declaration;
	//玩家标签信息
	PlayerLabelInfo playerLabelInfo;
};

//好友互赠天数
#define FRIEND_PRESENT_DAYS 15

//好友相赠经验符id
const static UInt32 FRIEND_PRESENT_ITEMS[] = { 0, 85, 86, 87, 88, 89, 90, 91, 92 };

//15日好友相赠奖励
#define FRIEND_PRESENT_ITEM15  31

//每天获好友赠送最大天数
#define MAX_RECEIVE_FRIENDGIFT_TIMES 6

//每天送给异性鲜花的次数
#define MAX_GIVE_FLOWER_NUM 5

//情人节情人礼包
#define VALENTINE_GIFT 422

//好友赠送礼包ID
#define FRIEND_GIVE_GIFT		800000001
//好友赠送次数重置时间（h）
#define FRIEND_DAY_GIFT			6
//好友每日赠送次数
#define FRIEND_DAY_GIVE_MAX		1


enum RelationPresentFlag
{
	RELATION_PRESENT_GIVE = 1 << 0,		//赠送
	RELATION_PRESENT_DRAW = 1 << 1,		//领取
	RELATION_PRESENT_GIVE15 = 1 << 2,	//15日赠送
	RELATION_PRESENT_DRAW15 = 1 << 3,	//15日领取
};

//玫瑰花
enum FlowerType
{
	FLOWER_1 = 424,
	FLOWER_9 = 425,
	FLOWER_99 = 426,
	FLOWER_999 = 427,
};

inline UInt32 GetFlowerNum(UInt32 itemId)
{
	switch(itemId)
	{
	case FLOWER_1: return 1;
	case FLOWER_9: return 9;
	case FLOWER_99:return 99;
	case FLOWER_999: return 999;
	}
	return 0;
}

// 公会称号
struct GuildTitle
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guildName & guildPost;
	}
	// 公会名
	std::string					guildName;
	// 公会职位
	UInt8						guildPost;
};

// 查看玩家的信息
struct PlayerWatchInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & occu & level & equips & fashionEquips & retinue 
			& pkInfo & pkValue & matchScore & vipLevel & guildTitle & seasonLevel 
			& seasonStar & pets & avatar & activeTimeType & masterType & regionId 
			& declaration & playerLabelInfo & adventureTeamName & adventureTeamGrade
			& adventureTeamRanking & emblemLevel & totalEquipScore;
	}

	// 玩家id
	ObjID_t						id;
	// 玩家名
	std::string					name;
	// 职业
	UInt8						occu;
	// 等级
	UInt16						level;
	// 装备
	std::vector<ItemBaseInfo>	equips;
	// 时装
	std::vector<ItemBaseInfo>	fashionEquips;
	// 随从
	RetinueInfo					retinue;
	// 战绩
	PkStatisticInfo				pkInfo;
	// 决斗经验
	UInt32						pkValue;
	// 赛季积分
	UInt32						matchScore;
	// VIP等级
	UInt8						vipLevel;
	// 公会称号
	GuildTitle					guildTitle;
	// 赛季段位等级
	UInt32						seasonLevel;
	// 赛季星级
	UInt32						seasonStar;
	//宠物
	std::vector<PetBaseInfo>	pets;
	//外观信息
	PlayerAvatar				avatar;
	//在线时间类型
	UInt8						activeTimeType;
	//师傅类型
	UInt8						masterType;
	//地区id
	UInt8						regionId;
	//宣言
	std::string					declaration;
	//玩家标签信息
	PlayerLabelInfo				playerLabelInfo;
	//冒险队名
	std::string					adventureTeamName;
	//冒险队评级
	std::string					adventureTeamGrade;
	//冒险队排名
	UInt32						adventureTeamRanking;
	//公会徽记
	UInt32						emblemLevel;
	// 全身装备评分
	UInt32                      totalEquipScore;
};

// 师傅公告最大字数(byte)
# define MASTER_NOTE_MAX_BYTE	64

enum AddonPayType
{
	APT_SELF = 0,
	APT_OTHER = 1,
};

enum AddonState
{
	AS_UNDO = 0,
	AS_AGREE = 1,
	AS_DISAGREE = 2,
};

// 代付订单数据
struct AddonPayOrderData
{
	// 代付id
	UInt64	uid;
	// 社会关系 relationType
	UInt8	relationType;
	// 请求者id
	UInt64	reqId;
	// 请求者名字
	std::string reqName;
	// 请求者职业
	UInt8	reqOccu;
	// 请求者等级
	UInt32	reqLevel;
	// 目标id
	UInt64	tarId;
	// 目标名字
	std::string tarName;
	// 目标职业
	UInt8	tarOccu;
	// 目标等级
	UInt16	tarLv;
	// 到期时间
	UInt32	dueTime;
	// 状态 AddonState
	UInt8	state;
	// 商品id
	UInt32	goodId;
	// 购买数量
	UInt32	buyNum;
};

// 客户端通信的代付信息
struct AddonPayData
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & uid & type & relationType & tarName & tarOccu & tarLv & dueTime & state & payMoneyType & payMoneyNum & words;
	}

	// 代付id
	UInt64	uid;
	// 代付类型 AddonPayType
	UInt8	type;
	// 社会关系 relationType
	UInt8	relationType;
	// 目标名字
	std::string tarName;
	// 目标职业
	UInt8	tarOccu;
	// 目标等级
	UInt16	tarLv;
	// 到期时间
	UInt32	dueTime;
	// 状态 AddonState
	UInt8	state;
	// 货币类型
	UInt32	payMoneyType;
	// 需要货币数量
	UInt32	payMoneyNum;
	// 留言
	std::string words;
};

#define PLAYER_REMARK_MAXLEN 16          //备注最大宽度
#define QUSETIONNAIRE_DECLAR_MAXLEN 40   //问卷宣言最大宽度

//成立师徒关系类型
enum MakeMasterDiscipleType
{
	MMDT_MASTER_REPLY = 1, //师傅同意拜师请求
	MMDT_DISCIPLE_REPLY = 2, //徒弟同意收徒请求
};

//关系标志
enum RelationMarkFlag
{
	RELATION_MARK_MUTUAL_FRIEND = 1 << 0,		//互为好友
};

//查询玩家类别
enum QueryPlayerType
{
	QPT_BATTLE_LOST = 2,// 战场地牢
};

// 客户端通信的招募信息
struct HireInfoData
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & taskID & cnt & status;
	}

	//任务id
	UInt32 taskID = 0;
	//任务计数
	UInt32 cnt = 0;
	// 任务状态
	UInt8 status = 0;
};

// 客户端通信的招募玩家信息
struct HirePlayerData
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & userId & name & occu & online & lv;
	}

	//角色id
	UInt64 userId = 0;
	//姓名
	std::string name;
	//职业
	UInt8 occu;
	//在线状态
	UInt8 online;
	//等级
	UInt32 lv;
};

enum HireTaskType
{
	HTT_LEVEL = 0,				//达到不同等级
	HTT_SHENG_YUAN_PIAO = 1,	//消耗N张深渊票
	HTT_YUAN_GU_PIAO = 2,		//消耗N张远古票
	HTT_GUILD_DUNGEON = 3,		//参与N次公会地下城
	HTT_GUILD_BATTLE = 4,		//参与N次公会战
	HTT_CHAOS_DUNGEON = 5,		//通关N次混沌地下城
	HTT_TEAM_DUNGEON = 6,		//通关N次团队地下城
	HTT_NONE_DUNGEON = 7,		//通关N次虚空地下城
	HTT_MAKE_TEAM = 8,			//每周与被招募的新玩家组队挑战任意地下城10次（深渊远古章节虚空混沌团本
	HTT_BIND_COMPLETE = 9,		//N个被招募的新玩家完成任务X
	HTT_BIND_OLD_MAN = 10,		//与老玩家绑定
	HTT_BIND_RETURN_MAN = 11,   //绑定N个回归玩家
};


#endif
