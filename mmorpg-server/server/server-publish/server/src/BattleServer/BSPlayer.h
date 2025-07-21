#pragma once

#include <AvalonProtocol.h>
#include <CLDefine.h>
#include <CLObject.h>
#include <CLObjectDefine.h>
#include <CLMatchProtocol.h>
#include <CLSysNotify.h>
#include <CLChatDefine.h>
#include <CLChatProtocol.h>
#include <CLLostDungeonDefine.h>

class BSPlayer : public CLObject
{
public:
	BSPlayer();
	~BSPlayer();

public:
	void SetSourceSceneNodeId(UInt32 sourceSceneNodeId);
	UInt32 GetSourceSceneNodeId() { return m_SourceSceneNodeId; }

	void SetDstSceneNodeId(UInt32 dstSceneNodeId) {	m_dstBSSceneNodeId = dstSceneNodeId;}
	UInt32 GetDstSceneNodeId() { return m_dstBSSceneNodeId; }

	void SetAccId(UInt32 accId) { m_accId = accId; }
	UInt32 GetAccId() { return m_accId; }

	inline UInt8 GetOccu() { return m_racePlayerInfo.occu;  }

	/**
	*@brief ���û�ȡԭ��id
	*/
	void SetSrcZoneId(UInt32 srcZoneId) { m_SrcZoneId = srcZoneId; }
	UInt32 GetSrcZoneId() const { return m_SrcZoneId; }


	//ս����Ϣ
	void SetRacePlayerInfo(RacePlayerInfo& racePlayerInfo) { m_racePlayerInfo = racePlayerInfo; }
	const RacePlayerInfo& GetRacePlayerInfo() { return m_racePlayerInfo; }

	void SetProtectTimeStamp(UInt32 timeStamp);
	UInt32 GetProtectTimeStamp() { return m_protectTimeStamp; }
	/**
	*@brief ������Ϣ��scene
	*/
	void SendToBScene(Avalon::Protocol& protocol);
	bool SendToSelfScene(Avalon::Protocol& protocol);

	/**
	*@brief  ������Ϣ�����
	*/
	bool sendToClient(Avalon::Protocol& protocol);

	/**
	*@brief ͬ���¼���Scene
	*/
	void SyncEventToBScene(SyncEventType type, ObjID_t param1 = 0, ObjID_t param2 = 0);

	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		CLProtocol::SysNotify notify;
		notify.type = type;
		notify.channel = 0;
		notify.color = color;
		notify.word = buffer;
		sendToClient(notify);
	}

protected:
	UInt32  m_accId;				//�˺�id
	UInt32	m_SourceSceneNodeId;	//ԴScene�ڵ�id
	UInt32  m_SrcZoneId;			//ԭ��id
	UInt32  m_dstBSSceneNodeId;		//Ŀ��ս���������ڵ�id
	RacePlayerInfo m_racePlayerInfo; //���ս����Ϣ

public:	//��ʧ����
	//��ʧ����״̬
	void SetLostState(UInt8 state, bool pvp = false);
	UInt8 GetLostState() { return m_lostState; }

	bool IsCanPK();
	bool IsCanPKed();

	inline void SetLostRaceId(UInt64 raceId) { m_lostRaceId = raceId; }
	inline	UInt64 GetLostRaceId() { return m_lostRaceId; }

	inline void SetLostBattleId(UInt32 battleId) { m_currLostBattleId = battleId; }
	inline	UInt64 GetLostBattleId() { return m_currLostBattleId; }
protected: //��ʧ����
	LostDungeonPlayerState	m_lostState; //��ʧ����״̬
	UInt64	m_lostRaceId;	//����id
	UInt32	m_currLostBattleId; //��ǰ����ս��Id
	UInt32  m_protectTimeStamp; //ս���󱣻��ڽ���ʱ���
};

typedef std::vector<BSPlayer*> VBSPlayer;

typedef HashMap<ObjID_t, BSPlayer*> MapBSPlayer;