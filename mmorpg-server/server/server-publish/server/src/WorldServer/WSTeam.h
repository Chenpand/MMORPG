#ifndef _WS_TEAM_H_
#define _WS_TEAM_H_

#include <AvalonSimpleTimer.h>
#include <CLDefine.h>
#include <CLObject.h>
#include <CLTeamProtocol.h>
#include <vector>
#include <CLGameDefine.h>
#include <CLObjectDefine.h>

class WSPlayer;

/**
 *@brief �����Ա
 */
struct WSTeamMember
{
	WSTeamMember() :player(NULL), enterTime(0), id(0), level(0), occu(0), intimacyLevel(0), 
		remainTimes(0), isAssist(false), isAssistByPlayer(false), isRacing(false),
		guildId(0), vipLevel(0), resistMagic(0){}

    UInt8 GetStatusMask() 
    { 
        UInt8 mask = 0;
        if (player)
        {
            mask |= TEAM_MEMBER_MASK_ONLINE;
        }

		if (isAssist)
		{
			mask |= TEAM_MEMBER_MASK_ASSIST;
		}

		if (isRacing)
		{
			mask |= TEAM_MEMBER_MASK_RACING;
		}

        return mask;
    }
    
	void SetPlayer(WSPlayer* player);

	TeammemberBaseInfo GetBaseInfo()
	{
		TeammemberBaseInfo info;
		info.id = id;
		info.name = name;
		info.level = level;
		info.occu = occu;
		info.playerLabelInfo = playerLabelInfo;

		return info;
	}

	TeammemberInfo GetInfo()
	{
		TeammemberInfo info;
		info.id = id;
		info.name = name;
		info.level = level;
		info.occu = occu;
		info.statusMask = GetStatusMask();
		info.avatar = avatar;
		info.remainTimes = remainTimes;
		info.guildId = guildId;
		info.vipLevel = vipLevel;
		info.resistMagic = resistMagic;
		info.playerLabelInfo = playerLabelInfo;

		return info;
	}

	//���
	WSPlayer* player;
	//�������ʱ��
	UInt32	enterTime;
	//���id
	ObjID_t	id;
	//�����
	std::string	name;
	//�ȼ�
	UInt16	level;
	//ְҵ
	UInt8	occu;
	//���ʱ���ܶȼӳɼ���
	UInt8	intimacyLevel;
	//���
	PlayerAvatar avatar;
	//���³�ʣ�����
	UInt32	remainTimes;
	// �Ƿ���ս
	bool	isAssist;
	// ����Ƿ��ֶ���������ս״̬
	bool	isAssistByPlayer;
	// �Ƿ���ս����
	bool	isRacing;
	// ����ID
	ObjID_t	guildId;
	// vip�ȼ�
	UInt8	vipLevel;
	// ��ħֵ
	UInt32  resistMagic;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
};

typedef std::vector<WSTeamMember> MemberVec;
typedef CLVisitor<WSTeamMember> WSTeamMemberVisitor;

// ��ӿ�ʼ���³�ͶƱ
struct TeamDungeonStartVote
{
	TeamDungeonStartVote()
	{
		Clear();
	}

	void Clear()
	{
		dungeonId = 0;
		agreePlayerIds.clear();
		startTime = 0;
	}

	bool IsTimeout()
	{
		return dungeonId > 0 && startTime + 20 < (UInt32)CURRENT_TIME.Sec();
	}

	bool HasPlayer(ObjID_t id)
	{
		return std::find(agreePlayerIds.begin(), agreePlayerIds.end(), id) != agreePlayerIds.end();
	}

	// ���³�ID
	UInt32					dungeonId;
	// ����ͬ���˵����
	std::vector<ObjID_t>	agreePlayerIds;
	// ��ʼʱ��
	UInt32					startTime;
};

enum TeamStatus
{
	// ����
	TEAM_STATUS_IDEL,
	// ������
	TEAM_STATUS_RACING,
};

struct TeamInviteInfo
{
	TeamInviteInfo()
	{
		roleId = 0;
		endTime = 0;
	}

	ObjID_t	roleId;
	// �����ֹʱ��
	UInt32	endTime;
};

