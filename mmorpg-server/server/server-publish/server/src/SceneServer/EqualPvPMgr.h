#ifndef _EQUAL_PK_MGR_H_
#define _EQUAL_PK_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonDefine.h>
#include "Item.h"

class EqualPvPMgr: public Avalon::Singleton<EqualPvPMgr>
{
	using PlayerAvatarMap = HashMap<UInt32, PlayerAvatar>;
public:
	EqualPvPMgr();
	~EqualPvPMgr();
	bool Init();
	//根据职业获得公平竞技场里的装备
	void GetEqualPvPEquip(UInt32 occu, std::vector<Item*>& equips);
	PlayerAvatar GetEqualPvPAvatar(UInt32 occu);
private:
	HashMap<UInt32, std::vector<Item*>> m_occuEquipMap;
	PlayerAvatarMap m_avatarMap;
};

#endif