#pragma once

#include <functional>
#include <CLObject.h>
#include <CLLostDungeonDefine.h>

class Player;
class LostDungeonTeam
{
public:
	//队伍成员
	typedef std::map<UInt8, LostDungTeamMember> MemberMap;
	typedef std::map<UInt64, LostDungTeamMember> PlayerMapMember;
	//最大人数
	const static UInt32 MAX_MEMBER_SIZE = 3;
public:
	LostDungeonTeam(UInt32 id);
	~LostDungeonTeam();

public:
	inline UInt32	GetId() { return m_Id; }

	inline UInt64 GetSceneId() { return m_SceneId; }
	inline void	  SetSceneId(UInt64 sceneId) { m_SceneId = sceneId; }

	inline void SetIndex(UInt8 index) { m_index = index; }
	inline UInt8 GetIndex() const { return m_index; }

	inline void SetChallengeMode(UInt8 mode) { m_challengeMode = mode; }
	inline UInt8 GetChallengeMode() {	return m_challengeMode;}

	inline UInt8 GetBattleState() { return m_battleState; }
	bool  SetBattleState(UInt8 st);

	inline UInt32 GetNum() const { return m_Members.size(); };

	bool IsEmpty();

	void AddMember(Player* player, PlayerMapMember& updates, bool isSwtich = false, bool notify = true);
	LostDungTeamMember* FindMember(ObjID_t id);

	void RemoveMember(ObjID_t id, PlayerMapMember& updates, bool isSwtich = false);

	void GetInfo(LostDungTeamInfo& info);

	bool VistMembers(const std::function<bool(LostDungeonTeam* team, LostDungTeamMember* member)>& ff);

	void OnRaceEnd();
private:
	//队伍id
	UInt32	m_Id;
	//队伍索引
	UInt8   m_index;
	//挑战模式
	UInt8   m_challengeMode;
	//队长
	ObjID_t	m_Master;
	//队员列表
	MemberMap	m_Members;
	//所在场景id
	UInt64	m_SceneId;
	//战斗状态
	UInt8	m_battleState;
};