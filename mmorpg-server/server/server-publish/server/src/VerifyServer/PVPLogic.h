#ifndef __PVP_LOGIC_H__
#define __PVP_LOGIC_H__

#include "ILogic.h"

class VerifyTask;
class PVPLogic : public ILogic
{
public:
	virtual bool OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket);

	/**
	*@brief 是否需要返回结算信息
	*/
	virtual bool IsNeedReportRaceEndToServer() const;
};

#endif