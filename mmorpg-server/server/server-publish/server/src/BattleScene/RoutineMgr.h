#ifndef _ROUTINE_MGR_H_
#define _ROUTINE_MGR_H_

#include <CLDefine.h>
#include <CLItemDefine.h>
#include <CLObjectProperty.h>

class Player;

/**
 *@brief 日常管理器
 */
class RoutineMgr : public CLComplexProperty
{
	typedef std::map<UInt8, UInt8> ActiveEventMap;

public:
	RoutineMgr();
	~RoutineMgr();

public:
	void SetOwner(Player* owner){ m_pOwner = owner; }
	Player* GetOwner() const{ return m_pOwner; }

public:

	/**
	 *@brief 发送活跃度信息
	 */
	void SendActiveDegreeInfo();

	/**
	 *@brief 发送今日签到奖励
	 */
	void SendTodaySigninInfo();

	/**
	 *@brief 领取箱子
	 */
	bool GetAward(UInt8 index);

	/**
	 *@brief 获取今天在线时间
	 */
	UInt32 GetDailyOnlineTime() const;

	/**
	*@brief 获取活跃度
	*/
	UInt32 GetActiveDegree() const;

private:

	/**
	 *@brief 获取连续在线天数
	 */
	void  Signin();
	UInt32 GetSigninDays() const;

	/**
	 *@brief 给予奖励
	 */
	bool GiveAward(const char* reason, const std::vector<ItemReward>& rewards);

	/**
	 *@brief 给予邮件
	 */
	void GiveMailAward(const char* name, const std::vector<ItemReward>& rewards);

public: //事件
	/**
	 *@brief 上线
	 */
	void OnOnline(bool bDayChanged);

	/**
	 *@brief 下线
	 */
	void OnOffline();

	/**
	 *@brief 进入第二天
	 */
	void OnDayChanged();

	/**
	 *@brief 完成活跃度事件
	 */
	void OnActiveEventFinished(UInt8 id);

public: //编解码
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_ActiveEvents;
	}

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	//拥有者
	Player*	m_pOwner;

	//活跃度项  noted by aprilliu,2016.05.03 每个日常活动项 当天已完成的次数
	ActiveEventMap	m_ActiveEvents;
};

#endif
