#ifndef _TCS_TEAM_CHECK_H_
#define _TCS_TEAM_CHECK_H_

#include <CLTeamCopyDefine.h>

/**
*@brief 队伍开战检查
*/
class TCSTeamCheck
{
public:
	TCSTeamCheck();
	~TCSTeamCheck();

	/**
	*@brief 队伍id
	*/
	inline void SetTeamId(UInt32 id) { m_TeamId = id; }
	inline UInt32 GetTeamId() { return m_TeamId; }

	/**
	*@brief 检查的玩家
	*/
	const std::set<UInt64>& GetCheckPlayerSet() { return m_PlayerSet; }

	/**
	*@brief 是否全都检查ok
	*/
	bool IsAllCheck();

	/**
	*@brief 加入检查列表
	*/
	void JoinCheck(UInt64 roleId);

	/**
	*@brief 删除检查玩家
	*/
	void RemoveCheck(UInt64 roleId);

private:

	// 队伍id
	UInt32 m_TeamId;
	// 检查的玩家列表
	std::set<UInt64> m_PlayerSet;
};

#endif
