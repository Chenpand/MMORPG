#ifndef __TEST_LOGIC_H__
#define __TEST_LOGIN_H__

#include "ILogic.h"

class VerifyTask;
class TestLogic : public ILogic
{
public:
	virtual bool OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket);
	virtual void OnTick(const Avalon::Time& now) {}
	virtual void PushFrame(const CLProtocol::Frame& frame);

	/**
	*@brief 上报随机数
	*/
	virtual void ReportChecksum(UInt32 frame, UInt32 checksum) {}

	/**
	*@brief 上报结算
	*/
	virtual void ReportRaceEnd(Avalon::Packet* raceEndInfo) {}

	/**
	*@brief 是否运行到最后一帧了
	*/
	virtual bool IsRunToLastFrame();

protected:
	VerifyTask*			m_Task;
};

#endif