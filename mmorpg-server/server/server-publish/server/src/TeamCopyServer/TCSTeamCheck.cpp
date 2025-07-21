#include "TCSTeamCheck.h"

TCSTeamCheck::TCSTeamCheck()
{
	m_TeamId = 0;
}

TCSTeamCheck::~TCSTeamCheck()
{

}

bool TCSTeamCheck::IsAllCheck()
{
	return m_PlayerSet.empty();
}

void TCSTeamCheck::JoinCheck(UInt64 roleId)
{
	m_PlayerSet.insert(roleId);
}

void TCSTeamCheck::RemoveCheck(UInt64 roleId)
{
	auto iter = m_PlayerSet.find(roleId);
	if (iter == m_PlayerSet.end())
	{
		return;
	}

	m_PlayerSet.erase(roleId);
}