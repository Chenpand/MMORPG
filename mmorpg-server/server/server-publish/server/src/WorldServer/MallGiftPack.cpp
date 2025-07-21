#include "MallGiftPack.h"
#include <CLRecordClient.h>


MallGiftPack::MallGiftPack()
{

}

MallGiftPack::~MallGiftPack()
{

}

MallGiftPack* MallGiftPack::Create()
{
	MallGiftPack* mallGiftPack = new MallGiftPack;

	return mallGiftPack;
}

void MallGiftPack::Destroy(MallGiftPack*& item)
{
	CL_SAFE_DELETE(item);
}

void MallGiftPack::SetActivateCounter(UInt16 actCounter)
{
	UInt16 conter = m_ActivateCounter;
	conter += actCounter;
	m_ActivateCounter = conter;
}

bool MallGiftPack::IsExpired(UInt32 curTime)
{
	if ((GetActivateStartTime() > 0 && curTime < GetActivateStartTime()) ||
		(GetActivateEndTime() > 0 && curTime > GetActivateEndTime()))
	{
		return true;
	}

	if (GetRestNum() == 0)
	{
		return true;
	}

	return false;
}

void MallGiftPack::UpdateToDB(WSPlayer* player, bool bFlush)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(player->GetSrcZoneId(), "t_mall_gift_pack", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void MallGiftPack::InsertProperty(WSPlayer* player)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand(player->GetSrcZoneId(), "t_mall_gift_pack", GetID());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}