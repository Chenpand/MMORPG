#pragma once

#include <CLDefine.h>
#include <AvalonSingleton.h>

class BSBaseRace;
class BSPlayer;

class BSRaceMgr : public Avalon::Singleton<BSRaceMgr>
{
public:
	BSRaceMgr();
	~BSRaceMgr();

public:
	BSBaseRace* CreateRace(UInt8 raceType);
	void DestoryRace(BSBaseRace* race);

	ObjID_t	GenRaceId();

	void AddRace(BSBaseRace* race);
	BSBaseRace* FindRace(ObjID_t raceId);

	template<class T>
	T* FindRaceWithType(ObjID_t id)
	{
		auto race = FindRace(id);
		if (!race)
		{
			return NULL;
		}

		return dynamic_cast<T*>(race);
	}

	//Ö»ÒÆ³ý£¬²»Ïú»Ù
	void RemoveRace(UInt64 raceId);

	void OnTick(const Avalon::Time& now);

	BSBaseRace* FindRaceByRoleId(UInt64 roleId);

	void  OnPlayerJoinRace(BSBaseRace* race, BSPlayer* player);
	void  OnPlayerLeaveRace(UInt64 roleId, bool isDisconnect);
private:
	//<raceid, race>
	HashMap<UInt64, BSBaseRace*>   m_races;

	HashMap<UInt64, BSBaseRace*>   m_roleId2Race;
};