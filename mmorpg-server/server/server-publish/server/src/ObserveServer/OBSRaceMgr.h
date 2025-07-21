#ifndef __OBS_RACE_MGR_H__
#define __OBS_RACE_MGR_H__

#include <map>
#include <set>

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLRelayServerProtocol.h>
#include <CLObject.h>

#include "ObserveRelaySession.h"
#include "RelaySession.h"

class OBSRace : public CLObject
{
public:
	void Init(UInt32 nodeID);
	void OnTick(const Avalon::Time& now);
	void OnSyncFrame(std::vector<CLProtocol::Frame>& frames);
	bool HaveOBSRelaySession(UInt32 nodeID);
	std::vector<CLProtocol::Frame>& GetAllFrames();
	void AddOBSRelay(UInt32 nodeID);
	/**
	*@brief ��ʼ����
	*/
	void SetInitPacket(Avalon::Packet* packet) { m_Packet = packet; }
	Avalon::Packet* GetInitPacket() const { return m_Packet; }
private:
	void SendUnSyncFrame();
private:
	//�����������ڵ�OBSRelay
	std::set<UInt32> m_OBSRealyNodeSet;
	//�����������ڵ�Relay
	UInt32 m_NodeID;
	std::vector<CLProtocol::Frame> m_AllFrames;
	std::vector<CLProtocol::Frame> m_UnSyncFrames;
	Avalon::Packet* m_Packet;
};



class OBSRaceMgr : public Avalon::Singleton<OBSRaceMgr>
{
public:
	void Init();
	void Final();
	void OnTick(const Avalon::Time& now);
	void OnRaceStart(UInt64 raceID, UInt32 nodeID, Avalon::Packet* packet);
	OBSRace* FindRace(UInt64 raceID);
	void OnSyncFrame(UInt64 raceID, std::vector<CLProtocol::Frame>& frames);
private:
	//����id -> ����
	std::map<UInt64, OBSRace> m_RaceMap;
};







#endif


