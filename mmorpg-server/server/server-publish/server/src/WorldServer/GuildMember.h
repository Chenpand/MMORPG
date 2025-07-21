#ifndef __GUILD_MEMBER_H__
#define __GUILD_MEMBER_H__

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLGuildDefine.h>
#include <CLObjectDefine.h>
#include <CLMaskProperty.h>

class WSPlayer;
struct RaceSkillInfo;
class GuildMember : public CLSyncObject
{
	AVALON_DEFINE_OBJECTPOOL(GuildMember, Avalon::NullMutex)
public:
	GuildMember();
	~GuildMember();

	bool Init(WSPlayer* player);

public:
	/*
		Get/Set�ӿ�
		*/

	/**
	*@brief ����ID
	*/
	inline ObjID_t GetGuildID() const { return m_GuildId; }
	inline void SetGuildID(ObjID_t id) { m_GuildId = id; }

	/**
	*@brief ְ��
	*/
	inline GuildPost GetPost() const { return (GuildPost)(UInt8)m_Post; }
	void SetPost(GuildPost post, bool updateTitle = true);

	/**
	*@brief ����
	*/
	inline std::string GetName() { return m_Name; }
	inline void SetName(std::string name) { m_Name = name; }

	/**
	*@brief ְҵ
	*/
	inline UInt8 GetOccu() const { return m_Occu; }
	inline void SetOccu(UInt8 occu) { m_Occu = occu; }

	/**
	*@brief �ȼ�
	*/
	inline UInt16 GetLevel() const { return m_Level; }
	inline void SetLevel(UInt16 level) { m_Level = level; }

	/**
	*@brief VIP�ȼ�
	*/
	inline UInt8 GetVipLevel() const { return m_VipLevel; }
	inline void SetVipLevel(UInt8 level) { m_VipLevel = level; }

	/**
	*@brief �ۼƹ��׶�
	*/
	inline UInt32 GetTotalContribution() const { return m_TotalContri; }
	void AddTotalContribution(UInt32 num);
	void SetTotalContribution(UInt32 contri) { m_TotalContri = contri; }

	/**
	*@brief ����ʱ��
	*/
	inline UInt32 GetEnterTime() const { return m_EnterTime; }
	inline void SetEnterTime(UInt32 time) { m_EnterTime = time; }

	/**
	*@brief ����ʱ��
	*/
	inline UInt32 GetOfflineTime() const { return m_OfflineTime; }
	inline void SetOfflineTime(UInt32 time) { m_OfflineTime = time; }

	/**
	*@brief �ɼ��빫��ʱ��
	*/
	inline UInt32 GetLeaveTime() const { return m_LeaveTime; }
	inline void SetLeaveTime(UInt32 time) { m_LeaveTime = time; }

	/**
	*@brief ��Ҷ���
	*/
	inline WSPlayer* GetPlayer() const { return m_Player; }
	inline void SetPlayer(WSPlayer* player) { m_Player = player; }

	/**
	*@brief ���
	*/
	inline PlayerAvatar GetAvatar() const { return m_Avatar; }
	inline void SetAvatar(const PlayerAvatar& avatar) { m_Avatar = avatar; }

	/**
	*@brief ���û�ȡ��ұ�ǩ��Ϣ
	*/
	void GetPlayerLabelInfo(PlayerLabelInfo& info);

	/**
	*@brief ����ս��ʤ
	*/
	inline UInt32 GetGuildBattleWinStreak() const { return m_GuildBattleWinStreak; }
	inline void SetGuildBattleWinStreak(UInt32 winStreak) { m_GuildBattleWinStreak = winStreak; }
	inline void IncGuildBattleWinStreak() { m_GuildBattleWinStreak = m_GuildBattleWinStreak + 1; }

	/**
	*@brief ����ս����
	*/
	inline UInt32 GetGuildBattleScore() const { return m_GuildBattleScore; }
	inline void SetGuildBattleScore(UInt32 score) { m_GuildBattleScore = score; }

	/**
	*@brief ����ս����
	*/
	inline UInt32 GetGuildBattleNumber() const { return m_GuildBattleNumber; }
	inline void SetGuildBattleNumber(UInt32 number) { m_GuildBattleNumber = number; }

