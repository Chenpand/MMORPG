#include "GASConsoleCmdHandler.h"

#include <json/writer.h>
#include <CLRecordClient.h>
#include <CLSysNotify.h>
#include <CLConsoleProtocol.h>
#include <TimeUtil.h>
#include <CLDrawPrizeDataEntry.h>

#include "GASConsoleAgent.h"
#include "ActivityMgr.h"


void QueryActConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Array activitiesArray;

	ActivityMgr::Instance()->VisitOpActivities([&activitiesArray](OpActivityInfo* opAct)->bool{
		if (!opAct) return true;

		json::Object active;
		active["g"] = json::Number(opAct->id);
		active["tmp"] = json::Number(opAct->type);
		active["n"] = json::String(opAct->name);
		active["state"] = json::Number(opAct->status);
		active["pt"] = json::Number(opAct->prepareTime);
		active["st"] = json::Number(opAct->startTime);
		active["et"] = json::Number(opAct->endTime);

		activitiesArray.Insert(active);

		return true;
	});

	json::Object body;
	body["activities"] = activitiesArray;
	body["error"] = json::Boolean(false);

	GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void SetActConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	std::string activeStr = (json::String)jsonobj["activities"];
	if (activeStr.empty())
	{
		ErrorLogStream << "SetActConfigHandler failed: activities are empty!" << LogStream::eos;

		body["error"] = json::Boolean(false);
		GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	json::Array actives;

	try
	{
		std::istringstream is(activeStr);
		json::Reader::Read(actives, is);
	}
	catch (const json::Exception& e)
	{
		ErrorLogStream << "SetActConfigHandler failed, read activeStr failed, error msg:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (actives.Empty())
	{
		ErrorLogStream << "SetActConfigHandler failed: activities are empty!" << LogStream::eos;

		body["msg"] = json::String("activities are empty!");
		GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	json::Array::const_iterator itr = actives.Begin();
	for (; itr != actives.End(); ++itr)
	{
		json::Object active = *itr;

		try
		{
			UInt32 type = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["tmp"]);
			if (type != OAT_GAMBING) 	// 不是夺宝类型的
			{
				continue;
			}
		
			UInt32 opActId = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["g"]);
			UInt32 endTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["et"]);
			if (endTime > 0 && endTime < (UInt32)CURRENT_TIME.Sec())
			{
				ErrorLogStream << "OpActivity:" << opActId << " endTime:" << TimeUtil::TimestampToStandardFormat(endTime) << " reach current time." << LogStream::eos;
				continue;
			}

			std::string taskStr = (json::String)active["task"];
			if (!taskStr.empty())
			{
				json::Array tasks;
				std::istringstream is(taskStr);
				json::Reader::Read(tasks, is);

				UInt32 taskSortIdx = 0;
				json::Array::const_iterator itrt = tasks.Begin();
				for (; itrt != tasks.End(); ++itrt)
				{
					taskSortIdx++;
					json::Object taskObj = *itrt;

					OpActTaskData task;
					task.dataId = opActId * OPACT_ID_TRANS_VALUE + Avalon::StringUtil::ConvertToValue<UInt32>((json::String)taskObj["g"]);
					task.completeNum = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)taskObj["c"]);

					std::string rewardStr = (json::String)taskObj["b"];
					std::vector<std::string> strs;
					Avalon::StringUtil::Split(rewardStr, strs, "|");
					for (const auto& elem : strs)
					{
						std::vector<std::string> paramStr;
						Avalon::StringUtil::Split(elem, paramStr, ":");
						if (paramStr.size() != 2)
						{
							ErrorLogStream << "SetActConfigHandler failed: decode opAct(" << opActId << ")'s reward faield!" << LogStream::eos;

							body["msg"] = json::String("decode b error!");
							GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
							return;
						}

						ItemReward reward;
						reward.id = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[0].c_str());
						reward.num = Avalon::StringUtil::ConvertToValue<UInt32>(paramStr[1].c_str());
						task.rewards.push_back(reward);
					}

					json::Array taskVars = (json::Array)taskObj["v"];
					if (taskVars.Size() != 8)
					{
						ErrorLogStream << "SetActConfigHandler failed, activity(" << opActId << "," << type << ")'s task(" << task.dataId
							<< ")'s variables size(" << (UInt32)task.variables.size() << ") is error!" << LogStream::eos;

						body["msg"] = json::String("error v size!");
						GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
						return;
					}

					std::vector<std::string> strVec;
					json::Array::const_iterator itrv = taskVars.Begin();
					for (; itrv != taskVars.End(); ++itrv)
					{
						std::string str = (json::String)(*itrv);
						strVec.push_back(str);
					}

					if (!GambingItemDataEntryMgr::Instance()->AddDataEntryCustomer(opActId, task.dataId, taskSortIdx, task.rewards, strVec))
					{
						ErrorLogStream << "SetActConfigHandler failed, activity(" << opActId << "," << type << ")'s task(" << task.dataId
							<< ") decode v failed!" << LogStream::eos;

						body["msg"] = json::String("error v format!");
						GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
						return;
					}
				}
			}

			OpActivityInfo opAct;
			opAct.type = type;
			opAct.id = opActId;
			opAct.name = (json::String)active["n"];
			opAct.prepareTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["pt"]);
			opAct.startTime = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["st"]);
			opAct.endTime = endTime;
			opAct.playerLevelLimit = Avalon::StringUtil::ConvertToValue<UInt16>((json::String)active["pl"]);
			opAct.description = (json::String)active["desc"];
			opAct.ruleDesc = (json::String)active["r"];
			opAct.parm = Avalon::StringUtil::ConvertToValue<UInt32>((json::String)active["param"]);
			opAct.dataSource = OADS_FROM_GM;

			std::string parm2Str = (json::String)active["param2"];
			std::vector<std::string> strVec;
			Avalon::StringUtil::Split(parm2Str, strVec, "|");
			for (auto elem : strVec)
			{
				opAct.parm2.push_back(Avalon::StringUtil::ConvertToValue<UInt32>(elem));
			}

			// 设置活动
			ActivityMgr::Instance()->GmSetOpActivity(opAct);
		}
		catch (const json::Exception& e)
		{
			ErrorLogStream << "SetActConfigHandler failed, read active json failed, error msg:" << e.what() << "!" << LogStream::eos;

			body["msg"] = json::String("decode json failed!");
			GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}
	}

	body["error"] = json::Boolean(false);

	GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}

