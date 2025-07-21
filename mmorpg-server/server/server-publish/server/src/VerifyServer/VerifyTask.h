#ifndef __VERIFY_TASK_H__
#define __VERIFY_TASK_H__

#include <CLDefine.h>
#include <CLRelayServerProtocol.h>
#include <CLMatchDefine.h>
#include <CLVerifyDefine.h>
#include <CLCheatDefine.h>

// ��֤�����״̬
enum VerifyTaskStatus
{
	// ��ʼ��
	VERIFY_TASK_STATUS_INIT,
	// ���ڽ���
	VERIFY_TASK_STATUS_DOING,
	// �ȴ��߼�����
	VERIFY_TASK_WAIT_LOGIC_FINISH,
	// ���
	VERIFY_TASK_STATUS_FINISH,
};

struct LogicChecksum
{
	UInt32 frame;
	UInt32 checksum;
};

struct VerifyGamer
{
	VerifyGamer()
	{
		roleId = 0;
		pos = 0;
		cheat = false;
	}
	ObjID_t						roleId;
	UInt8						pos;
	std::vector<ChecksumInfo>	checksums;
	bool						cheat;
};

// ����ս���� 0~4 ��һ����5-9 ������һ��
#define MAX_GAMER_COUNT	(10)
//֡У��ļ������λ��֡��
#define FRAME_CHECKSUM_INTERVAL (50)

struct RaceChecksumInfo
{
	RaceChecksumInfo()
	{
		memset(this, 0, sizeof(*this));
	}

	// ֡��
	UInt32 frame;
	// ������ҵ�У�飨����λ�ţ�
	UInt32 checksums[MAX_GAMER_COUNT];
};

class ILogic;
class VerifyTask
{
public:
	VerifyTask(RaceType type, ObjID_t raceId, UInt32 connId);
	virtual ~VerifyTask();

public:
	/**
	*@brief ����ս����ʼ����Ϣ��������
				pk:			MatchSvrNotifyRacePlayerInfo
				Dungeon:	WorldDungeonStartRaceReq
	*/
	bool OnInit(Avalon::Packet* raceInitPacket);

	/**
	*@brief Tick
	*/
	void OnTick(const Avalon::Time& now);

public:
	/**
	*@brief ID
	*/
	ObjID_t GetID() const { return m_RaceId; }

	/**
	*@brief Type
	*/
	RaceType GetType() const { return m_Type; }

	/**
	*@brief ��֤����״̬
	*/
	void SetStatus(VerifyTaskStatus status) { m_Status = status; }
	VerifyTaskStatus GetStatus() const { return m_Status; }

	/**
	*@brief ��Ϸ�����
	*/
	void AddGamer(ObjID_t roleId, UInt8 pos);
	VerifyGamer* FindGamerByRoleID(ObjID_t raceId);
	VerifyGamer* FindGamerByPos(UInt8 pos);

	/**
	*@brief ��ȡ����ϱ������ʱ��(������Ǹ�)
	*/
	UInt64 GetPlayerReportRaceEndTime() const { return m_PlayerReportRaceEndTime; }

public:
	/**
	*@brief ���֡����
		fillFrame�����Ƿ����ʱ��֡
	*/
	void PushFrame(const std::vector<CLProtocol::Frame>& frames);

	/**
	*@brief ��������ϱ��Ľ���
	*/
	void SetPlayerRaceEndInfo(ObjID_t roleId, Avalon::Packet* raceEndInfo);

	/**
	*@brief ��������ϱ��������
	*/
	void SetPlayerChecksum(ObjID_t roleId, UInt32 frame, UInt32 checksum);

public:
	// �߼����
	/**
	*@brief �����߼��ϱ��Ľ���
	*/
	void SetLogicRaceEndInfo(Avalon::Packet* raceEndInfo);
	Avalon::Packet* GetLogicRaceEndInfo() { return m_LogicRaceEnd; }

	/**
	*@brief �����߼��ϱ��������
	*/
	void SetLogicChecksum(UInt32 frame, UInt32 checksum);

	/**
	*@brief ������֤
	*/
	void GiveupVerify();

protected:
	// ��֤���

	/**
	*@brief ��֤�����
	*/
	void _OnFrameChecksum();

	/**
	*@brief ��֤����
	*/
	void _OnCheckRaceEnd();

	/**
	*@brief ��֤�ɹ�
	*/
	virtual void _OnVerifyFinish();

	/**
	*@brief ��֤��ʱ
	*/
	virtual void _OnVerifyTimeout();

	/**
	*@brief �������
	*/
	virtual void _OnPlayerCheat(ObjID_t roleid, PlayerCheatReason reason, UInt32 param = 0);

	/**
	*@brief ���һ�����֡��
	*/
	UInt32 _GetMaxFrameNum() const;

	/**
	*@brief һ�ֱ������ʱ��(ms)
	*/
	UInt64 _GetRaceMaxTime() const ;

	/**
	*@brief ��ӡ����
	*/
	void ShowDungeonRaceEnd(ObjID_t roleid, Avalon::Packet* packet);

	/**
	*@brief ��ӡ����
	*/
	void ShowPkRaceEnd(ObjID_t roleid, Avalon::Packet* packet);

protected:
	/**
	*@brief ����Э�鵽������
	*/
	void _SendbackProtocol(Avalon::Protocol& protocol);

protected:
	// ��������
	RaceType							m_Type;
	// �߼�����
	ILogic*								m_Logic;
	// ����ID
	ObjID_t								m_RaceId;
	// ������ID
	UInt32								m_ConnId;
	// ״̬
	VerifyTaskStatus					m_Status;
	// ��ʼʱ��
	UInt64								m_StartTime;
	// �յ������һ֡
	UInt32								m_LastFrame;
	// ���е�֡
	std::vector<CLProtocol::Frame>		m_Frames;
	// �����Ϣ
	std::vector<VerifyGamer>			m_Gamers;
	
	// ��֤���
	// ��������ϱ��Ľ�����Ϣ
	std::map<ObjID_t, Avalon::Packet*>	m_PlayerRaceEnd;
	// �߼����ɵĴ�У������
	std::map<UInt32, UInt32>			m_logicChecksums;
	// �߼����ɵĽ�����Ϣ
	Avalon::Packet*						m_LogicRaceEnd;
	// �յ���ҽ���ʱ��
	UInt64								m_PlayerReportRaceEndTime;
};

#endif