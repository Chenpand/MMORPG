#ifndef __PVP_LOGIC_H__
#define __PVP_LOGIC_H__

#include "ILogic.h"

class VerifyTask;
class PVPLogic : public ILogic
{
public:
	virtual bool OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket);

	/**
	*@brief �Ƿ���Ҫ���ؽ�����Ϣ
	*/
	virtual bool IsNeedReportRaceEndToServer() const;
};

#endif