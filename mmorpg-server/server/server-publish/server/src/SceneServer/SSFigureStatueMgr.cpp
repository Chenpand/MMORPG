#include "SSFigureStatueMgr.h"
#include "PlayerMgr.h"

void SSFigureStatueMgr::SetFigureStatue(const FigureStatueVec& statues)
{
	for (UInt32 i = 0; i < m_FigureStatues.size(); ++i)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_FigureStatues[i].roleId);
		if (player != NULL)
		{
			player->OnRemoveFigureStatus(m_FigureStatues[i]);
		}
	}

	m_FigureStatues = statues;

	for (UInt32 i = 0; i < m_FigureStatues.size(); ++i)
	{
		Player* player = PlayerMgr::Instance()->FindNormalPlayer(m_FigureStatues[i].roleId);
		if (player != NULL)
		{
			player->OnCreateFigureStatus(m_FigureStatues[i]);
		}
	}
}

bool SSFigureStatueMgr::IsFigureStatue(FigureStatueType type, UInt64 playerId)
{
	for (UInt32 i = 0; i < m_FigureStatues.size(); ++i)
	{
		if (m_FigureStatues[i].statueType == (UInt8)type && playerId == m_FigureStatues[i].roleId)
		{
			return true;
		}
	}
	return false;
}

void SSFigureStatueMgr::GetFigureStatuesById(std::vector<FigureStatueType>& figureStatueList, UInt64 playerId)
{
	for (UInt32 i = 0; i < m_FigureStatues.size(); ++i)
	{
		if (playerId == m_FigureStatues[i].roleId)
		{
			figureStatueList.push_back((FigureStatueType)m_FigureStatues[i].statueType);
		}
	}
}
