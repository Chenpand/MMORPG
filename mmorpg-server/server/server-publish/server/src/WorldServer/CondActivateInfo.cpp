#include "CondActivateInfo.h"

#include <CLRecordClient.h>

#include "WSPlayer.h"


CondActivateInfo::CondActivateInfo()
{
	m_AccId = 0;
	m_RoleId = 0;
	m_MallGoodsID = 0;
	m_ActivateStartTime = 0;
	m_ActivateEndTime = 0;
	m_RestNum = 0;
	m_ActivateState = 0;
	m_ActivateCond = 0;
	m_ActivateCount = 0;
	m_ActivateSubCond = 0;
	m_ActivateType = 0;
}

CondActivateInfo::~CondActivateInfo()
{

}

CondActivateInfo* CondActivateInfo::Create()
{
	CondActivateInfo* ptr = new CondActivateInfo;
	if (!ptr)
	{
		ErrorLogStream << "CondActivateInfo ptr is null!" << LogStream::eos;
	}

	return ptr;
}

void CondActivateInfo::AddActivateCount()
{
	UInt16 count = m_ActivateCount;
	count++;
	m_ActivateCount = count;
}

void CondActivateInfo::Destroy(CondActivateInfo*& ptr)
{
	CL_SAFE_DELETE(ptr);
}

void CondActivateInfo::UpdateToDB(WSPlayer* player, bool bFlush)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(player->GetSrcZoneId(), "t_mall_gift_pack", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void CondActivateInfo::InsertToDB(WSPlayer* player)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(player->GetSrcZoneId(), "t_mall_gift_pack", GetID());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}