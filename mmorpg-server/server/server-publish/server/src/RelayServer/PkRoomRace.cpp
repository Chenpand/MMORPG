#include "PkRoomRace.h"

PkRoomRace::PkRoomRace(RaceType raceType)
:PvPRace(raceType, RACE_PK_ROOM)
{

}

PkRoomRace::~PkRoomRace()
{

}

void PkRoomRace::CheckRaceEnd(EGameSessionDismissType reason)
{
	std::vector<PkRaceEndInfo> akRaceEndInfo;
	for (auto& gamer : m_gamers)
	{
		auto endInfo = gamer.GetPkRaceEndInfo();
		if (!endInfo.empty())
		{
			akRaceEndInfo.push_back(endInfo);
		}
	}

	bool invalidGame = false;
	// ���еĽ��㶼Ӧ��һ��
	for (UInt32 i = 0; i < akRaceEndInfo.size(); i++)
	{
		for (UInt32 j = i + 1; j < akRaceEndInfo.size(); j++)
		{
			if (akRaceEndInfo[i] != akRaceEndInfo[j])
			{
				invalidGame = true;
				break;
			}
		}
	}

	// �����¼ʱ�������
	if (akRaceEndInfo.empty())
	{
		std::set<RoomSlotGroup> loginCamps;
		for (auto& gamer : m_gamers)
		{
			if (gamer.GetStatus() != GAMER_STATUS_WAIT_LOAD && gamer.GetStatus() != GAMER_STATUS_WAIT_LOGIN)
			{
				loginCamps.insert(_GetCampBySeat(gamer.GetPos()));
				DebugLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") have login." << LogStream::eos;
			}
			else
			{
				DebugLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") have not login." << LogStream::eos;
			}
		}

		// ���߶����˵������ˣ�������Ч��
		if (loginCamps.size() == ROOM_SLOT_GROUP_NUM - 1)
		{
			invalidGame = true;
		}
		else
		{
			auto onlineCamp = loginCamps.empty() ? ROOM_SLOT_GROUP_INVALID : *loginCamps.begin();

			// �����û�ǿ�Ƹĳ�ʧ��
			PkRaceEndInfo endInfo;
			endInfo.gamesessionId = GetID();
			for (auto& gamer : m_gamers)
			{
				PkPlayerRaceEndInfo result;
				result.roleId = gamer.GetID();
				result.pos = gamer.GetPos();
				result.remainHp = HP_MP_FULL_VALUE;
				result.remainMp = HP_MP_FULL_VALUE;
				result.damagePercent = 0;
				if (onlineCamp == ROOM_SLOT_GROUP_INVALID)
				{
					result.resultFlag = resultBitmask_invalid;
				}
				else if (_GetCampBySeat(gamer.GetPos()) == onlineCamp)
				{
					result.resultFlag = resultBitmask_win;
					DebugLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") win." << LogStream::eos;
				}
				else
				{
					result.resultFlag = resultBitmask_lose;
					DebugLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") lose." << LogStream::eos;
				}
				endInfo.results.push_back(result);
			}

			akRaceEndInfo.push_back(endInfo);
		}
	}

	if (invalidGame || akRaceEndInfo.empty())
	{
		PkRaceEndInfo endInfo;
		endInfo.gamesessionId = GetID();

		for (auto& gamer : m_gamers)
		{
			gamer.SetRaceResult(resultBitmask_invalid);

			PkPlayerRaceEndInfo result;
			result.roleId = gamer.GetID();
			result.pos = gamer.GetPos();
			result.remainHp = HP_MP_FULL_VALUE;
			result.remainMp = HP_MP_FULL_VALUE;
			result.resultFlag = resultBitmask_invalid;
			result.damagePercent = 0;

			DebugLogStream << "race(" << GetID() << ") gamer(" << gamer.GetID() << ") invalid." << LogStream::eos;

			endInfo.results.push_back(result);
		}
		SetPkRaceEndInfo(endInfo);
	}
	else
	{
		for (auto& info : akRaceEndInfo[0].results)
		{
			auto gamer = GetGamerByRoleID(info.roleId);
			if (gamer)
			{
				gamer->SetRaceResult(info.resultFlag);
			}
		}
		SetPkRaceEndInfo(akRaceEndInfo[0]);
	}
}

void PkRoomRace::OnGamerReconnectTimeout(ObjID_t id)
{
	Race::OnGamerReconnectTimeout(id);
	if (!this->IsAllGamerLeave())
	{
		return;
	}

	Dismiss(EGameSessionDismissType::gamesessionDismissType_offline);
}

bool PkRoomRace::_IsNeedDismiss()
{
	/*// ��һ����Ҷ������ˣ��ǾͿ��Խ�����
	if (_GetGamerNumByCamp(ROOM_SLOT_GROUP_RED) == 0 || _GetGamerNumByCamp(ROOM_SLOT_GROUP_BLUE) == 0)
	{
		return true;
	}*/

	if (IsAllGamerLeave())
	{
		return true;
	}

	return false;
}

RoomSlotGroup PkRoomRace::_GetCampBySeat(UInt8 seat)
{
	return GetRoomSlotGroup(seat);
}

UInt32 PkRoomRace::_GetGamerNumByCamp(RoomSlotGroup camp, bool mustOnline)
{
	UInt32 num = 0;

	for (auto& gamer : m_gamers)
	{
		if (_GetCampBySeat(gamer.GetPos()) != camp)
		{
			continue;
		}

		if (mustOnline && gamer.IsOffline())
		{
			continue;
		}

		num++;
	}

	return num;
}

bool PkRoomRace::_GetReplayInfo(ReplayInfo& info)
{
	return PvPRace::_GetReplayInfo(info);
}