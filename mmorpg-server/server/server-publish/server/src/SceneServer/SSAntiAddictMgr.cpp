#include "SSAntiAddictMgr.h"

#include <CLCenterProtocol.h>
#include <CLErrorCode.h>
#include <CLAntiAddictHolidayDataEntry.h>
#include <CLAntiAddictTimeDataEntry.h>
#include "SSRouter.h"
#include "SSApplication.h"
#include "Player.h"

SSAntiAddictMgr::SSAntiAddictMgr()
{
	m_SyncAddictTimer.SetInterval(5 * 60 * 1000);
}

SSAntiAddictMgr::~SSAntiAddictMgr()
{

}

void SSAntiAddictMgr::OnTick(const Avalon::Time& now)
{
	if (IsAdult() || !SSApplication::Instance()->IsCheckAntiAddict())
	{
		return;
	}

	CLAntiAddictMgr::OnTick(now);

	if (m_SyncAddictTimer.IsTimeout(now))
	{
		DebugLogStream << PLAYERINFO(GetOwner()) << "sync addict time:" << GetAntiAddictTime() << LogStream::eos;
		SyncAddictTimeToCenter();
	}

	bool isKick = false;
	UInt32 notifyId = 0;
	UInt32 allowGameTime = 0;

	do
	{
		// δ����ҹ���ߵ�
		if (GetIsIDAuth() == AUTH_NO_ADULT && IsNightAddict())
		{
			isKick = true;
			notifyId = ErrorCode::LOGIN_ANTIADDICT_NIGHT;
			break;
		}

		// δ��֤���οͣ�
		if (GetIsIDAuth() == AUTH_NO_ID)
		{
			allowGameTime = GetTouristOnlineTime();

			// ��ǰʮ����ʾ
			if (GetTouristOnlineTime() - GetAntiAddictTime() == SSApplication::Instance()->GetAntiAddictTipTime())
			{
				notifyId = ErrorCode::LOGIN_ANTIADDICT_TOURIST_ONLINE;
				break;
			}

			// �οͿ�������1Сʱ
			if (GetAntiAddictTime() >= GetTouristOnlineTime())
			{
				isKick = true;
				notifyId = ErrorCode::LOGIN_ANTIADDICT_TOURIST_ONLINE;
				break;
			}
		}
		else
		{
			bool isHoliday = AntiAddictHolidayDataEntryMgr::Instance()->TodayIsHoliday();
			UInt32 gameTime = AntiAddictTimeDataEntryMgr::Instance()->GetAntiAddictTime(isHoliday, GetAge());
			allowGameTime = gameTime * MIN_TO_SEC;

			if (gameTime != 0 && gameTime * MIN_TO_SEC - GetAntiAddictTime() == SSApplication::Instance()->GetAntiAddictTipTime())
			{
				notifyId = isHoliday ? ErrorCode::LOGIN_ANTIADDICT_TIPS_HOLIDAY : ErrorCode::LOGIN_ANTIADDICT_TIPS_COMMON;
				break;
			}

			if (gameTime != 0 && GetAntiAddictTime() >= gameTime * MIN_TO_SEC)
			{
				isKick = true;
				notifyId = isHoliday ? ErrorCode::LOGIN_ANTIADDICT_TIPS_HOLIDAY : ErrorCode::LOGIN_ANTIADDICT_TIPS_COMMON;
				break;
			}
		}
	} while (0);

	// ������ʱ��, ������
	if (!GetOwner()->IsInDungeon() && !GetOwner()->IsInPK())
	{
		if (isKick)
		{
			GetOwner()->KickOff(notifyId);
			return;
		}

		if (notifyId != 0)
		{
			GetOwner()->SendNotify(notifyId);
		}
	}

	// ��ʾ��ҳ���ʣ��ʱ��
	if (GetIsIDAuth() == AUTH_NO_ID)
	{
		UInt32 notifyTime = SSApplication::Instance()->IsNotifyTourAntiAddict(allowGameTime - GetAntiAddictTime());
		if (notifyTime != 0)
		{
			GetOwner()->SendNotify(ErrorCode::LOGIN_ANTIADDICT_TOURIST_TIME_TIP, notifyTime);
		}
	}
	else
	{
		UInt32 notifyTime = SSApplication::Instance()->IsNotifyNoAdultAntiAddict(allowGameTime - GetAntiAddictTime());
		if (notifyTime != 0)
		{
			GetOwner()->SendNotify(ErrorCode::LOGIN_ANTIADDICT_NOADULT_TIME_TIP, notifyTime);
		}
	}
}

void SSAntiAddictMgr::OnDayChanged()
{
	if (IsAdult() || !SSApplication::Instance()->IsCheckAntiAddict())
	{
		return;
	}

	DebugLogStream << PLAYERINFO(GetOwner()) << "day change sync addict time:" << GetAntiAddictTime() << LogStream::eos;

	CLAntiAddictMgr::OnDayChanged();

	// ֪ͨCenterServer
	SyncAddictTimeToCenter();
}

void SSAntiAddictMgr::SendAntiAddictNotice(UInt32 id)
{
	GetOwner()->SendNotify(id);
	GetOwner()->KickOff(id);
}

void SSAntiAddictMgr::SyncAddictTimeToCenter()
{
	if (IsAdult() || !SSApplication::Instance()->IsCheckAntiAddict())
	{
		return;
	}

	CLProtocol::CenterPlayerAddictTimeSync protocol;
	protocol.platform = GetOwner()->GetPf();
	protocol.openid = GetOwner()->GetOpenId();
	protocol.accid = GetOwner()->GetAccID();
	protocol.roleid = GetOwner()->GetID();
	protocol.addictTime = GetAntiAddictTime();
	Router::BroadcastToGate(protocol);
}