#ifndef _CL_CHAT_DEFINE_H_
#define _CL_CHAT_DEFINE_H_

#include "CLDefine.h"
#include <AvalonNetStream.h>

//最大聊天内容长度
#define CL_MAX_CHAT_LEN (512)
//最大显示宽度
#define CL_MAX_SHOW_WIDTH (100)
//聊天高频限制次数最大值
#define CHAT_HI_FRQ_MAX		5
//高频聊天禁言时间
#define CHAT_HI_FRQ_TIME	1 * 60

/**
 *@brief 聊天系统频道定义
 */
enum ChatChannelType
{
	CHAT_CHANNEL_AROUND = 1,	//附近频道
	CHAT_CHANNEL_TEAM = 2,		//队伍频道
	CHAT_CHANNEL_WORLD = 3,		//世界
	CHAT_CHANNEL_PRIVATE = 4,	//私聊
	CHAT_CHANNEL_TRIBE = 5,		//公会
	CHAT_CHANNEL_TEAM_SPECIAL = 7,	//队伍趣味表情
	CHAT_CHANNEL_SYSTEM = 8,	//系统
	CHAT_CHANNEL_TEAM_INVITE = 9,	//组队邀请频道
	CHAT_CHANNEL_HORN = 10,		// 喇叭
	CHAT_CHANNEL_ROOM = 11,		//房间

	CHAT_CHANNEL_TEAM_COPY_PREPARE = 12,	//团本准备
	CHAT_CHANNEL_TEAM_COPY_TEAM = 13,		//团本队伍
	CHAT_CHANNEL_TEAM_COPY_SQUAD = 14,		//团本小队

	CHAT_CHANNEL_BATTLE = 15,			//战场
	CHAT_CHANNEL_BAT_LOST_TEAM = 16,	//战场地牢队伍
	//TODO
	CHAT_CHANNEL_MAX,			
};



//聊天间隔限制
const static UInt32 CHAT_INTERVAL[] = { 0, 30000, 1000, 30000, 500, 1000, 10000, 100, 100, 30000, 0, 0, 30000, 5000, 5000 };
const static UInt32 NEWERTRIBE_CHAR_INTERVAL = 8000;

struct ChatDataInfo
{
	//频道
	UInt8		channel;
	//说话人id
	ObjID_t		objid;
	//性别
	UInt8		sex;
	//职业
	UInt8		occu;
	//等级
	UInt16		level;
	//vip等级
	UInt8		viplvl;
	//说话人名字
	std::string	objname;
	//接收者id
	ObjID_t		receiverId;
	//语句
	std::string	word;
	//是否link
	UInt8		bLink;
	//是否gm
	UInt8		isGm;
	//时间
	UInt32		time;
	//语音key
	std::string voiceKey;
	//语音长度
	UInt8		voiceDuration;
	// 头像框
	UInt32		headFrame;
};

/**
 *@brief 聊天计时器 
 */
class ChatTimer
{
public:
	ChatTimer(){ memset(m_Times,0,sizeof(m_Times)); }
	~ChatTimer(){}

public:
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream.SeriaArray(m_Times, CHAT_CHANNEL_MAX);
	}

	/**
	 *@brief 判断时间
	 *@return 如果判断成功，返回0，否则返回剩余时间
	 */
	inline UInt64 CheckChatTime(int channel, UInt64 now);
	inline void SetChatTime(int channel, UInt64 now);
	inline UInt64 GetChatTime(int channel);

private:
	//上次聊天时间
	UInt64 m_Times[CHAT_CHANNEL_MAX];
};

UInt64 ChatTimer::CheckChatTime(int channel,UInt64 now)
{
	if(channel >= CHAT_CHANNEL_MAX) return 0;

	if(now < m_Times[channel] + CHAT_INTERVAL[channel]){
		return m_Times[channel] + CHAT_INTERVAL[channel] - now;
	}
	return 0;
}

void ChatTimer::SetChatTime(int channel, UInt64 now)
{
	if(channel >= CHAT_CHANNEL_MAX) return;

	m_Times[channel] = now;
}

UInt64 ChatTimer::GetChatTime(int channel)
{
	if(channel >= CHAT_CHANNEL_MAX) return 0;

	return m_Times[channel];
}

/** 
 *@brief 聊天链接数据类型
 */
