#include "CreditPointRecordMgr.h"

#include <TimeUtil.h>
#include <CLRecordClient.h>
#include "Player.h"
#include "SceneSysNotify.h"

/**
*@brief 请求信用点券记录
*/
class SelectCreditPointRecordCallback : public CLRecordCallback
{
public:
	void OnFinished()
	{
		CreditPointRecordMgr::Instance()->LoadData(this);
	}

	void OnFailed(UInt32 errorcode)
	{
		ErrorLogStream << "load table t_credit_point_record error!" << LogStream::eos;
	}
};


CreditPointRecordMgr::CreditPointRecordMgr()
{

}

CreditPointRecordMgr::~CreditPointRecordMgr()
{

}

void CreditPointRecordMgr::Init()
{
	CLSelectCommand* creditPoint = CLRecordClient::Instance()->PrepareSelectCommand("t_credit_point_record");
	CLRecordClient::Instance()->SendCommand(creditPoint, new SelectCreditPointRecordCallback());
}

void CreditPointRecordMgr::Final()
{
	for (auto& it : m_AccCreditPointMap)
	{
		if (it.second != NULL)
		{
			CL_SAFE_DELETE(it.second);
		}
	}

	m_AccCreditPointMap.clear();
}

void CreditPointRecordMgr::OnTick(const Avalon::Time& now, Player* player)
{
	if (NULL == player) return;

	// 每个月N号6点转化一定数量的信用点券
	// 本月该账号是否已经转化
	UInt32 curTransferTime = (UInt32)getMonthLogicBeginTime(now) + GetTransferDate();
	UInt32 lastTransferTime = OpActivityRecordMgr::Instance()->GetRecordValue(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_TRANSFER_TIME,
		OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, player->GetAccID(), 0);
	if (lastTransferTime >= curTransferTime)
	{
		// 上次转化的时间大于本次要转的时间，转换日期往前调时间了，返回
		return;
	}

	UInt32 startTransferTime = 0;
	std::list<UInt32> transferTimeList;
	if (UInt32(now.Sec()) < curTransferTime)
	{
		if (lastTransferTime == 0)
		{
			// 上次没转过
			UInt32 lastOfflineTime = player->GetAccOfflineTime();
			Avalon::Date lastOfflineDate((UInt64)lastOfflineTime * 1000);
			if (CURRENT_DATE.Month() == lastOfflineDate.Month() && CURRENT_DATE.Year() == lastOfflineDate.Year())
			{
				return;	// 上次没转，上次的离线时间是本月，等本月转化时间到了再转吧
			}
			else
			{
				// 上次的离线时间大于上个月要转的时间，等本月要转的时间到了才转
				if (lastOfflineTime > (UInt32)getLastMonthLogicBeginTime(now.MSec()) + GetTransferDate())
				{
					return;
				}

				startTransferTime = (UInt32)getMonthLogicBeginTime((UInt64)lastOfflineTime * 1000) + GetTransferDate();
				if (lastOfflineTime > (UInt32)getMonthLogicBeginTime((UInt64)lastOfflineTime * 1000) + GetTransferDate())
				{
					startTransferTime = (UInt32)getLastMonthLogicBeginTime((UInt64)lastOfflineTime * 1000) + GetTransferDate();
				}
			}
		}
		else
		{
			Avalon::Date lastDate((UInt64)lastTransferTime * 1000);
			if (CURRENT_DATE.Month() == lastDate.Month() && CURRENT_DATE.Year() == lastDate.Year())
			{
				return;	// 本月已经转过,但本次要转时间未到
			}
			else
			{
				// 上个月转过, 等着本月转
				if ((UInt32)getNextMonthLogicBeginTime((UInt64)lastTransferTime * 1000) + GetTransferDate() == curTransferTime)
				{
					return;
				}
				else
				{
					startTransferTime = lastTransferTime;
				}
			}
		}

		UInt32 lastMonthTime = (UInt32)getLastMonthLogicBeginTime((UInt64)curTransferTime * 1000) + GetTransferDate();
		OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_TRANSFER_TIME,
			OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, player->GetAccID(), 0, int(lastMonthTime));
	}
	else
	{
		if (lastTransferTime == 0)
		{
			UInt32 lastOfflineTime = player->GetAccOfflineTime();
			startTransferTime = (UInt32)getMonthLogicBeginTime((UInt64)lastOfflineTime * 1000) + GetTransferDate();

			if (lastOfflineTime < startTransferTime)
				startTransferTime = (UInt32)getLastMonthLogicBeginTime((UInt64)lastOfflineTime * 1000) + GetTransferDate();
		}
		else
		{

			Avalon::Date lastDate((UInt64)lastTransferTime * 1000);
			if (CURRENT_DATE.Month() == lastDate.Month() && CURRENT_DATE.Year() == lastDate.Year())
			{
				startTransferTime = (UInt32)getLastMonthLogicBeginTime((UInt64)lastTransferTime * 1000) + GetTransferDate();
			}
			else
			{
				startTransferTime = lastTransferTime;
			}
		}

		OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_TRANSFER_TIME,
			OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, player->GetAccID(), 0, int(curTransferTime));
	}
	
	InfoLogStream << PLAYERINFO(player) << " credit point transfer start time:" << startTransferTime << LogStream::eos;

	if (player->GetCreditPoint() == 0)
	{
		return;
	}

	CreditPointRecord* record = FindCreditPointRecord(player->GetAccID());
	if (NULL == record)
	{
		return;
	}

	// 看跨月能转出几个
	while (startTransferTime < curTransferTime)
	{
		startTransferTime = (UInt32)getNextMonthLogicBeginTime((UInt64)startTransferTime * 1000) + GetTransferDate();
		if (startTransferTime > UInt32(now.Sec()))
			break;
		
		transferTimeList.push_back(startTransferTime);
	}

	for(auto iter : transferTimeList)
	{
		record->TransferToPoint(player, iter);
		InfoLogStream << PLAYERINFO(player) << " transfer credit point time:" << iter << LogStream::eos;
	}
}

