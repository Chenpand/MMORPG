#ifndef __GUILD_BUILDING_H__
#define __GUILD_BUILDING_H__

#include <CLGuildDefine.h>
#include <CLObjectProperty.h>

class GuildBuildingMgr : public CLComplexProperty
{
public:
	GuildBuildingMgr();
	~GuildBuildingMgr();

public:
	/**
	*@brief 建筑等级
	*/
	void SetLevel(GuildBuildingType type, UInt8 level);
	UInt8 GetLevel(GuildBuildingType type) const;

	/**
	*@brief 建筑信息
	*/
	std::vector<GuildBuilding> GetBuildings() const;

public:
	// 编码相关
	virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	virtual void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

private:
	GuildBuilding m_Buildings[GUILD_BUILDING_NUM];
};

#endif