enum ChatLinkDataType
{
	CHAT_LINKDATA_INVALID = 0,
	CHAT_LINKDATA_ITEM = 1,
	CHAT_LINKDATA_PET = 2,
};


/**
 *@brief 请求类型
 */
enum RequestType
{
	REQUEST_TRADE = 0,				//交易请求
	REQUEST_INVITETEAM = 1,			//邀请入队
	REQUEST_JOINTEAM = 2,			//请求入队(根据队伍成员ID)
	REQUEST_FRIEND = 3,				//请求加为好友
	REQUEST_MASTER = 4,			//请求拜师
	REQUEST_DISCIPLE = 5,		//请求收徒
	REQUEST_RECOMMENDTRIBE = 6,		//推荐部落
	REQUEST_JOINCOPYTEAM = 7,		//请求加入副本队伍
	REQUEST_CANCELENTER = 8,		//取消加入部落申请
	REQUEST_TRIBECALL = 9,			//部落召唤
	REQUEST_BLESSLEVELUP = 10,		//祝福升级
	REQUEST_QUICKFRIEND = 11,		//一键征友
	REQUEST_FLOWER = 14,			 //接受鲜花提示
	REQUEST_TRIBE_UNION = 15,		//部落联盟
	REQUEST_TRIBE_CANCEL_UNION = 16,	//取消部落联盟
	REQUEST_TRIBEWAR_INVITE = 17,	//宝地战邀请
	REQUEST_TRIBEWAR_CL_INVIATE = 18,	//取消宝地战邀请
	REQUEST_MARRY = 19,				//结婚
	REQUEST_MARRY_SUCC = 20,		//结婚成功
	REQUEST_JOINTEAM_TEAMID = 21,	//请求入队(根据队伍ID)

	REQUEST_MARRY_ASK = 22,			//索要请帖
	REQUEST_MARRY_GIVEGIFT1 = 23,	//赠送小贺礼
	REQUEST_MARRY_GIVEGIFT2 = 24,	//赠送中贺礼
	REQUEST_MARRY_GIVEGIFT3 = 25,	//赠送大贺礼
	REQUEST_MARRY_RITE = 26,		//拜天地
	REQUEST_DIVORCE = 27,			//离婚

	REQUEST_ROLL = 28,				//roll点

	REQUEST_FRIEND_BY_NAME = 29,				//通过名字请求加为好友

    REQUEST_CHALLENGE_PK = 30,      // 挑战
	REQUEST_INVITEGUILD = 31,		// 邀请公会
	REQUEST_EQUAL_PK    = 32,       //公平竞技场邀请
	//TODO
	REQUEST_MAX,
};

//每类请求最大数量
#define	MAX_REQUEST_NUM		100
//请求最大有效时间
#define REQUEST_VALID_TIME	600

//最低允许聊天等级
#define MIN_CHAT_LEVEL	10

struct HornInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roldId & name & occu & level & viplvl & content & minTime & maxTime & combo & num & headFrame;
	}

	// 发送者的基本信息
	//角色id
	ObjID_t			roldId;
	//名字
	std::string		name;
	//职业
	UInt8			occu;
	//等级
	UInt16			level;
	//vip等级
	UInt8			viplvl;
	
	// 喇叭具体信息
	// 内容
	std::string		content;
	// 保护时间
	UInt8			minTime;
	// 持续时间
	UInt8			maxTime;
	// combo数
	UInt16			combo;
	// 连发数量
	UInt8			num;
	// 头像框
	UInt32			headFrame;
};

// 红包消息
enum ChatMask
{
	CHAT_MASK_RED_PACKET = 1 << 0,
	CHAT_MASK_ADD_FRIEND = 1 << 1,		//添加好友
	CHAT_MASK_NOT_HEADPOINT = 1 << 2,	//不要红点提示
	CHAT_MASK_SYS = 1 << 3, //系统提示
};

// 自定义日志上报类型
enum CustomLogReportType
{
	CLRT_INVALID,
	// 加入语音房间
	CLRT_JOIN_VOICE_ROOM,
	// 退出语音房间
	CLRT_QUIT_VOICE_ROOM,
	// 发送录音
	CLRT_SEND_RECORD_VOICE,
	// 下载录音
	CLRT_LOAD_RECORD_VOICE,
};

#endif
