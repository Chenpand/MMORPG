#include "GambingRecord.h"

#include <CLRecordClient.h>

GambingRecord::GambingRecord()
{
	m_OwnerSourceCenterNode = 0;
	m_OwnerSourceAdminNode = 0;
	m_GambingItemId = 0;
	m_GambingItemNum = 0;
	m_ItemDataId = 0;
	m_GroupId = 0;
	m_InvestCurrencyId = 0;
	m_InvestCurrencyNum = 0;
	m_InvestCopies = 0;
	m_GambingWeight = 0;
	m_CreateTimestamp = 0;
}

GambingRecord::~GambingRecord()
{

}

std::string GambingRecord::CalculateGambingRate(UInt32 baseValue)
{
	if (baseValue == 0)
	{
		ErrorLogStream << "Base value is zero!" << LogStream::eos;
		return Avalon::StringUtil::ConvertToString<UInt32>(m_InvestCopies);
	}
	else
	{
		float rate = (float)m_InvestCopies / (float)baseValue * 100;

		std::string rateStr = Avalon::StringUtil::ConvertToString<float>(rate);

		std::string::size_type index = rateStr.find_first_of('.');
		if (index == std::string::npos)
		{
			ErrorLogStream << "Float value(" << rateStr << ") error!" << LogStream::eos;
			return Avalon::StringUtil::ConvertToString<UInt32>(m_InvestCopies);
		}

		return rateStr.substr(0, index) + rateStr.substr(index, 3);
	}
}

void GambingRecord::UpdateToDB()
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gambing_record", GetID(), true);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GambingRecord::InsertToDB()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gambing_record", GetID());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

GambingLotteryRecord::GambingLotteryRecord()
{
	m_GambingItemId = 0;
	m_GambingItemNum = 0;
	m_ItemDataId = 0;
	m_GroupId = 0;
	m_SoldOutTimeSec = 0;
	m_LotteryTimeSec = 0;
	m_CreateTimeSec = 0;
}

GambingLotteryRecord::~GambingLotteryRecord()
{

}

void GambingLotteryRecord::UpdateToDB(bool bFlush)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_gambing_lottery_record", GetID(), bFlush);
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}

void GambingLotteryRecord::InsertToDB()
{
	SetID(CLRecordClient::Instance()->GenGuid());
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_gambing_lottery_record", GetID());
	GetDBData(cmd, true);
	CLRecordClient::Instance()->SendCommand(cmd);
}