/**
 *@brief ����
 */
class WSTeam
{
	typedef std::map<UInt32, WSPlayer*>	ScenePlayerMap;

public:
	//��Ч�Ķ���ID
	const static UInt32 INVALID_TEAM_ID = 0;
	//�������
	const static UInt32 MAX_TEAM_SIZE = 4;
	//���˽�ɢ�ȴ�ʱ��
	const static UInt32 EMPTY_DISMISS_WAITTIME = 0;
    //������󳤶�
    const static UInt32 MAX_TEAM_NAME_LENGTH = 14;
    //������󳤶�
    const static UInt32 MAX_TEAM_PASSWD_LENGTH = 4;
	//����������Чʱ��
	const static UInt32 TEAM_INVITE_VALID_TIME = 60;

public:
	WSTeam(WSPlayer* master);
	WSTeam(WSPlayer* master, WSPlayer* member);
	~WSTeam();

	void OnTick(const Avalon::Time& now);

public:
	/**
	 *@brief ���û�ȡid
	 */
	void SetId(UInt32 id){ m_Id = id; }
	UInt32 GetId() const{ return m_Id; }

	/**
	*@brief ������
	*/
	void SetNumber(UInt16 number) { m_number = number; }
	UInt16 GetNumber() const { return m_number; }

	void SetStatus(TeamStatus status);
	TeamStatus GetStatus() const;

    /**
    *@brief ����Ŀ��
    */
    void SetTeamTarget(UInt32 target) { m_target = target; }
	UInt32 ChangeTeamTarget(UInt32 target);
    UInt32 GetTeamTarget() const { return m_target; }
	void SetTargetDungeonID(UInt32 id) { m_TargetDungeonId = id; }
	UInt32 GetTargetDungeonID() const { return m_TargetDungeonId; }

	/**
	*@brief �Ƿ�ƥ�䵽
	*/
	void SetIsMatched(bool isMatched) { m_IsMatched = isMatched; }
	bool GetIsMatched() const { return m_IsMatched; }

	/** 
	 *@brief �ӳ�
	 */
	void SetMaster(ObjID_t master){ m_Master = master; }
	ObjID_t GetMaster() const{ return m_Master; }

	/**
	*@brief �Զ�ͬ��
	*/
	void SetAutoAgree(bool agree) { m_AutoAgree = agree; }
	bool IsAutoAgree() const { return m_AutoAgree; }

	/**
	*@brief ������
	*/
	void SetOption(TeamOption option) { m_OptionMask |= option; }
	void ClearOption(TeamOption option) { m_OptionMask &= (~option); }
	bool IsSetOption(UInt32 option) const { return (m_OptionMask & option) > 0; }

	/**
	 *@brief �ж϶����Ƿ�����
	 */
	bool IsFull() const{ return GetSize() >= GetMaxTeamMemberNum(); }

	/**
	 *@brief �ж϶����Ƿ�ͶƱ��
	 */
	bool IsInVote() { return m_Vote.dungeonId > 0; }

	/**
	 *@brief ����Ƴ���Ա
	 */
	void AddMember(WSPlayer* player, bool sendNotify, bool cancelMatch = true);
	void RemoveMember(ObjID_t playerId);

	void VisitMembers(WSTeamMemberVisitor& visitor);
	std::vector<WSPlayer*> GetAllMembers();

    /**
    *@brief ��ȡ���������Ϣ, �������Ϊ0������ӳ�
    */
    TeamBaseInfo GetBaseInfo(UInt64 fromId = 0);

	/**
	*@brief �޸������Ϣ
	*/
	void UpdateAvatar(ObjID_t id, const PlayerAvatar& avatar);

	/**
	*@brief �޸���ս״̬
	*/
	void ChangeMemberAssistStatus(ObjID_t id, bool isAssist, bool modifyByPlayer = false);

	/**
	*@brief �޸Ĺ���ID
	*/
	void ChangeMemberGuildID(ObjID_t id, ObjID_t guildId);

	/**
	*@brief �޸���ҵ��³�ʣ�����
	*/
	void OnChangeRemainTimes(ObjID_t id, UInt32 targetId, UInt32 remainTimes);
	void SetMemberRemainTimes(ObjID_t id, UInt32 remainTimes);

