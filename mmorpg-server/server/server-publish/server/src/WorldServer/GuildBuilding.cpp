#include "GuildBuilding.h"

GuildBuildingMgr::GuildBuildingMgr()
{
	for (UInt32 i = 0; i < sizeof(m_Buildings) / sizeof(m_Buildings[0]); i++)
	{
		m_Buildings[i].type = (GuildBuildingType)i;
		m_Buildings[i].level = 1;
	}
}

GuildBuildingMgr::~GuildBuildingMgr()
{

}

void GuildBuildingMgr::SetLevel(GuildBuildingType type, UInt8 level)
{
	if (type >= GUILD_BUILDING_NUM)
	{
		return;
	}

	if (m_Buildings[type].level == level)
	{
		return;
	}

	m_Buildings[type].level = level;
	SetDirty();
}

UInt8 GuildBuildingMgr::GetLevel(GuildBuildingType type) const
{
	if (type >= GUILD_BUILDING_NUM)
	{
		return 0;
	}

	return m_Buildings[type].level;
}

std::vector<GuildBuilding> GuildBuildingMgr::GetBuildings() const
{
	std::vector<GuildBuilding> buildings;
	for (auto i = 0; i < GUILD_BUILDING_NUM; i++)
	{
		buildings.push_back(m_Buildings[i]);
	}

	return buildings;
}

void GuildBuildingMgr::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & (UInt8)GUILD_BUILDING_NUM;
	for (auto& building : m_Buildings)
	{
		stream & (UInt8)building.type & building.level;
	}
}
void GuildBuildingMgr::Decode(Avalon::NetInputStream& stream)
{

}

void GuildBuildingMgr::DecodeString(std::istringstream& is)
{
	char split = 0;
	UInt32 type = 0;
	UInt32 level = 1;
	while (is >> type)
	{
		is >> split >> level >> split;

		if (type >= GUILD_BUILDING_NUM)
		{
			return;
		}

		m_Buildings[type].level = (UInt8)level;
	}
}

void GuildBuildingMgr::EncodeString(std::ostringstream& os)
{
	for (const auto& building : m_Buildings)
	{
		os << (UInt32)building.type << ":" << (UInt32)building.level << "|";
	}
}