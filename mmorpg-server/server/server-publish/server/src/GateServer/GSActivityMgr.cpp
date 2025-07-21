#include "GSActivityMgr.h"
#include <CLSystemValueDataEntry.h>

GSActivityMgr::GSActivityMgr()
{
	m_Timer.SetInterval(Avalon::Time::MSECS_OF_SEC);
}

GSActivityMgr::~GSActivityMgr()
{

}

void GSActivityMgr::OnTick(const Avalon::Time& now)
{
	if (m_Timer.IsTimeout(now))
	{
		CountAppointmentOccus();
	}
}

void GSActivityMgr::OnLoadOpActData(OpActRegMap& activeData)
{
	for (auto elem : activeData)
	{
		auto actId = elem.first;

		auto itr = m_opRegInfos.find(actId);
		if (itr != m_opRegInfos.end())
		{
			auto pOpAct = itr->second;
			if (!pOpAct) continue;

			*pOpAct = elem.second;
		}
		else
		{
			auto pOpAct = new OpActivityRegInfo(elem.second);
			m_opRegInfos[actId] = pOpAct;
		}
	}

	CountAppointmentOccus();
}

void GSActivityMgr::OnOpActStateChange(OpActivityRegInfo& info)
{
	if (m_opRegInfos.find(info.dataId) == m_opRegInfos.end())
	{
		auto active = new OpActivityRegInfo();
		*active = info;
		m_opRegInfos[info.dataId] = active;
	}
	else
	{
		*m_opRegInfos[info.dataId] = info;
	}
	

	if (info.tmpType == OAT_APPOINTMENT_OCCU)
	{
		CountAppointmentOccus();
	}
}

std::vector<OpActivityRegInfo*> GSActivityMgr::GetOpActDataByTmpType(OpActivityType type, bool inActivity) const
{
	std::vector<OpActivityRegInfo*> infoes;

	for (auto& itr : m_opRegInfos)
	{
		auto& info = itr.second;
		if (!info)
		{
			continue;
		}

		if (info->tmpType != type)
		{
			continue;
		}

		if (inActivity && info->state != AS_IN)
		{
			continue;
		}

		infoes.push_back(info);
	}

	return infoes;
}

void GSActivityMgr::CountAppointmentOccus()
{
	auto activitys = GetOpActDataByTmpType(OAT_APPOINTMENT_OCCU);
	
	std::vector<UInt8> occus;
	for (auto& activity : activitys)
	{
		if (!activity)
		{
			continue;
		}

		for (auto occu : activity->parm2)
		{
			if (std::find(occus.begin(), occus.end(), occu) == occus.end())
			{
				occus.push_back(occu);
			}
		}
	}

	m_AppointmentOccus = occus;
}


bool GSActivityMgr::IsOccuInAppointmentOccuActivity(UInt8 occu, UInt32 createTime) const
{
	if (!IsAppointmentOccu(occu))
	{
		return false;
	}

	auto activitys = GetOpActDataByTmpType(OAT_APPOINTMENT_OCCU);

	std::vector<UInt8> occus;
	for (auto& activity : activitys)
	{
		if (!activity)
		{
			continue;
		}

		if (createTime < activity->startTime || createTime > activity->endTime)
		{
			continue;
		}

		if (std::find(activity->parm2.begin(), activity->parm2.end(), occu) == activity->parm2.end())
		{
			continue;
		}

		return true;
	}

	return false;
}

UInt32 GSActivityMgr::GetMaxAppointmentRoleNum()
{
	return SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_APPOINTMENT_MAX_ROLE);
}