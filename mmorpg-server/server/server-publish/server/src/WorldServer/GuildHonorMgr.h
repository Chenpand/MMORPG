#ifndef __GUILD_HONOR_H__
#define __GUILD_HONOR_H__

#include <CLGuildDefine.h>
#include <CLObjectProperty.h>
#include <CLGuildHonorDataEntry.h>
#include "CLGuildProtocol.h"
class Guild;
struct GuildHonorData
{
	GuildHonorData()
	{
		id = 0;
		groupId = 0;
		type = 0;
		curCount = 0;
		maxCount = 0;
		finishTime = 0;
	}
	UInt32 id;
	UInt32 groupId;
	//公会荣誉类型 GuildHonorChallenge
	UInt32 type;
	UInt32 curCount;
	UInt32 maxCount;
	UInt32 finishTime;
	std::vector<UInt32> conds;
};
class GuildHonorMgr : public CLComplexProperty
{
	typedef std::map<UInt32, UInt32> MapById;
	typedef std::map<UInt32, MapById> HonorMap;
public:
	GuildHonorMgr();

	void SetOwner(Guild* guild){ m_guild = guild; }
	Guild* GetOwner(){return m_guild; }

	void DecodeString(std::istringstream& is);

	void EncodeString(std::ostringstream& os);
	void AddGuildHonorCnt(GuildHonorChallenge type, std::vector<UInt32>& conds);
	void GetGuildHonor(CLProtocol::WorldGuildHonorInfoRes& res, std::vector<UInt32>& groupIds);
private:
	Guild*						m_guild;
	std::vector<GuildHonorData>	m_honorVec;
	// type -> dataEntry Id -> m_honorVec下标
	HonorMap					m_HonorMap;
	//dataEntry Id -> m_honorVec下标
	MapById						m_HonorById;
};

#endif