	/**
	*@brief �޸��������ս���е�״̬
	*/
	void OnChangePlayerRacingStatus(ObjID_t id, bool racing);

	/**
	*@brief ����Ŀ���Ƿ��д�������
	*/
	bool IsTargetTimesLimit();

	/**
	*@brief ��ȡ������͵ȼ�
	*/
	UInt16 GetMinLevelLimit();

	/**
	*@brief ���¿�ħֵ
	*/
	void OnPlayerResistMageicChanged(WSPlayer* player);
public:
	/**
	*@brief ����������
	*/
	UInt32 RequestJoinTeam(WSPlayer* player, const std::string& passwd);

	/**
	 *@brief ��ɢ����
	 */
	bool Dismiss(WSPlayer* player);
	void Dismiss();

	/**
	 *@brief ��ȡ��ɢʱ��
	 */
	UInt32 GetDismissTime() const{ return m_DismissTime; }

	/**
	 *@brief ��ȡ����
	 */
	UInt32 GetSize() const{ return UInt32(m_Members.size()); }

	/**
	 *@brief ת�ƶӳ�
	 */
	void TransferMaster(WSPlayer* player, ObjID_t id);

	/**
	 *@brief ���ҳ�Ա
	 */
	WSTeamMember* FindMember(ObjID_t id);

    /**
    *@brief ����޸Ķ���ѡ��
    */
    void OnPlayerChangeTeamOption(WSPlayer* player, TeamOptionOperType type, std::string str, UInt32 param1 = 0, UInt32 param2 = 0);

	/**
	*@brief ����޸Ķ�����սѡ��
	*/
	void OnPlayerChangeAssist(WSPlayer* player, bool isAssist);

public: // ���³�ͶƱ
	/**
	*@brief ��ʼ���³�ͶƱ
	*/
	UInt32 OnStartRaceVote(UInt32 dungeonId);

	/**
	*@brief ����ϴ���ʼ���³�ͶƱ
	*/
	void OnPlayerReportVote(WSPlayer* player, bool agree);

	/**
	*@brief ������Ҷ�ͬ��
	*/
	void OnStartRaceVoteAllAgree();

	/**
	*@brief ����Ҿܾ�
	*/
	void OnStartRaceVoteFailure(ObjID_t roleId = 0, UInt32 noticeId = 0);

	/**
	*@brief ��scene���Ϳ�ʼ��������
	*/
	void SendStartRaceToScene(UInt32 dungeonId, SceneNpc cityMonster);

public:
	void UpdateSceneInfo();
	void SyncTeamInfo();
	void SyncTeamBaseToScene();

public: // �����б����
	/**
	*@brief ���������
	*/
	void AddRequester(WSPlayer* player);

	/**
	*@brief ��ȡ������
	*/
	TeamRequester* FindRequester(ObjID_t id);

	/**
	*@brief ɾ��������
	*/
	void DelRequester(ObjID_t id);

	/**
	*@brief ��������б�
	*/
	void ClearRequesterList(WSPlayer* player);

	/**
	*@brief �ظ���������
	*/
	void ReplyJoinRequest(WSPlayer* player, ObjID_t targetId, bool agree);

	/**
	*@brief ��ȡ�������б�
	*/
	const std::vector<TeamRequester>& GetRequesterList() const { return m_Requesters; }

	/**
	*@brief ��ȡ�����Ա�б�
	*/
	const MemberVec& GetMemberVec() { return m_Members; }

public: // �����������
	/**
	*@brief ��ӵ������б�
	*/
	void AddInvitePlayer(WSPlayer* player);

	/**
	*@brief �������б���ɾ��
	*/
	void DelInvitePlayer(ObjID_t id);

	/**
	*@brief ����Ƿ��������б���
	*/
	bool IsInInviteList(ObjID_t id);

	/**
	*@brief ��������б�
	*/
	void ClearInviteList(bool notify = true);

	/**
	*@brief ͬ���������Ը���������
	*/
	void SyncTeamAttrToInvitePlayer();

