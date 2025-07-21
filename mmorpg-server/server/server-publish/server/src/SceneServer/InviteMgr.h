#ifndef _INVITE_MGR_H_
#define _INVITE_MGR_H_

#include <CLDefine.h>
class Player;
class RewardGroup;

/**
 *@brief 邀请管理器
 */
class InviteMgr
{
	//每个被邀请者召回银币奖励
	const static UInt32 INVITEE_RECALL_SILVER = 100;
	//每日最大可领取返利
	const static UInt32	MAX_DAILY_GOLDAWARD = 360;

public:
	InviteMgr();
	~InviteMgr();

public:
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const{ return m_pOwner; }

	/**
	 *发送每日信息
	 */
	void SendDailyInfo();

	/**
	 *@brief 领取礼包
	 */
	void GetGiftbag(UInt8 type);

	/**
	 *@brief 领取召回奖励
	 */
	void GetRecallAward(UInt32 uid);

	/**
	 *@brief 领取消费回馈
	 */
	void GetConsumeAward();

	/**
	 *@brief 请求邀请好友列表
	 */
	void RequestInviteeList();

public://事件
	/**
	 *@brief 上线                                                                     
	 */
	void OnOnline();

	/**
	 *@brief 新邀请了一个
	 */
	void OnInvitFriend();

	/**
	 *@brief 检查礼包条件返回
	 */
	void OnCheckGiftbagCondRet(UInt8 type);

	/**
	 *@brief 检查领取召回奖励返回
	 */
	void OnCheckRecallAwardRet(UInt32 num);

	/**
	 *@brief 检查领取消费返利返回
	 */
	void OnCheckConsumeAwardRet(UInt32 gold, UInt32 totalGold);

private:
	/**
	 *@brief 判断礼包是否领取
	 */
	bool CheckGiftbagGot(UInt8 type);
	void SetGiftbagGot(UInt8 type);

	/**
	 *@brief 获取礼包id
	 */
	UInt32 GetGiftbagID(UInt8 type);

	/**
	 *@brief 给恐龙礼包                                                                     
	 */
	void GiveMailGiftbag(const char* name, RewardGroup* rewardGroup);

private:
	//拥有者
	Player* m_pOwner;
	//上次领取金贝时间
	UInt32	m_GetGoldAwardTime;
};

#endif
