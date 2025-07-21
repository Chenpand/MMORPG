#include "SeasonMgr.h"
#include "PlayerMgr.h"
#include <CLSeasonDataEntry.h>

#define SEASON_MAX_ATTR 7

bool SeasonMgr::ChangeSeasonAttr(UInt32 seasonAttrStartTime, UInt32 seasonAttrEndTime, std::vector<UInt64> playerIds)
{
	if (m_SeasonAttrStartTime == seasonAttrStartTime && m_SeasonAttrEndTime == seasonAttrEndTime) return false;

	m_SeasonAttrStartTime = seasonAttrStartTime;
	m_SeasonAttrEndTime = seasonAttrEndTime;

	if (!playerIds.empty())
	{
		//更新
		UInt32 maxLevel = SeasonLevelDataEntryMgr::Instance()->GetMaxSeasonLevelValue();
		for (UInt32 i = 0; i < playerIds.size(); ++i)
		{
			Player* player = PlayerMgr::Instance()->FindNormalPlayer(playerIds[i]);
			if (player != NULL)
			{
				if (player->GetSeasonLevel() >= maxLevel && player->GetChangeSeasonAttrTime() < m_SeasonAttrStartTime)
				{
					player->SetSeasonAttr(m_SeasonAttrStartTime, SEASON_MAX_ATTR);
				}
			}
			else
			{
				//更新玩家表数据
				std::ostringstream condstream;
				condstream << "guid =" << playerIds[i] << " and `season_level` >=" << maxLevel << " and season_attr_end_time <" << m_SeasonAttrStartTime;
				CLUpdateCommand	*cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", condstream.str());
				cmd->PutData("season_attr_end_time", m_SeasonAttrStartTime);
				cmd->PutData("season_attr", SEASON_MAX_ATTR);
				cmd->PutData("season_play_status", (UInt8)SeasonPlayStatus::SPS_NEW_ATT);
				CLRecordClient::Instance()->SendCommand(cmd);

				//更新玩家counter数据
				std::ostringstream counterCondStream;
				counterCondStream << "owner =" << playerIds[i] << " and name='" << SEASON_ATTR_LEVEL << "'";
				CLUpdateCommand	*counterCmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_counter", counterCondStream.str());
				counterCmd->PutData("value", maxLevel);
				CLRecordClient::Instance()->SendCommand(counterCmd);

				//处理已经下线,但是内存中存在玩家
				Player* tmpPlayer = PlayerMgr::Instance()->FindPlayer(playerIds[i]);
				if (tmpPlayer != NULL)
				{
					if (tmpPlayer->GetSeasonLevel() >= maxLevel && tmpPlayer->GetChangeSeasonAttrTime() < m_SeasonAttrStartTime)
					{
						tmpPlayer->SetSeasonAttr(m_SeasonAttrStartTime, SEASON_MAX_ATTR);
					}
				}
			}
		}
		
	}
	return true;
}

void SeasonMgr::UpdateSeasonInfo()
{
	class UpdateSeasonInfoPlayerVisitor : public PlayerVisitor
	{
	public:
		UpdateSeasonInfoPlayerVisitor(){}
		~UpdateSeasonInfoPlayerVisitor(){}

		bool operator()(Player* pPlayer)
		{
			pPlayer->UpdateSeasonInfo();
			return true;
		}

	};

	UpdateSeasonInfoPlayerVisitor visitor;

	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}

