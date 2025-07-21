#include "CreditPointRecord.h"

#include <TimeUtil.h>
#include <CLGoldConsignmentValueDataEntry.h>
#include "Player.h"
#include "SceneSysNotify.h"
#include "CreditPointRecordMgr.h"
#include "ActiveRecordMgr.h"

CreditPointRecord::CreditPointRecord()
{
	m_AccId = 0;
}

CreditPointRecord::~CreditPointRecord()
{

}

void CreditPointRecord::TransferToPoint(Player* player, UInt32 transferTime)
{
	if (NULL == player || m_CreditPointMap.empty() || player->GetCreditPoint() == 0)
	{
		return;
	}

	// 可转化上限
	UInt32 monthTransferLimit = PlayerDataEntryMgr::Instance()->GetCreditPointTransferLimit(player->GetRoleMaxLevel());
	UInt32 vipMonthTransferLimit = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_PRIVILEGE_CREDIT_POINT_TRANSFER_LIMIT,
		player->GetVipLvl());

	if (vipMonthTransferLimit > monthTransferLimit)
	{
		monthTransferLimit = vipMonthTransferLimit;
	}

	UInt32 transferNum = 0;
	std::string reason = GetReason(SOURCE_CREDIT_POINT_TRANSFER);
	if (player->GetCreditPoint() < monthTransferLimit)
	{
		transferNum = player->GetCreditPoint();
		player->AddPoint(reason.c_str(), player->GetCreditPoint());
		player->RemoveCreditPoint(reason.c_str(), player->GetCreditPoint());
	}
	else
	{
		transferNum = monthTransferLimit;
		player->AddPoint(reason.c_str(), monthTransferLimit);
		player->RemoveCreditPoint(reason.c_str(), monthTransferLimit);
	}

	InfoLogStream << PLAYERINFO(player) << " transfer credit point time:" << transferTime << " transferNum:" << transferNum 
		<< " creditPointNum:" << player->GetCreditPoint() << LogStream::eos;

	// 修改今日转化
	auto today = m_CreditPointMap.find(transferTime);
	if (today != m_CreditPointMap.end())
	{
		today->second.todayTransfer = transferNum;
		CreditPointRecordMgr::Instance()->UpdateToDB(today->second);
	}
	else
	{
		CreditPointRecordEntry record;
		record.SetID(CLRecordClient::Instance()->GenGuid());
		record.getTime = transferTime;
		record.todayTransfer = transferNum;
		m_CreditPointMap[transferTime] = record;
		CreditPointRecordMgr::Instance()->InsertToDB(player->GetAccID(), record);
	}

	// 邮件提示
	std::string title = SysNotifyMgr::Instance()->MakeString(10077);
	std::string sender = SysNotifyMgr::Instance()->MakeString(10075);
	std::string content = SysNotifyMgr::Instance()->MakeString(10078, transferNum, transferNum,
		TimeUtil::TimestampToStandardFormatNoHour(transferTime));
	player->SendSysMail(sender.c_str(), title.c_str(), content.c_str(), reason.c_str(), NULL);
}

void CreditPointRecord::CreditPointRecordReq(Player* player)
{
	if (NULL == player) return;

	CLProtocol::SceneCreditPointRecordRes res;

	UInt32 transferDay = GoldConsignmentValueDataEntryMgr::Instance()->GetValue(GC_VALUE_CREDIT_POINT_SHOW_DAY);
	UInt32 transferTime = transferDay * 24 * 3600;
	UInt32 todayStartTime = UInt32(GetDayBeginTime2(CURRENT_TIME) / 1000);

	for (auto& it : m_CreditPointMap)
	{
		if (todayStartTime > it.first + transferTime)
			continue;

		res.recordVec.push_back(it.second);
	}

	player->SendProtocol(res);
}

void CreditPointRecord::ObtainCreditPoint(UInt32 obtainNum, UInt64 orderId)
{
	if (obtainNum == 0) return;

	InfoLogStream << "accId:" << GetAccId() << " obtain credit point:" << obtainNum << " orderId:" << orderId << LogStream::eos;

	CreditPointOrder order;
	order.orderId = orderId;
	order.getNum = obtainNum;
	order.getTime = UInt32(CURRENT_TIME.Sec());

	UInt32 todayStartTime = UInt32(GetDayBeginTime2(CURRENT_TIME) / 1000);
	auto iter = m_CreditPointMap.find(todayStartTime);
	if (iter == m_CreditPointMap.end())
	{
		CreditPointRecordEntry record;
		record.SetID(CLRecordClient::Instance()->GenGuid());
		record.getTime = todayStartTime;
		record.totalNum = obtainNum;
		record.orderVec.push_back(order);
		m_CreditPointMap[todayStartTime] = record;

		CreditPointRecordMgr::Instance()->InsertToDB(GetAccId(), record);
	}
	else
	{
		CreditPointRecordEntry& record = iter->second;
		record.totalNum += obtainNum;

		// 最多存50条
		if (record.orderVec.size() < 50)
		{
			record.orderVec.push_back(order);
		}
		else
		{
			ErrorLogStream << "add credit point order failed! accId:" << GetAccId() << " orderId:" << orderId << " num:" << obtainNum << LogStream::eos;
		}

		CreditPointRecordMgr::Instance()->UpdateToDB(record);
	}
}

void CreditPointRecord::LoadAddRecord(CreditPointRecordEntry& record)
{
	auto iter = m_CreditPointMap.find(record.getTime);
	if (iter == m_CreditPointMap.end())
	{
		m_CreditPointMap[record.getTime] = record;
		return;
	}

	if (record.GetID() == iter->second.GetID())
	{
		return;
	}

	InfoLogStream << "accId:" << GetAccId() << " merge server record_1:" << record.LogStr() << " record_2:" << record.LogStr() << LogStream::eos;

	// 合服处理
	iter->second.totalNum += record.totalNum;
	iter->second.todayTransfer += record.todayTransfer;

	UInt32 n = 0;
	for (auto& it : record.orderVec)
	{
		// 最多合并50条
		if (n > 50)
		{
			ErrorLogStream << "accId:" << GetAccId() << " merge credit point record over:" << record.LogStr() << " orderId:" << it.orderId << LogStream::eos;
			continue;
		}

		iter->second.orderVec.push_back(it);
		++n;
	}

	// 更新一个
	CreditPointRecordMgr::Instance()->UpdateToDB(iter->second);

	// 删除另一个
	CreditPointRecordMgr::Instance()->DelToDB(record.GetID());
}

