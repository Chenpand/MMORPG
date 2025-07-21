#include "AbnormalTransactionRecord.h"

#include <CLRecordClient.h>

AbnormalTransactionRecord::AbnormalTransactionRecord()
{
	m_HandleStatus = 0;
	m_CreateTime = 0;
	m_Buyer = 0;
	m_Seller = 0;
	m_TransactionTime = 0;
	m_ItemUid = 0;
	m_ItemDataId = 0;
	m_ItemNum = 0;
	m_MoneyType = 0;
	m_TransationAmount = 0;
	m_FrozenAmount = 0;
	m_BackAmount = 0;
	m_FreezeObject = 0;
	m_FrozenMoneyBackWay = 0;
	m_IsFrozenPermanent = 0;
	m_TreasureFlag = 0;
	m_Reason = 0;
	m_MailNotifyType = 0;
	m_UnfrozenAmount = 0;
	m_RemovePlayerTransMoneyFlag = 0;
}

AbnormalTransactionRecord::~AbnormalTransactionRecord()
{

}

void AbnormalTransactionRecord::SetHandleStatus(AbnormalTransHandleStatus status)
{
	if (m_HandleStatus != (UInt8)status)
	{
		m_HandleStatus = (UInt8)status;
		SaveToDB();
	}
}

void AbnormalTransactionRecord::SetFrozeAmount(UInt32 amount)
{
	if (m_FrozenAmount != amount)
	{
		m_FrozenAmount = amount;
		SaveToDB();
	}
}

void AbnormalTransactionRecord::IncFrozeAmount(UInt32 amount) 
{ 
	m_FrozenAmount = m_FrozenAmount + amount; 
	SaveToDB();
}

void AbnormalTransactionRecord::IncBackAmount(UInt32 amount) 
{ 
	m_BackAmount = m_BackAmount + amount; 
	SaveToDB();
}

void AbnormalTransactionRecord::SetFrozenMoneyBackWay(UInt8 backWay)
{ 
	m_FrozenMoneyBackWay = backWay; 
}

bool AbnormalTransactionRecord::IsFrozenPermanent() const
{ 
	return m_IsFrozenPermanent > 0; 
}

bool AbnormalTransactionRecord::IsTreasure() const
{ 
	return m_TreasureFlag > 0; 
}

bool AbnormalTransactionRecord::NeedRemovePlayerTransMoney() const
{
	return m_RemovePlayerTransMoneyFlag == 0;
}

bool AbnormalTransactionRecord::CanUnfreezeByCharge()
{ 
	return (m_FrozenMoneyBackWay.GetDBValue() & (UInt8)ATFROZEN_MONEY_BACK_WAY_CHARGE) > 0; 
}

bool AbnormalTransactionRecord::CanUnFreezeByDailyTaskScore()
{ 
	return (m_FrozenMoneyBackWay.GetDBValue() & (UInt8)ATFROZEN_MONEY_BACK_WAY_DAILY_TASK_SCORE) > 0;
}

bool AbnormalTransactionRecord::IsOnlyUnfreezeByCharge()
{ 
	return !(m_FrozenMoneyBackWay.GetDBValue() ^ (UInt8)ATFROZEN_MONEY_BACK_WAY_CHARGE);
}

const std::string AbnormalTransactionRecord::LogStr() const
{
	std::string str = "Abnormal record(id=";
	str.append(Avalon::StringUtil::ConvertToString<ObjID_t>(GetID()))
		.append(", create time=")
		.append(Avalon::StringUtil::ConvertToString<UInt32>(GetCreateTime()))
		.append(")");
	return str;
}

void AbnormalTransactionRecord::SaveToDB()
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_abnormal_transaction", GetID(), true);
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