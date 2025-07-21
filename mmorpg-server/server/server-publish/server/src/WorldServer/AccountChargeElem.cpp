#include "AccountChargeElem.h"
#include <CLRecordClient.h>

AccountChargeElem::AccountChargeElem()
{
	m_MallType = 0;
	m_GoodsId = 0;
	m_Accid = 0;
	m_RoleId = 0;
	m_TodayTimes = 0;
	m_TotalTimes = 0;
	m_UpdateTime = 0;
}

UInt32 AccountChargeElem::GetTodayTimes()
{
	if (m_TodayTimes > 0)
	{
		// 检测需不需刷新
		bool reset = false;
		Avalon::Time updateTime(UInt64(GetUpdateTime()) * 1000);
		Avalon::Date updateDate(updateTime);
		UInt64 curTime = Avalon::Time::CurrentTime().MSec();

		Avalon::Date refreshDate = updateDate;
		refreshDate.Hour(6);
		refreshDate.Min(0);
		refreshDate.Sec(0);
		Avalon::Time refreshTime = refreshDate.ToTime();
		if (updateTime.MSec() >= refreshTime.MSec())
		{
			refreshTime += Avalon::Time::SECS_OF_DAY * Avalon::Time::MSECS_OF_SEC;
		}

		if (curTime >= refreshTime.MSec())
		{
			reset = true;
		}

		if (reset)
		{
			SetTodayTimes(0);
			SetUpdateTime((UInt32)CURRENT_TIME.Sec());
		}
	}
	
	return m_TodayTimes;
}

void AccountChargeElem::SaveToDB()
{
	CLUpdateCommand* updateCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_charge_record", GetID(), true);
	GetDBData(updateCmd, true);
	if (updateCmd->GetDataNum() > 0)
	{
		CLRecordClient::Instance()->SendCommand(updateCmd);
	}
	else
	{
		CL_SAFE_DELETE(updateCmd);
	}

	ClearDirty();
}

void AccountChargeElem::InsertToDB()
{
	CLInsertCommand* insertCmd = CLRecordClient::Instance()->PrepareInsertCommand("t_charge_record", GetID());
	GetDBData(insertCmd, false);
	CLRecordClient::Instance()->SendCommand(insertCmd);

	ClearDirty();
}