void DelActConfigHandler::operator()(UInt32 connid, json::Object& jsonobj)
{
	json::Object body;
	body["error"] = json::Boolean(true);

	std::string uidStr;
	try
	{
		uidStr = (json::String)jsonobj["g"];
	}
	catch (const json::Exception& e)
	{
		ErrorLogStream << "DelActConfigHandler failed:" << e.what() << "!" << LogStream::eos;

		body["msg"] = json::String("decode json failed!");
		GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	if (uidStr.empty())
	{
		ErrorLogStream << "DelActConfigHandler failed: uids are empty!" << LogStream::eos;

		body["msg"] = json::String("the param g, which value is empty!");
		GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
		return;
	}

	std::vector<std::string> uidStrs;
	Avalon::StringUtil::Split(uidStr, uidStrs, ",");
	for (auto elem : uidStrs)
	{
		UInt32 uid = Avalon::StringUtil::ConvertToValue<UInt32>(elem);
		if (!ActivityMgr::Instance()->GmCloseOpActivity(uid))
		{
			ErrorLogStream << "DelActConfigHandler failed, can not find opActivity(" << uid << ")!" << LogStream::eos;

			std::string errormsg("can not find activity ");
			errormsg.append(Avalon::StringUtil::ConvertToString<UInt32>(uid));
			body["msg"] = json::String(errormsg);
			GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
			return;
		}
	}

	body["error"] = json::Boolean(false);
	
	GASConsoleAgent::Instance()->SendResponse(connid, GetCmd(), GetSubCmd(), body);
}