#include "CLAntiAddictMgr.h"
#include <CLErrorCode.h>
#include <CLAntiAddictHolidayDataEntry.h>
#include <CLAntiAddictTimeDataEntry.h>

CLAntiAddictMgr::CLAntiAddictMgr()
{
	m_AntiAddictTime = 0;
	m_IsIDAuth = 0;
	m_Age = 0;
	m_LastLoginTime = 0;
	m_TouristLoginInterval = 0;
	m_TouristOnlineTime = 0;
}

CLAntiAddictMgr::~CLAntiAddictMgr()
{

}

void CLAntiAddictMgr::OnInit(const AntiAddictInfo& info, UInt32 touristLoginInterval, UInt32 touristOnlineTime)
{
	m_AntiAddictTime = info.addictSecTime;
	m_Age = info.age;
	m_IsIDAuth = info.isIDAuth;
	m_LastLoginTime = info.lastLoginTime;

	m_TouristLoginInterval = touristLoginInterval * 3600;
	m_TouristOnlineTime = touristOnlineTime * 3600;

	// 年龄特殊处理
	if (m_Age == 0)
	{
		if (m_IsIDAuth == AUTH_NO_ADULT)
			m_Age = 17;
		else if(m_IsIDAuth == AUTH_ADULT)
			m_Age = 20;
	}
}

void CLAntiAddictMgr::OnTick(const Avalon::Time& now)
{
	++m_AntiAddictTime;
}

void CLAntiAddictMgr::OnDayChanged()
{
	m_AntiAddictTime = 0;
}

UInt32 CLAntiAddictMgr::OnLoginCheck()
{
	if (IsAdult())
	{
		return ErrorCode::SUCCESS;
	}

	// 未验证(游客)15天登录限制
	if (m_IsIDAuth == AUTH_NO_ID)
	{
		if ((UInt32)CURRENT_TIME.Sec() - m_LastLoginTime < m_TouristLoginInterval)
		{
			return ErrorCode::LOGIN_ANTIADDICT_TOURIST_LOGIN;
		}
	}
	else
	{
		// 未成年夜间不允许登录
		if (IsNightAddict())
		{
			return ErrorCode::LOGIN_ANTIADDICT_NIGHT;
		}
	}

	UInt32 time = 0;
	return AddictTimeOut(time);
}

AntiAddictInfo CLAntiAddictMgr::GetAntiAddictInfo()
{
	AntiAddictInfo info;
	info.Init(m_AntiAddictTime, m_Age, m_IsIDAuth, m_LastLoginTime);
	return info;
}

UInt32 CLAntiAddictMgr::AddictTimeOut(UInt32& allowGameTime)
{
	// 未认证（游客）
	if (m_IsIDAuth == AUTH_NO_ID)
	{
		allowGameTime = m_TouristOnlineTime;

		// 游客可在线玩1小时
		if (m_AntiAddictTime > m_TouristOnlineTime)
		{
			return ErrorCode::LOGIN_ANTIADDICT_TOURIST_ONLINE;
		}
	}
	else
	{
		bool isHoliday = AntiAddictHolidayDataEntryMgr::Instance()->TodayIsHoliday();
		UInt32 gameTime = AntiAddictTimeDataEntryMgr::Instance()->GetAntiAddictTime(isHoliday, m_Age);
		allowGameTime = gameTime * MIN_TO_SEC;

		if (gameTime != 0 && m_AntiAddictTime >= gameTime * MIN_TO_SEC)
		{
			return isHoliday ? ErrorCode::LOGIN_ANTIADDICT_TIPS_HOLIDAY : ErrorCode::LOGIN_ANTIADDICT_TIPS_COMMON;
		}
	}

	return ErrorCode::SUCCESS;
}

bool CLAntiAddictMgr::IsNightAddict()
{
	return CURRENT_DATE.Hour() >= 22 || (CURRENT_DATE.Hour() >= 0 && CURRENT_DATE.Hour() < 8);
}

bool CLAntiAddictMgr::IsAdult()
{
	return m_IsIDAuth == AUTH_ADULT;
}