void CreditPointRecordMgr::HandlerCreditPointRecordReq(Player* player)
{
	if (NULL == player) return;

	CreditPointRecord* record = FindCreditPointRecord(player->GetAccID());
	if (NULL == record)
	{
		CLProtocol::SceneCreditPointRecordRes res;
		player->SendProtocol(res);
		return;
	}

	record->CreditPointRecordReq(player);
}

void CreditPointRecordMgr::ObtainCreditPoint(UInt32 accId, UInt32 obtainNum, UInt64 orderId)
{
	if (obtainNum == 0 || accId == 0) return;
	CreditPointRecord* record = FindCreditPointRecord(accId);
	if (NULL == record)
	{
		record = new CreditPointRecord;
		record->SetAccId(accId);
		m_AccCreditPointMap[accId] = record;
	}

	record->ObtainCreditPoint(obtainNum, orderId);
}

CreditPointRecord* CreditPointRecordMgr::FindCreditPointRecord(UInt32 accId)
{
	auto iter = m_AccCreditPointMap.find(accId);
	return iter != m_AccCreditPointMap.end() ? iter->second : NULL;
}

void CreditPointRecordMgr::AddCreditPointRecord(UInt32 accId, CreditPointRecordEntry& entry)
{
	CreditPointRecord* record = FindCreditPointRecord(accId);
	if (NULL == record)
	{
		record = new CreditPointRecord;
		record->SetAccId(accId);
		m_AccCreditPointMap[accId] = record;
	}

	record->LoadAddRecord(entry);
}

void CreditPointRecordMgr::GMTransferCreditPoint(UInt32 accId)
{
	InfoLogStream << "accId" << accId << " GM Transfer Credit Point!" << LogStream::eos;

	OpActivityRecordMgr::Instance()->SetRecord(OP_ACTIVITY_BEHAVIOR_CREDIT_POINT_TRANSFER_TIME,
		OP_ACTIVITY_BEHAVIOR_OWNER_ACCOUNT, accId, 0, 0);
}

bool CreditPointRecordMgr::LoadData(CLRecordCallback* callback)
{
	if (NULL == callback) return false;
	while (callback->NextRow())
	{
		UInt32 accId = callback->GetData("acc_id");

		CreditPointRecordEntry record;
		record.SetID(callback->GetKey());
		record.getTime = callback->GetData("get_time");
		record.totalNum = callback->GetData("total_num");
		record.todayTransfer = callback->GetData("today_tansfer");

		std::string orderStr(callback->GetData("order_list").GetString());
		std::vector<std::string> orderVec;
		Avalon::StringUtil::Split(orderStr, orderVec, "|");
		for (auto& it : orderVec)
		{
			std::vector<std::string> param;
			Avalon::StringUtil::Split(it, param, ",");

			if (param.size() != 3)
			{
				ErrorLogStream << "accId:" << accId << " credit point record decode error!" << orderStr << LogStream::eos;
				continue;
			}

			CreditPointOrder order;
			order.orderId = Avalon::StringUtil::ConvertToValue<UInt64>(param[0]);
			order.getNum = Avalon::StringUtil::ConvertToValue<UInt32>(param[1]);
			order.getTime = Avalon::StringUtil::ConvertToValue<UInt32>(param[2]);
			record.orderVec.push_back(order);
		}

		AddCreditPointRecord(accId, record);
	}

	return true;
}

void CreditPointRecordMgr::UpdateToDB(CreditPointRecordEntry& record)
{
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_credit_point_record", record.GetID(), false);
	cmd->PutData("total_num", record.totalNum);
	cmd->PutData("today_tansfer", record.todayTransfer);

	std::string str;
	record.EncodeOrderVec(str);
	cmd->PutData("order_list", str);

	CLRecordClient::Instance()->SendCommand(cmd);
}

void CreditPointRecordMgr::InsertToDB(UInt32 accId, CreditPointRecordEntry& record)
{
	CLInsertCommand* cmd = CLRecordClient::Instance()->PrepareInsertCommand("t_credit_point_record", record.GetID());
	cmd->PutData("acc_id", accId);
	cmd->PutData("get_time", record.getTime);
	cmd->PutData("total_num", record.totalNum);
	cmd->PutData("today_tansfer", record.todayTransfer);

	std::string str;
	record.EncodeOrderVec(str);
	cmd->PutData("order_list", str);

	CLRecordClient::Instance()->SendCommand(cmd);
}

void CreditPointRecordMgr::DelToDB(UInt64 guid)
{
	CLDeleteCommand* cmd = CLRecordClient::Instance()->PrepareDeleteCommand("t_credit_point_record", guid);
	CLRecordClient::Instance()->SendCommand(cmd);
}