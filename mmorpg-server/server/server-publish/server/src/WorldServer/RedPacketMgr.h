#ifndef __RED_PACKET_MGR_H__
#define __RED_PACKET_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonDailyTimer.h>
#include "RedPacket.h"

class RedPacketMgr : public Avalon::Singleton<RedPacketMgr>
{
	// 初始状态最多保存时间
	const UInt32 RED_PACKET_INIT_TIME_OUT = 24 * 60 * 60;
	// 发送了的红包保存时间
	const UInt32 RED_PACKET_WAIT_RECEIVE_TIME = 24 * 60 * 60;
	// 红包名最大长度
	const UInt32 RED_PACKET_NAME_MAX_WIDTH = 20;
public:
	RedPacketMgr();
	~RedPacketMgr();

public:
	/**
	*@brief 初始化
	*/
	bool Init();

	/**
	*@brief Tick
	*/
	bool OnTick(const Avalon::Time& now);

	/**
	*@brief 结束
	*/
	void Final();

	/**
	*@brief 获取红包
	*/
	RedPacket* FindRedPacketByID(ObjID_t id) const;

	/**
	*@brief 删除红包
	*/
	void DelRedPacketByID(ObjID_t id);

public:
	/**
	*@brief 玩家上线
	*/
	void OnPlayerOnline(WSPlayer* player);

	/**
	*@brief 玩家修改公会（退会或者加入公会）
	*/
	void OnPlayerChangeGuild(ObjID_t roleId, ObjID_t oldGuildId, ObjID_t newGuildId);

public:
	/**
	*@brief 玩家发送自费红包
	*/
	UInt32 OnPlayerSendCustomRedPacket(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

	/**
	*@brief 玩家添加红包
	*/
	UInt32 OnPlayerAddRedPacket(WSPlayer* player, UInt16 reason, std::string name, ObjID_t& redPacketId);

	/**
	*@brief 玩家发送红包
	*/
	UInt32 OnPlayerSendRedPacket(ObjID_t roleId, ObjID_t redPacketId, UInt8 num = 0);

	/**
	*@brief 玩家打开红包
	*/
	bool OnPlayerOpenRedPacket(WSPlayer* player, ObjID_t redPacketId);

	/**
	*@brief 是否有抢公会红包资格
	*/
	bool IsCanReciveGuildRedPacket(RedPacket& redPacket, ObjID_t palyerId);
	
	/**
	*@brief 获取最近公会战时间
	*/
	UInt32 GetGuildBattleLastTime(UInt8 type);

	/**
	*@brief 类型转换
	*/
	UInt8 RedPackThreTpCoverToPlayerJoinTp(RedPacketThreeType type);

	/**
	*@breif 获取公会红包信息
	*/
	UInt32 GetGuildRedPacketInfo(WSPlayer* player, std::vector<GuildRedPacketSpecInfo>& infos);
public:
	/**
	*@brief 自费红包事务返回
	*/
	UInt32 OnCheckCustomRedPacketRet(WSPlayer* player, UInt16 reason, std::string name, UInt8 num);

public:
	// 数据库相关

	/**
	*@brief 加载红包返回
	*/
	bool OnSelectRedPacketRet(CLRecordCallback* callback);

	/**
	*@brief 加载红包领取者返回
	*/
	bool OnSelectRedPacketReceiverRet(CLRecordCallback* callback);

protected:
	/**
	*@brief 玩家是否关注这个红包
	*/
	bool _IsPlayerSubscribe(WSPlayer* player, RedPacket* redPacket);

	/**
	*@brief 添加红包到管理器中
	*/
	void _AddRedPacket(RedPacket* redPacket);

	/**
	*@brief 删除红包
	*/
	void _DelRedPacket(RedPacket* redPacket);

private:
	HashMap<ObjID_t, RedPacket*>		m_RedPackets;
	Avalon::DailyTimer					m_DailyTimer;
};

#endif