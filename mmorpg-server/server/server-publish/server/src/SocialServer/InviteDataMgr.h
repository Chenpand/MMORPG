#ifndef _INVITE_DATA_MGR_H_
#define _INVITE_DATA_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonPropertySet.h>
#include <AvalonProtocol.h>
#include <CLInviteGameProtocol.h>

struct memcached_st;

struct PlayerData;

/**
 *@brief 邀请数据管理器
 */
class InviteDataMgr : public Avalon::Singleton<InviteDataMgr>
{
public:
	InviteDataMgr();
	~InviteDataMgr();

public:
	/**
	 *@brief 初始化终止
	 */
	bool Init(Avalon::PropertySet* properties);
	void Final();

	/**
	 *@brief 请求被邀请者列表
	 */
	bool GetInviteeList(UInt32 inviter, std::vector<CLProtocol::InviteeInfo>& list, UInt32& totalGold);

	/**
	 *@brief 检查礼包条件
	 */
	bool CheckGiftbagCond(UInt32 accId, UInt8 type, UInt16 minLevel, UInt16 maxLevel, UInt32 playerNum);

	/**
	 *@brief 检查领取召回奖励条件
	 */
	bool CheckRecallAwardCond(UInt32 accId, UInt32 inviteeId, std::vector<CLProtocol::InviteeInfo>& list);

	/**
	 *@brief 检查领取消费返利
	 */
	bool CheckConsumeAward(UInt32 accId, UInt32 maxGold, UInt32& gold, UInt32& totalGold);

	/**
	 *@brief 绑定恐龙活动账号                                                                     
	 */
	bool BindKLPlayer(UInt32 accId, const std::string& openid, UInt16 level, const std::string& klopenid);

	/**
	 *@brief 检查恐龙活动                                                                     
	 */
	void CheckKLActivity(UInt32 nodeId, UInt32 serverId, UInt32 accId, ObjID_t playerId);

public://消息相关
	/**
	 *@brief 发送消息
	 */
	void SendProtocol(UInt32 connId, Avalon::Protocol& protocol);

public://事件
	/**
	 *@brief 玩家升级
	 */
	void OnPlayerLevelup(UInt32 uid, UInt16 level);

	/**
	 *@brief 上线
	 */
	void OnPlayerOnline(UInt32 uid);

	/**
	 *@brief 下限
	 */
	 void OnPlayerOffline(UInt32 uid);

	 /**
	  *@brief 玩家消费
	  */
	 void OnPlayerConsume(UInt32 uid, UInt32 gold);

	 /**
	  *@brief 被邀请者进入游戏
	  */
	 void OnInviteeEnterGame(UInt32 inviteeId, const std::string& openid, UInt32 inviterId);

private:
	/**
	 *@brief 根据玩家id获取数据
	 */
	bool GetPlayerDataByUID(UInt32 uid, PlayerData& data);

	/**
	 *@brief 根据玩家id列表获取数据
	 */
	bool GetPlayerDataByUIDs(const UInt32* ids, UInt32 idNum, std::vector<PlayerData*>& datas);

	/**
	 *@brief 保存玩家数据
	 */
	bool SetPlayerDataByUID(UInt32 uid, const PlayerData& pData);

	/**
	 *@brief 添加一个玩家数据
	 */
	bool AddPlayerDataByUID(UInt32 uid, const PlayerData& pData);

	/**
	 *@brief 销毁数据
	 */
	void FreePlayerDatas(std::vector<PlayerData*>& datas);

	/**
	 *@brief 获取被邀请者信息
	 */
	void GetInviteeData(const PlayerData& pData, UInt32 dayBeginTime, CLProtocol::InviteeInfo& info);

private:
	//memcached状态
	memcached_st* m_pMemc;
};

#endif
