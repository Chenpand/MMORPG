#include "SSConsoleCmdHandler.h"

#include <CLRecordClient.h>
#include "SSConsoleAgent.h"
#include "PlayerMgr.h"


void AbnormalTransQueryHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	ObjID_t roleid = 0;

	try
	{
		roleid = Avalon::StringUtil::ConvertToValue<ObjID_t>((json::String)jsonobj["roleid"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "AbnormalTransQueryHandler failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (roleid == 0)
	{
		GameErrorLogStream("Console") << "AbnormalTransQueryHandler failed, error roleid!" << LogStream::eos;

		body["msg"] = json::String("roleid is zero!");
		SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	auto player = PlayerMgr::Instance()->FindNormalPlayer(roleid);
	if (!player)
	{
		GameErrorLogStream("Console") << "AbnormalTransQueryHandler failed, error roleid!" << LogStream::eos;

		body["msg"] = json::String("player offline!");
		SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	UInt32 frozenAmount = 0;
	UInt32 unFrozenAmount = 0;
	UInt32 neverBackAmount = 0;

	player->GetAbnormalTransactionMgr().VisitAbnormalRecord([&frozenAmount, &unFrozenAmount, &neverBackAmount](AbnormalTransactionRecord* record)->bool{
		if (record != NULL)
		{
			if (record->GetHandleStatus() == ATHS_FROZEN_TAIL)
			{
				frozenAmount += record->GetFrozeAmount();
				unFrozenAmount += record->GetBackAmount();
			}

			if (record->GetHandleStatus() == ATHS_NEVER_BACK_MONEY)
			{
				if (record->GetFrozeAmount() >= record->GetBackAmount())
				{
					neverBackAmount += record->GetFrozeAmount() - record->GetBackAmount();
				}
				else
				{
					ErrorLogStream << record->LogStr() << " froze amount(" << record->GetFrozeAmount() << ") is less than back amount(" << record->GetBackAmount() << ")!" << LogStream::eos;
				}
			}
		}
		return true;
	});

	body["error"] = json::Boolean(false);

	// 总冻结金额
	body["fa"] = json::Number(frozenAmount);
	// 已解冻金额
	body["ufa"] = json::Number(unFrozenAmount);
	// 剩余解冻金额
	body["rfa"] = json::Number(0);
	if (frozenAmount > unFrozenAmount)
	{
		body["rfa"] = json::Number(frozenAmount - unFrozenAmount);
	}
	// 解冻时间
	body["dl"] = json::String(TimeUtil::TimestampToStandardFormatCustomer(player->GetAbnormalTransactionMgr().GetTailDeadline()));
	// 系统扣除金额
	body["nba"] = json::Number(neverBackAmount);

	SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void AbnormalTransUnfreezeHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	ObjID_t roleid = 0;

	try
	{
		roleid = Avalon::StringUtil::ConvertToValue<ObjID_t>((json::String)jsonobj["roleid"]);
	}
	catch (const json::Exception& e)
	{
		GameErrorLogStream("Console") << "AbnormalTransUnfreezeHandler failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (roleid == 0)
	{
		GameErrorLogStream("Console") << "AbnormalTransUnfreezeHandler failed, error roleid!" << LogStream::eos;

		body["msg"] = json::String("roleid is zero!");
		SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	auto player = PlayerMgr::Instance()->FindNormalPlayer(roleid);
	if (!player)
	{
		GameErrorLogStream("Console") << "AbnormalTransUnfreezeHandler failed, error roleid!" << LogStream::eos;

		body["msg"] = json::String("player offline!");
		SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	player->GetAbnormalTransactionMgr().OnAbnormalTransUnfreezeReq();

	body["error"] = json::Boolean(false);
	SSConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}