#ifndef _CL_ANTI_ADDICT_MGR_H_
#define _CL_ANTI_ADDICT_MGR_H_

#include <CLAntiAddictDefine.h>

/**
 *@brief 防沉迷管理
 */
class CLAntiAddictMgr
{
public:
	CLAntiAddictMgr();
	virtual ~CLAntiAddictMgr();

	void SetAntiAddictTime(UInt32 time) { m_AntiAddictTime = time; }
	UInt32 GetAntiAddictTime() { return m_AntiAddictTime; }

	void SetIsIDAuth(bool idAuth) { m_IsIDAuth = idAuth; }
	bool GetIsIDAuth() { return m_IsIDAuth; }

	void SetAge(UInt32 age) { m_Age = age; }
	UInt32 GetAge() { return m_Age; }

	UInt32 GetTouristLoginInterval() { return m_TouristLoginInterval; }
	UInt32 GetTouristOnlineTime() { return m_TouristOnlineTime; }

	/**
	 *@brief init
	 */
	void OnInit(const AntiAddictInfo& info, UInt32 touristLoginInterval, UInt32 touristOnlineTime);

	/**
	 *@brief tick
	 */
	virtual void  OnTick(const Avalon::Time& now);

	/**
	 *@brief 跨天
	 */
	virtual void OnDayChanged();

	/**
	 *@brief 登录检查
	 */
	UInt32 OnLoginCheck();

	/**
	 *@brief 获取沉迷信息
	 */
	AntiAddictInfo GetAntiAddictInfo();

	/**
	 *@brief 防沉迷超时
	 */
	UInt32 AddictTimeOut(UInt32& allowGameTime);

	/**
	 *@brief 防沉迷夜晚
	 */
	bool IsNightAddict();

	/**
	 *@brief 是否成年
	 */
	bool IsAdult();

private:

	// 沉迷时间
	UInt32 m_AntiAddictTime;
	// 是否身份认证
	UInt32 m_IsIDAuth;
	// 年龄
	UInt32 m_Age;
	// 账号上次登录时间
	UInt32 m_LastLoginTime;
	// 游客登录间隔
	UInt32 m_TouristLoginInterval;
	// 游客在线游戏时间
	UInt32 m_TouristOnlineTime;
};

#endif
