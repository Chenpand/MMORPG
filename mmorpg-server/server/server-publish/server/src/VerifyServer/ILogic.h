#ifndef __ILOGIC_H__
#define __ILOGIC_H__

#include <CLDefine.h>
#include <CLRelayServerProtocol.h>

class VerifyTask;
class ILogic
{
public:
	ILogic() : m_Task(NULL) {}
	virtual ~ILogic() {}

public:
	ObjID_t	GetID();

	virtual bool OnInit(VerifyTask* task, Avalon::Packet* raceInitPacket);
	virtual void OnTick(const Avalon::Time& now);
	virtual void PushFrame(const CLProtocol::Frame& frame);

	virtual std::string ShowInfo() const { return ""; }

	virtual UInt32 GetDungeonID() const { return 0; }
	
	/**
	*@brief �Ƿ����е����һ֡��
	*/
	virtual bool IsRunToLastFrame();

	/**
	*@brief �Ƿ���Ҫ���ؽ�����Ϣ
	*/
	virtual bool IsNeedReportRaceEndToServer() const { return false; }

protected:
	VerifyTask*											m_Task;
	// ��push���߼���֡
	std::vector<CLProtocol::RelaySvrFrameDataNotify>		m_WaitPushFrames;
};

#endif