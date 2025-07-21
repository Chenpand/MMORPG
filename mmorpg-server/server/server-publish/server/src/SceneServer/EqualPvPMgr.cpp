#include "EqualPvPMgr.h"
#include <CLEqualPvPEuqipDataEntry.h>
#include "ItemMaker.h"


EqualPvPMgr::EqualPvPMgr()
{
}


EqualPvPMgr::~EqualPvPMgr()
{
	for (auto &vec : m_occuEquipMap)
	{
		for (auto& item : vec.second)
		{
			CL_SAFE_DELETE(item);
		}
	}
}

bool EqualPvPMgr::Init()
{
	class EquipVisit : public Avalon::DataEntryVisitor<EqualPvPEuqipDataEntry>
	{
	public:
		EquipVisit(PlayerAvatarMap &avatarMap) : m_avatarMap(avatarMap){ };
		~EquipVisit(){};
		bool operator()(EqualPvPEuqipDataEntry* dataEntry)
		{
			Item* item = ItemMaker::Instance()->MakeItem(dataEntry->equipID, 1, 100, dataEntry->strengthenLv);
			if (!item || !item->GetDataEntry())
			{
				return false;
			}
			EqualPvPMgr::Instance()->m_occuEquipMap[dataEntry->occu].push_back(item);
			if (item->GetDataEntry()->subType == ST_EQUIP_WEAPON)
			{
				m_avatarMap[dataEntry->occu].weaponStrengthen = item->GetStrengthen();
			}
			m_avatarMap[dataEntry->occu].equipItemIDs.push_back(item->GetDataID());
			return true;
		}
	private:
		PlayerAvatarMap &m_avatarMap;
	};

	EquipVisit visit(m_avatarMap);
	EqualPvPEuqipDataEntryMgr::Instance()->Visit(visit);
	for (auto &avatar : m_avatarMap)
	{
		//avatar.second.titleId = this->GetActiveTitle();
		avatar.second.isShoWeapon = 0;
	}
	return true;
}

void EqualPvPMgr::GetEqualPvPEquip(UInt32 occu,std::vector<Item*>& equips)
{
	auto iter = m_occuEquipMap.find(occu);
	if (iter == m_occuEquipMap.end())
	{
		return;
	}

	for (auto& item : iter->second)
	{
		equips.push_back(item);
	}
}


PlayerAvatar EqualPvPMgr::GetEqualPvPAvatar(UInt32 occu)
{
	auto iter = m_avatarMap.find(occu);
	if (iter != m_avatarMap.end())
	{
		return iter->second;
	}
	else
	{
		return{};
	}
}
