#include "BSBattleSwitchMgr.h"
#include "BSApplication.h"
#include "BSSortListMgr.h"
#include "GameZoneMgr.h"

#include <CLChiJiSwitchDataEntry.h>
#include <CLBattleProtocol.h>
#include <CLSysProtocol.h>

BSBattleSwitchMgr::BSBattleSwitchMgr()
{
	m_TickTimer1000ms.SetInterval(1000);
	m_TickTimer30s.SetInterval(30 * 1000);
	m_OpenTime = 0;
	m_CurWeek = 0;
	startTime = 0;
	endTime = 0;
}

BSBattleSwitchMgr::~BSBattleSwitchMgr()
{

}

bool BSBattleSwitchMgr::Init()
{
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	Avalon::ResultSet* result = dbConn->Query("select * from t_chi_ji_switch");
	if (NULL == result) return false;

	InfoLogStream << " BSBattleSwitchMgr Init Success" << LogStream::eos;

	while (result->NextRow())
	{
		m_OpenTime = result->GetField("open_time").GetUInt64();
	}

	dbConn->FreeResultSet(result);

	_InitTime();
	return true;
}

void BSBattleSwitchMgr::OnTick(const Avalon::Time& now)
{
	if (!m_TickTimer1000ms.IsTimeout(now))
	{
		return;
	}

	if (m_CurWeek != CURRENT_DATE.WDay())
	{
		_InitTime();
	}

	// 吃鸡开放
	if (startTime != 0 && m_OpenTime != startTime && now.Sec() > startTime)
	{
		UInt64 tempTime = m_OpenTime;
		m_OpenTime = startTime;
		if (tempTime == 0)
			_InsertOpenTime();
		else
			_UpdateOpenTime();

		// 清理排行榜
		BSSortListMgr::Instance()->ClearSortList();
	}

	// 通知各个scene清除玩家身上的排行积分
	if (m_TickTimer30s.IsTimeout(now))
	{
		CLProtocol::BattleOpenNotify notify;
		notify.openTime = m_OpenTime;

		CLProtocol::SysTransmitInNodesType req;
		req.destnodetype = (UInt8)ST_SCENE;
		if (notify.Encode(req.packet))
		{
			GameZoneMgr::Instance()->BroadcastToGameZone(req);
		}
	}

	// 吃鸡关闭, 暂时什么也不做
	if (endTime != 0 && now.Sec() > endTime)
	{
		
	}
}

bool BSBattleSwitchMgr::IsOpen()
{
	if (startTime == 0 || endTime == 0)
	{
		return false;
	}

	if (CURRENT_TIME.Sec() < startTime && CURRENT_TIME.Sec() > endTime)
	{
		return false;
	}

	return true;
}

void BSBattleSwitchMgr::_InitTime()
{
	m_CurWeek = CURRENT_DATE.WDay();
	const ChiJiSwitch* jiSwitch = ChiJiSwitchDataEntryMgr::Instance()->GetChiJiSwitch(m_CurWeek);
	if (NULL != jiSwitch)
	{
		UInt64 day_start = GetDayBeginTime(CURRENT_TIME) / 1000;
		startTime = day_start + jiSwitch->startTime;
		endTime = day_start + jiSwitch->endTime;
	}
	else
	{
		startTime = endTime = 0;
	}
}

void BSBattleSwitchMgr::_InsertOpenTime()
{
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	if (NULL == dbConn)
	{
		ErrorLogStream << "dbConn null!" << LogStream::eos;
		return;
	}

	std::string insertSql;
	insertSql.reserve(128);
	insertSql.append("insert into t_chi_ji_switch values(")
		.append(std::to_string(m_OpenTime)).append(")");

	if (dbConn->Update(insertSql) <= 0)
	{
		ErrorLogStream << "_InsertData " << insertSql << " error:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}

void BSBattleSwitchMgr::_UpdateOpenTime()
{
	Avalon::RetryDBConnection* dbConn = BSApplication::Instance()->GetDBConnection();
	if (NULL == dbConn)
	{
		ErrorLogStream << "dbConn null!" << LogStream::eos;
		return;
	}

	std::string updateSql;
	updateSql.reserve(128);
	updateSql.append("update t_chi_ji_switch set ")
		.append("open_time=").append(std::to_string(m_OpenTime))
		.append(";");

	if (dbConn->Update(updateSql) <= 0)
	{
		ErrorLogStream << "_UpdateData " << updateSql << " error:" << dbConn->GetError() << LogStream::eos;
		return;
	}
}