	/**
	*@brief ����ս�������
	*/
	inline std::string GetRewardMask() { return m_GuildBattleRewardMask.GetDBValue(); }

	/**
	*@brief �Ƿ���Ȩ��
	*/
	inline bool HasPower(GuildPowerFlag power) const { return HasGuildPower(GetPost(), power); }

	/**
	*@brief ���û��ֽ���
	*/
	inline bool CheckRewardMask(UInt32 boxId) const { return m_GuildBattleRewardMask.CheckMask(boxId); }
	inline void SetRewardMask(UInt32 boxId){ m_GuildBattleRewardMask.SetMask(boxId); }

	/**
	*@brief ���ù���սHP
	*/
	inline UInt32 GetGuildBattleHP() const { return m_GuildBattleHP; }
	inline void SetGuildBattleHP(UInt32 hp) { m_GuildBattleHP = hp; }

	/**
	*@brief ���ù���սMP
	*/
	inline UInt32 GetGuildBattleMP() const { return m_GuildBattleMP; }
	inline void SetGuildBattleMP(UInt32 mp) { m_GuildBattleMP = mp; }

	/**
	*@brief ���ù���ս����
	*/
	inline UInt32 GetGuildBattleInspire() const { return m_GuildBattleInspire; }
	inline void IncGuildBattleInspire() { m_GuildBattleInspire = m_GuildBattleInspire + 1; }

	/**
	*@brief ���ù���ս״̬
	*/
	inline GuildBattleMemberStatus GetBattleStatus() const { return m_BattleStatus; }
	inline void SetGuildBattleStatus(GuildBattleMemberStatus status) { m_BattleStatus = status; }

	/**
	*@brief ���ù����Ա�齱״̬
	*/
	inline UInt8 GetGuildBattleLotteryStatus() const{ return m_GuildBattleLotteryStatus; }
	inline void SetGuildBattleLotteryStatus(GuildBattleLotteryStatus status) { m_GuildBattleLotteryStatus = (UInt8)status; }

	/**
	*@brief ���ù���ս���ÿ�ս�����ȡ
	*/
	inline UInt32 GetGuildBattleTerrDayReward() const { return m_GuildTerrDayReward; }
	inline void SetGuildBattleTerrDayReward(UInt32 flag) { m_GuildTerrDayReward = flag; }

	/**
	*@brief ��Ծ��
	*/
	inline UInt32 GetActiveDegree() const { return m_ActiveDegree; }
	inline void SetActiveDegree(UInt32 value)
	{
		if (m_ActiveDegree == value) return;
		m_ActiveDegree = value;
		m_ActiveUpdateTime = (UInt32)CURRENT_TIME.Sec();
	}
	inline UInt32 GetActiveDegreeUpdateTime() const { return m_ActiveUpdateTime; }

	/**
	*@brief ���ö�λ
	*/
	inline UInt32 GetSeasonLevel() const { return m_SeasonLevel; }
	inline void SetSeasonLevel(UInt32 level) { m_SeasonLevel = level; }

	/**
	*@brief ���ûռ�
	*/
	inline UInt32 GetEmblemLevel() const { return m_EmblemLevel; }
	inline void SetEmblemLevel(UInt32 level) { m_EmblemLevel = level; }

	/**
	*@brief ��Ա��Ϣ
	*/
	GuildMemberEntry GetEntry();

	/**
	*@brief Բ����Ա��Ϣ
	*/
	GuildTableMember GetTableMember(UInt8 seat, UInt8 type);

	/**
	*@brief ͬ���¼�
	*/
	void SyncEventToScene(SyncEventType type, ObjID_t param1);

	/**
	*@brief �Ƿ��ڹ�����
	*/
	bool IsInGuild() const;

	/**
	*@brief �Ƿ��ܳ齱
	*/
	bool IsCanLottery() const;

public: //Э��	
	void SendProtocol(Avalon::Protocol& protocol);

public:	//����ս���

	//һ������ս����
	void OnGuildBattleRaceEnd(GuildBattleMember& guildBattleMember, bool isWin, UInt32 terrId, UInt32 hp = HP_MP_FULL_VALUE, UInt32 mp = HP_MP_FULL_VALUE);

