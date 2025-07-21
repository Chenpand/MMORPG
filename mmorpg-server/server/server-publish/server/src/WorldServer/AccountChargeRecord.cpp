#include "AccountChargeRecord.h"
#include <CLRecordClient.h>

AcccountChargeRecord::AcccountChargeRecord()
{

}

AcccountChargeRecord::~AcccountChargeRecord()
{
	for (auto record : m_Records)
	{
		delete record;
	}
}

UInt32 AcccountChargeRecord::GetChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId)
{
	UInt32 times = 0;
	for (auto& record : m_Records)
	{
		if (record->GetMallType() == mallType && record->GetGoodsID() == goodsId && 
				(roleId == 0 || roleId == record->GetRoleID()))
		{
			times += record->GetTotalTimes();
		}
	}

	return times;
}

UInt32 AcccountChargeRecord::GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId)
{
	UInt32 times = 0;
	for (auto& record : m_Records)
	{
		if (record->GetMallType() == mallType && record->GetGoodsID() == goodsId &&
			(roleId == 0 || roleId == record->GetRoleID()))
		{
			times += record->GetTodayTimes();
		}
	}

	return times;
}

void AcccountChargeRecord::IncChargeTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times)
{
	auto record = FindRecord(mallType, goodsId, roleId);
	if (!record)
	{
		record = new AccountChargeElem();
		record->SetID(CLRecordClient::Instance()->GenGuid());
		record->SetMallType(mallType);
		record->SetGoodsID(goodsId);
		record->SetAccID(accid);
		record->SetRoleID(roleId);
		record->SetTodayTimes(0);
		
		record->InsertToDB();

		AddRecord(record);
	}

	record->SetTodayTimes(record->GetTodayTimes() + 1);
	record->SetTotalTimes(record->GetTotalTimes() + 1);
	record->SetUpdateTime((UInt32)CURRENT_TIME.Sec());

	record->SaveToDB();
}

AccountChargeElem* AcccountChargeRecord::FindRecord(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId)
{
	for (auto& record : m_Records)
	{
		if (record->GetMallType() == mallType && record->GetGoodsID() == goodsId && record->GetRoleID() == roleId)
		{
			return record;
		}
	}

	return NULL;
}

void AcccountChargeRecord::AddRecord(AccountChargeElem* record)
{
	m_Records.push_back(record);
}