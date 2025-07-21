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
	*@brief �ϱ������
	*/
	virtual void ReportChecksum(UInt32 frame, UInt32 checksum) {}

	/**
	*@brief �ϱ�����
	*/
	virtual void ReportRaceEnd(Avalon::Packet* raceEndInfo) {}

	/**
	*@brief �Ƿ����е����һ֡��
	*/
	virtual bool IsRunToLastFrame();

protected:
	VerifyTask*			m_Task;
};

#endif