	//����ս��ʼ
	void OnEnrollBegin();

	//���ù��輼��
	void SetInspireSkill(std::vector<RaceSkillInfo>& skills);

	//����
	UInt32 Inspire();

public:
	/* ���ݿ���� */
	void SaveToDB();
	void InsertToDB();
	void DelFromDB();

	CL_DBPROPERTY_BEGIN(CLSyncObject)
	CL_DBPROPERTY("guildid", m_GuildId)
	CL_DBPROPERTY("post", m_Post)
	CL_DBPROPERTY("name", m_Name)
	CL_DBPROPERTY("occu", m_Occu)
	CL_DBPROPERTY("level", m_Level)
	CL_DBPROPERTY("vip_level", m_VipLevel)
	CL_DBPROPERTY("totalcontri", m_TotalContri)
	CL_DBPROPERTY("entertime", m_EnterTime)
	CL_DBPROPERTY("offlinetime", m_OfflineTime)
	CL_DBPROPERTY("leavetime", m_LeaveTime)
	CL_DBPROPERTY("avatar", m_Avatar)
	CL_DBPROPERTY("battle_number", m_GuildBattleNumber)
	CL_DBPROPERTY("battle_score", m_GuildBattleScore)
	CL_DBPROPERTY("battle_winstreak", m_GuildBattleWinStreak)
	CL_DBPROPERTY("battle_reward_mask", m_GuildBattleRewardMask)
	CL_DBPROPERTY("battle_hp", m_GuildBattleHP)
	CL_DBPROPERTY("battle_mp", m_GuildBattleMP)
	CL_DBPROPERTY("battle_inspire", m_GuildBattleInspire)
	CL_DBPROPERTY("battle_lottery_status", m_GuildBattleLotteryStatus)
	CL_DBPROPERTY("active_degree", m_ActiveDegree);
	CL_DBPROPERTY("season_level", m_SeasonLevel);
	CL_DBPROPERTY("emblem_level", m_EmblemLevel);
	CL_DBPROPERTY("battle_terr_day_reward", m_GuildTerrDayReward);
	CL_DBPROPERTY_END()

private:
	//����ID
	ObjUInt64	m_GuildId;
	//ְλ(��Ӧö��GuildPost)
	ObjUInt8	m_Post;
	//����
	ObjString	m_Name;
	//ְҵ
	ObjUInt8	m_Occu;
	//�ȼ�
	ObjUInt16	m_Level;
	//vip�ȼ�
	ObjUInt8	m_VipLevel;
	//�ۼƹ��׶�
	ObjUInt32	m_TotalContri;
	//����ʱ��
	ObjUInt32	m_EnterTime;
	//����ʱ��
	ObjUInt32	m_OfflineTime;
	//�뿪����ʱ��
	ObjUInt32	m_LeaveTime;
	//��Ծ��
	ObjUInt32	m_ActiveDegree;
	//��Ծ�ȸ���ʱ��
	UInt32		m_ActiveUpdateTime;
	//���
	ObjPlayerAvatar	m_Avatar;
	//��Ҷ���
	WSPlayer*	m_Player;

	//����ս���
	//����ս����
	ObjUInt32	m_GuildBattleNumber;
	//����ս����
	ObjUInt32	m_GuildBattleScore;
	//����ս��ʤ
	ObjUInt32	m_GuildBattleWinStreak;
	//������ȡ���
	CLMaskProperty<GUILD_BATTLE_REWARD_SIZE> m_GuildBattleRewardMask;
	//����սʣ��HP
	ObjUInt32	m_GuildBattleHP;
	//����սʣ��MP
	ObjUInt32	m_GuildBattleMP;
	//����ս�������
	ObjUInt32	m_GuildBattleInspire;
	//����ս�齱״̬
	ObjUInt8	m_GuildBattleLotteryStatus;
	//����ս״̬
	GuildBattleMemberStatus	m_BattleStatus;
	//��λ
	ObjUInt32	m_SeasonLevel;
	//�ռ�
	ObjUInt32	m_EmblemLevel;
	//����ս���ÿ�ս�����ȡ
	ObjUInt32	m_GuildTerrDayReward;
};


#endif //__GUILD_MEMBER_H__