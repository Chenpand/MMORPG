#ifndef __CL_RED_PACKET_DEFINE_H__
#define __CL_RED_PACKET_DEFINE_H__

#include <CLDefine.h>
#include <AvalonNetStream.h>

typedef PlayerIcon RedPacketPlayerIcon;

// 红包状态
enum RedPacketStatus
{
	// 初始状态
	RED_PACKET_STATUS_INIT,
	// 未达成
	RED_PACKET_STATUS_UNSATISFY,
	// 等待别人领取红包
	RED_PACKET_STATUS_WAIT_RECEIVE,
	// 已抢
	RED_PACKET_STATUS_RECEIVED,
	// 已被领完
	RED_PACKET_STATUS_EMPTY,
	// 可摧毁
	RED_PACKET_STATUS_DESTORY,
};

// 红包类型
enum RedPacketType
{
	// 公会红包
	RED_PACKET_TYPE_GUILD = 1,
	// 全服红包
	RED_PACKET_TYPE_NEW_YEAR = 2,
};

// 红包第三类型
enum RedPacketThreeType
{
	// 公会全体成员
	RPT_THR_TYPE_GUILD_ALL = 1,
	// 参与公会战成员
	RPT_THR_TYPE_GUILD_BATTLE = 2,
	// 参与跨服公会战成员
	RPT_THR_TYPE_GUILD_CROSS_BATTLE = 3,
	// 参与公会地下城成员
	RPT_THR_TYPE_GUILD_DUNGEON = 4,

	RPT_THR_TYPE_GUIL_MAX,
};

// 红包基础信息
struct RedPacketBaseEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & id & name & ownerId & ownerName & status & opened & type & reason & totalMoney & totalNum & remainNum & guildTimeStamp;
	}

	// 红包ID
	ObjID_t		id;
	// 名字
	std::string	name;
	// 发送者ID
	ObjID_t		ownerId;
	// 发送者名字
	std::string	ownerName;
	// 状态(RedPacketStatus)
	UInt8		status;
	// 有没有打开过
	UInt8		opened;
	// 红包类型(对应枚举RedPacketType)
	UInt8		type;
	// 红包来源
	UInt16		reason;
	// 红包金额
	UInt32		totalMoney;
	// 红包数量
	UInt8		totalNum;
	// 红包剩余数量
	UInt8		remainNum;
	// 公会系列战场次时间戳
	UInt32		guildTimeStamp;
};

// 红包领取者信息
struct RedPacketReceiverEntry
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & icon & money;
	}

	// icon
	RedPacketPlayerIcon icon;
	// 获得金额
	UInt32				money;
};

// 红包详细信息
struct RedPacketDetail
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & baseEntry & ownerIcon & receivers;
	}

	// 基础信息
	RedPacketBaseEntry					baseEntry;
	// 拥有者头像
	RedPacketPlayerIcon					ownerIcon;
	// 所有领取的玩家
	std::vector<RedPacketReceiverEntry> receivers;
};

// 红包领取记录
struct RedPacketRecord 
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & time & redPackOwnerName & moneyId & moneyNum & isBest;
	}
	
	//唯一id
	ObjID_t guid;

	//时间
	UInt32  time;

	//红包发送者名字
	std::string	redPackOwnerName;

	//货币id
	UInt32  moneyId;

	//货币数量
	UInt32 moneyNum;

	//是否最佳红包
	UInt8  isBest;
};

// 公会红包相关信息
struct GuildRedPacketSpecInfo 
{
	GuildRedPacketSpecInfo()
	{
		type = 0;
		lastTime = 0;
		joinNum = 0;
	}
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & type & lastTime & joinNum;
	}
	//类型
	UInt8		type;
	//时间
	UInt32		lastTime;
	//参与人数
	UInt32		joinNum;
};

#endif