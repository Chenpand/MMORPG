#include "MallItem.h"

#include <CLRecordClient.h>
#include "WSPlayerMgr.h"


MallItem::MallItem()
{
	m_MallItemId = 0;
	m_RoleId = 0;
	m_CreateTime = 0;
	m_StartTime = 0;
	m_EndTime = 0;
}

MallItem::~MallItem()
{

}

bool MallItem::IsOnSale(UInt32 limitTime)
{
	UInt64 tmpVar = (UInt64)limitTime * SEC_TO_MSEC;
	if ((GetStartTime() > 0 && tmpVar < GetStartTime()) ||
		(GetEndTime() > 0 && tmpVar > GetEndTime()))
	{
		return false;
	}
	return true;
}

void MallItem::CalcEndTime(const std::string& interval)
{
	UInt32 tmpVar = Avalon::StringUtil::ConvertToValue<UInt32>(interval);
	UInt64 endTime = GetStartTime() + tmpVar * HOUR_TO_SEC * SEC_TO_MSEC;
	SetEndTime(endTime);
}

void MallItem::TransformMallItem(MallItemInfo& mallItem)
{
	mallItem.startTime = (UInt32)(GetStartTime() / SEC_TO_MSEC);
	mallItem.endTime = (UInt32)(GetEndTime() / SEC_TO_MSEC);
}

const std::string MallItem::LogStr()
{
	std::ostringstream oss;
	oss << " player(" << m_RoleId << ") mall item(" << GetID() << "," << GetMallItemId() << ")";
	return oss.str();
}

void MallItem::SaveToDB()
{
	if (GetID() == 0)
	{
		SetID(CLRecordClient::Instance()->GenGuid());
		SetCreateTime(CURRENT_TIME.MSec());
		auto cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_mall_item", GetID());
		GetDBData(cmd, false);
		CLRecordClient::Instance()->SendCommand(cmd);
	}
	else
	{
		auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_mall_item", GetID(), true);
		GetDBData(cmd, true);
		if (cmd->GetDataNum() > 0)
		{
			CLRecordClient::Instance()->SendCommand(cmd);
		}
		else
		{
			CL_SAFE_DELETE(cmd);
		}
	}
}