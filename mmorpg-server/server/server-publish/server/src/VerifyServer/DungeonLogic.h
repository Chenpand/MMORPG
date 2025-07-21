#ifndef __DUNGEON_LOGIC_H__
#define __DUNGEON_LOGIC_H__

#include "ILogic.h"

class VerifyTask;
class DungeonLogic : public ILogic
{
public:
	virtual std::string ShowInfo() const;
	virtual bool OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket);

	virtual UInt32 GetDungeonID() const { return m_DungeonId; }

	virtual bool IsNeedReportRaceEndToServer() const;

private:
	UInt32			m_DungeonId;
};

#endif