	/**
	*@brief �㲥�������������
	*/
	void BroadcastToInvitePlayer(Avalon::Protocol& protocol);

public: //��Ϣ���
	/**
	 *@brief �㲥����Ա���ڳ���
	 */
	void BroadcastToScene(Avalon::Protocol& protocol);

	/** 
	 *@brief �㲥�����г�Ա
	 */
	void Broadcast(Avalon::Protocol& protocol);

	/**
	*@brief �㲥�����˶ӳ�֮��ĳ�Ա
	*/
	void BroadcastExceptMaster(Avalon::Protocol& protocol);

	/**
	 *@brief �㲥��������г�Ա
	 */
	void Notify(UInt16 type, UInt8 color, const char* word);

	/**
	*@brief �㲥��Ա״̬
	*/
	void BroadcastMemberStatus(ObjID_t id);

public: //�¼�
	/**
	 *@brief ���������
	 */
	bool OnPlayerOnline(WSPlayer* player);
	void OnPlayerOffline(WSPlayer* player);

	/**
	 *@brief �����������л�
	 */
	void OnSceneServerChanged(WSPlayer* player);

	/**
	 *@brief �ȼ��ı�
	 */
	void OnPlayerLevelChanged(WSPlayer* player);

	/**
	*@brief ְҵ�ı�
	*/
	void OnPlayerOccuChanged(WSPlayer* player);

	/**
	*@brief vip�ȼ��ı�
	*/
	void OnPlayerVipLevelChanged(WSPlayer* player);

	/**
	 *@brief ��ɢ
	 */
	void OnDismissed();

	/**
	*@brief �µ�һ��
	*/
	void OnDayChanged();

	/**
	*@brief ��ȡ�����������
	*/
	UInt8 GetMaxTeamMemberNum() const;
	void SetMaxTeamMemberNum(UInt8 num) { m_maxMemberNum = num; }

	/**
	*@brief ��ȡʣ�����
	*/
	void QueryRemainTimes(ObjID_t roleId, UInt32 targetId);
	void QueryAllMemberRemainTimes(UInt32 targetId);

	/**
	*@brief ֪ͨ���Զ�Ա��ɢ����ʱ״̬
	*/
	void BroadcastTeamKick(bool status);

	/**
	*@brief ���ƽ�ɢ����
	*/
	void ControlKickMaster(bool bo);

private:
	void FillTeamInfo(CLProtocol::WorldSyncTeamInfo& sync);
	void FillSceneTeamInfo(CLProtocol::WorldSyncTeamInfoToScene& sync);
	void JudgeMaster();
	void ResetKickMasterTimer();
	
private:
	//����id
	UInt32	m_Id;
	//�����ţ����ڿͻ�����ʾ��
	UInt16	m_number;
	//����״̬
	TeamStatus m_Status;
	//�ӳ�
	ObjID_t	m_Master;
    // ����Ŀ��
    UInt32	m_target;
	// �������
	UInt32	m_maxMemberNum;
	//��ɢʱ��
	UInt32	m_DismissTime;
	//�����Ա�б�
	MemberVec	m_Members;
	//���������б�
	ScenePlayerMap	m_ScenePlayers;
	//�����б�
	std::vector<TeamRequester>	m_Requesters;
	//�Ƿ��Զ�ͬ��
	bool		m_AutoAgree;
	//����ѡ��(��ӦTeamOption��λ����)
	UInt32						m_OptionMask;
	//��ʼ����ͶƱ
	TeamDungeonStartVote	m_Vote;
	//�����б�
	std::vector<TeamInviteInfo> m_InviteList;
	//Ŀ����³�
	UInt32						m_TargetDungeonId;
	//�Ƿ��Ѿ���ƥ�䵽
	bool                        m_IsMatched;
	//�����ɢת�ƶӳ���ʱ��
	Avalon::SimpleTimer         m_KickMasterTimer;
	//�����ɢת�ƶӳ�����
	bool						m_KickMasterControl;
	//�����ɢ�㲥��ʱ��
	Avalon::SimpleTimer         m_KickMasterBroadcastTimer;
};

typedef CLVisitor<WSTeam> WSTeamVisitor;

#endif
