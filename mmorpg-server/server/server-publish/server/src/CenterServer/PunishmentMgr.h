#ifndef _PUNISHMENT_MGR_H_
#define _PUNISHMENT_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>

class GameZone;

/**
 *@brief 惩罚管理器
 */
class PunishmentMgr : public Avalon::Singleton<PunishmentMgr>
{
	typedef std::map<UInt32, UInt32> PlayerMap;

	//一次禁言时间
	const static UInt32	FORBITTALK_TIME = DAY_TO_SEC * 30;

public:
	PunishmentMgr();
	~PunishmentMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 禁言一个玩家
	 */
	void ForbitTalk(UInt32 accid);

	/**
	 *@brief 解禁一个玩家
	 */
	void UnForbitTalk(UInt32 accid);

	/**
	 *@brief 发送禁言列表
	 */
	void SendForbidList(GameZone* zone);

public: //事件
	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 禁言
	 */
	void OnPunished(UInt32 accid);

	/**
	 *@brief 解禁
	 */
	void OnUnPunished(UInt32 accid);

	/** 
	 *@brief 游戏区建立连接
	 */
	void OnGameZoneConnected(GameZone* zone);

private:
	/**
	 *@brief 删除禁言
	 */
	void DeleteForbidTalk(UInt32 accid);

private:
	//禁言列表
	PlayerMap	m_PunishmentMap;
};

#endif
