#ifndef _TEAM_H_
#define _TEAM_H_

#include <CLObject.h>
#include <CLChatDefine.h>
#include <CLSysNotify.h>
#include <vector>
#include <AvalonSocket.h>
#include <CLDungeonDefine.h>
#include <CLTeamDefine.h>
#include <CLObjectDefine.h>

class Player;
class TeamMemberVisitor
{
public:
	virtual ~TeamMemberVisitor(){}

	virtual bool operator()(Player* player) = 0;
};

/**
 *@brief ����
 */
class Team
{
	typedef std::vector<SceneTeamMember> MemberVec;

	//�������
	const static UInt32 MAX_TEAM_SIZE = 5;

public:
	Team(UInt32 id);
	~Team();

public:
    /**
    *@brief ��ӿ�ʼ���³�
    */
	bool OnPlayerRequestStartDungeonRace(Player* player, UInt32 dungeonId, const SceneNpc& cityMonster);

	/**
	*@brief world����ʼ��������
	*/
	bool OnWorldServerStartDungeonRes(UInt32 dungeonId, const SceneNpc& cityMonster);

    /**
    *@brief RelayServer������������
    */
    bool OnRelayServerCreateRaceRet(UInt32 relayServerID, UInt32 dungeonId, UInt64 session, Avalon::SockAddr addr, bool isHell, const SceneNpc& cityMonster);

    /**
    *@brief ����
    */
    bool OnDungeonRaceEnd(const DungeonRaceEndInfo& info);

    /**
    *@brief ����
    */
    void OnPlayerLeaveDungeonRace(ObjID_t roleId);

	/**
	*@brief �������ܷ�ʼ����
	*/
	bool CanStartDungeonRace(UInt32 dungeonId, bool checkTicket, bool sendNotice, bool checkLoading, const SceneNpc& cityMonster);

private:
    std::vector<Player*> _GetMemberPlayerInfo();

public:
	/**
	 *@brief ��ȡid
	 */
	UInt32 GetId() const{ return m_Id; }

    /**
    *@brief ����id
    */
    void SetRaceID(ObjID_t id) { m_raceId = id; }
    ObjID_t GetRaceID() const { return m_raceId; }

    /**
    *@brief RelayServer id
    */
    void SetRelayServerID(UInt32 id) { m_relayServerId = id; }
    UInt32 GetRelayServerID() const { return m_relayServerId; }

    /**
    *@brief session
    */
    void SetSession(ObjID_t session) { m_session = session; }
    ObjID_t GetSession() const { return m_session; }

	/**
	 *@brief ���û�ȡ�ӳ�
	 */
	void SetMaster(ObjID_t master){ m_Master = master; }
	ObjID_t GetMaster() const{ return m_Master; }

	/**
	 *@brief ��ȡ��С
	 */
	UInt8 GetSize() const{ return UInt8(m_Members.size()); }

	/**
	 *@brief ���ó�Ա�б�
	 */
	void SetMembers(const MemberVec& members);

	/**
	 *@brief ����Ƴ���Ա
	 */
	void AddMember(ObjID_t member);
	void RemoveMember(ObjID_t member);
	bool IsMember(ObjID_t member) const;
	SceneTeamMember* FindMember(ObjID_t id);

	/**
	 *@brief ���û�ȡ�ϴλ�õ��ߵĶ���
	 */
	void SetLastItemOwner(ObjID_t owner){ m_LastItemOwner = owner; }
	ObjID_t GetLastItemOwner() const{ return m_LastItemOwner; }

	/**
	 *@brief ������Χ�ڶ���
	 */
	void VisitMembersAround(UInt32 sceneId, const CLPosition& pos, TeamMemberVisitor& visitor);
	void VisitSceneMembers(UInt32 sceneId, TeamMemberVisitor& visitor);

	/**
	 *@brief ��ȡ�������ĳ�Ա��
	 */
	UInt32 GetMemberNumInScene();

	/** 
	 *@brief ��ȡ��Χͬ������
	 */
	UInt32 GetMemberNumAround(UInt32 sceneId, UInt16 x, UInt16 y);

	/**
	 *@brief �������Ա�ȼ�
	 */
	ObjID_t	CheckMembersLevel(UInt16 minLevel);

	/**
	 *@brief ��ȡ���ȼ�
	 */
	UInt16 GetMembersMaxLevel();

	/**
	 *@brief ��ȡ����ƽ���ȼ�
	 */
	UInt16 GetMembersAverageLevel();

	/**
	 *@brief ��鱳��������
	 */
	ObjID_t	CheckPackGridNum(UInt32 num);

	/**
	 *@brief �������Ա�ܷ��ȡĳ������
	 */
	ObjID_t CheckTaskAcceptable(UInt32 id);

	/**
	 *@brief ��������
	 */
	void	AcceptTask(UInt32 id, UInt8 quality);

	/**
	 *@brief �ύ����
	 */
	void	SubmitTask(UInt32 id, UInt32 sceneId, UInt16 x, UInt16 y);

	/**
	 *@brief ������Ϣ
	 */
	void UpdateTeamInfo();

    /**
    *@brief ���¶�Ա״̬
    */
    void UpdateTeamMemberStatus(ObjID_t id, UInt8 status);

    /**
    *@brief �Ƿ����г�Ա��׼������
    */
    bool IsAllMemberReady();

	/**
	 *@brief ���Ͷ���֪ͨ
	 */
	//by huchenhui
	//void BroadcastNotify(UInt32 id, ... );

	template <typename... Args>
	void BroadcastNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);
		if (!ret) return;

		Notify(type, color, buffer);
	}

	void Notify(UInt16 type, UInt8 color, const char* word);

    /**
    *@brief �㲥Э��
    */
    void BroadcastProtocol(Avalon::Protocol& protocol);

	/**
	 *@brief �����������
	 */
	void SetTaskVar(UInt32 taskId, const char* name, Int64 val);
	/**
	 *@brief �Ƿ�Ϊ���Ի�϶��� ��ͬһ������
	 */
	bool IsMixedTeamInScene(UInt32 sceneId);

public://�¼�
	/**
	 *@brief ���볡��
	 */
	void OnEnterScene(Player* player);

	/**
	 *@brief �뿪����
	 */
	void OnLeaveScene(Player* player);

	/**
	 *@brief �����ƶ�
	 */
	void OnMoved(Player* player);

private:
	/**
	 *@brief ͬ��λ��
	 *@param bOther �Ƿ�ͬ��������λ��
	 */
	void SyncScenePos(Player* player, bool bOtherPos);

private:
	//����id
	UInt32	m_Id;
    //����ID
    ObjID_t m_raceId;
    //RelayServer��ID
    UInt32 m_relayServerId;
    //Session
    ObjID_t m_session;
	//�ӳ�
	ObjID_t	m_Master;
	//��Ա�б�
	MemberVec	m_Members;
	//�ϴλ�õ��ߵĶ���
	ObjID_t	m_LastItemOwner;

	//����boss id
	UInt32	m_TeamBossId;
	//����boss����id
	UInt32	m_BossSceneId;
	//����bossλ��
	CLPosition	m_BossPos;
};

#endif
