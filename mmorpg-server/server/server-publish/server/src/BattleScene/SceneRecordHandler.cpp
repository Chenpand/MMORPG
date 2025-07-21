#include "SceneRecordHandler.h"

#include "PlayerMgr.h"

void HandleProtocol(CLProtocol::RecordNotifyOtherDisconnected& protocol)
{
	class OnRecordDisconnectedVisitor : public PlayerVisitor
	{
	public:
		OnRecordDisconnectedVisitor(UInt32 zoneId):m_ZoneId(zoneId){}

		bool operator()(Player* player)
		{
			if(m_ZoneId == 0)
			{
				if(player->GetSrcZoneId() != 0) player->KickOff();
			}
			else
			{
				if(player->GetSrcZoneId() == m_ZoneId) player->KickOff();
			}
			return true;
		}

	private:
		UInt32	m_ZoneId;
	};

	OnRecordDisconnectedVisitor visitor(protocol.zoneId);
	PlayerMgr::Instance()->VisitNormalPlayers(visitor);
}
