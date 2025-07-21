#ifndef _TCS_PLAYER_H_
#define _TCS_PLAYER_H_

#include <CLObject.h>
#include <CLProtocolDefine.h>
#include <CLItemDefine.h>
#include <kingnet_analyzer.h>
#include <udplog_typedef.h>
#include <CLChatProtocol.h>
#include <CLSysNotify.h>

#define PLAYERINFO(T) \
	"accid(" << T->GetAccID() << ") playerid(" << T->GetID() << ") name(" << T->GetName() << ") zoneid(" << T->GetZoneID() << ")"

struct TeamCopyInviteInfo;

// ����key
struct FlopKey
{
	FlopKey() : grade(0), stage(0) {}
	FlopKey(UInt32 _grade, UInt32 _stage) : grade(_grade), stage(_stage) {}
	UInt32 grade;
	UInt32 stage;

	bool operator < (const FlopKey& key) const
	{
		if (grade != key.grade)
		{
			return grade < key.grade;
		}
		else
		{
			return stage < key.stage;
		}

		return true;
	}
};

struct TCSPlayerData
{
	TCSPlayerData() : dayQuitTeamNum(0), weekQuitTeamNum(0) {}

	// ���˶Ӵ���
	UInt32 dayQuitTeamNum;
	// ���˶Ӵ���
	UInt32 weekQuitTeamNum;

	// �մ���(key->�Ѷȣ� val->����)
	std::map<UInt32, UInt32> dayNumMap;
	// �ܴ���(key->�Ѷȣ� val->����)
	std::map<UInt32, UInt32> weekNumMap;

	// ģʽ����(key->model, val->����)
	std::map<UInt32, UInt32> modelNumMap;

	// �������(key->model, val->����)
	std::map<UInt32, UInt32> extraNumMap;

	// roll����(key->model, val->����)
	std::map<UInt32, UInt32> rollNumMap;

	// �շ���(key->�׶Σ�val->���ƴ���)
	std::map<FlopKey, UInt32> dayFlopMap;
	// �ܷ���(key->�׶Σ�val->���ƴ���)
	std::map<FlopKey, UInt32> weekFlopMap;
};

namespace CLProtocol {
	class TeamCopyPlayerInfoSync;
	class TeamCopyPlayerInfoNotify;
};

/**
*@brief �ű����
*/
class TCSPlayer : public CLObject
{
	AVALON_DEFINE_OBJECTPOOL(TCSPlayer, Avalon::NullMutex)
public:

	TCSPlayer();
	~TCSPlayer();

	/**
	*@brief �ű�����
	*/
	inline void SetTeamType(UInt32 type) { m_TeamType = type; }
	inline UInt32 GetTeamType() { return m_TeamType; }

	/**
	*@brief zoneId
	*/
	inline void SetZoneID(UInt32 id) { m_ZoneId = id; }
	inline UInt32 GetZoneID() { return m_ZoneId; }
	
	/**
	*@brief �˺�
	*/
	inline void SetAccID(UInt32 id) { m_AccId = id; }
	inline UInt32 GetAccID() { return m_AccId; }

	/**
	*@brief ְҵ
	*/
	inline void SetOccu(UInt32 occu) { m_Occu = occu; }
	inline UInt32 GetOccu() { return m_Occu; }

	/**
	*@brief �ȼ�
	*/
	inline void SetLevel(UInt32 lvl) { m_Level = lvl; }
	inline UInt32 GetLevel() { return m_Level; }

	/**
	*@brief vip�ȼ�
	*/
	inline void SetVipLevel(UInt32 lvl) { m_VipLvl = lvl; }
	inline UInt32 GetVipLevel() { return m_VipLvl; }

	/**
	*@brief װ������
	*/
	inline void SetEquipScore(UInt32 score) { m_EquipScore = score; }
	inline UInt32 GetEquipScore() { return m_EquipScore; }

	/**
	*@brief ����Id
	*/
	inline void SetConnId(UInt32 id) { m_ConnId = id; }
	inline UInt32 GetConnId() { return m_ConnId; }

	/**
	*@brief �ű�Id
	*/
	inline void SetCopyId(UInt32 id) { m_CopyId = id; }
	inline UInt32 GetCopyId() { return m_CopyId; }

	/**
	*@brief ��ǰ��������
	*/
	inline void SetSceneType(UInt32 type) { m_SceneType = type; }
	inline UInt32 GetSceneType() { return m_SceneType; }

	/**
	*@brief ��ǰ����������
	*/
	inline void SetSceneSubType(UInt32 type) { m_SceneSubType = type; }
	inline UInt32 GetSceneSubType() { return m_SceneSubType; }

	/**
	*@brief ԭscene
	*/
	inline void SetSceneNodeId(UInt32 id) { m_SceneNodeId = id; }
	inline UInt32 GetSceneNodeId() { return m_SceneNodeId; }

	/**
	*@brief ԭworld
	*/
	inline void SetWorldNodeId(UInt32 id) { m_WorldNodeId = id; }
	inline UInt32 GetWorldNodeId() { return m_WorldNodeId; }

	/**
	*@brief ����id
	*/
	inline void SetGuildId(UInt64 id) { m_GuildId = id; }
	inline UInt64 GetGuildId() { return m_GuildId; }

	/**
	*@brief ����
	*/
	inline void SetAwaken(UInt32 awaken) { m_Awaken = awaken; }
	inline UInt32 GetAwaken() { return m_Awaken; }
	
	/**
	*@brief �˶Ӵ���
	*/
	UInt32 GetDayQuitTeamNum(UInt32 teamType);
	UInt32 GetWeekQuitTeamNum(UInt32 teamType);
	void AddQuitTeamNum(UInt32 teamType);

	/**
	*@brief �Ƿ�ʣ������˶Ӵ���
	*/
	bool IsOddFreeQuitTeamNum(UInt32 teamType);

	/**
	*@brief �����ս����
	*/
	void GMClearChallengeNum();

	/**
	*@brief GM���������ʸ�
	*/
	void GMOpenGoldTeam();

	/**
	*@brief GM����ج���ʸ�
	*/
	void GMOpenDiffGrade();

	/**
	*@brief ��Ʊ�Ƿ��㹻
	*/
	void SetTicketIsEnough(UInt32 teamType, UInt32 flag);
	UInt32 GetTicketIsEnough(UInt32 teamType);

	/**
	*@brief �Ƿ�loading
	*/
	inline void SetIsLoading(bool flag) { m_IsLoading = flag; }
	inline bool GetIsLoading() { return m_IsLoading; }

	/**
	*@brief ģʽ����
	*/
	void AddModelNum(UInt32 teamType, UInt32 model, UInt32 num);
	void SetModelNum(UInt32 teamType, UInt32 model, UInt32 num);
	UInt32 GetModelNum(UInt32 teamType, UInt32 model);
	UInt32 GetAllModelNum(UInt32 teamType);

	/**
	*@brief �Ѷ��մ���
	*/
	void AddDayNum(UInt32 teamType, UInt32 grade, Int32 num);
	void SetDayNum(UInt32 teamType, UInt32 grade, UInt32 num);
	UInt32 GetDayNum(UInt32 teamType, UInt32 grade);

	/**
	*@brief �Ѷ��ܴ���
	*/
	void AddWeekNum(UInt32 teamType, UInt32 grade, Int32 num);
	void SetWeekNum(UInt32 teamType, UInt32 grade, UInt32 num);
	UInt32 GetWeekNum(UInt32 teamType, UInt32 grade);

	/**
	*@brief ��ȡ�ܴ���
	*/
	UInt32 GetCfgTotalNum(UInt32 teamType, UInt32 teamGrade, bool isDay);

	/**
	*@brief �Ƿ�����ս����
	*/
	bool IsHasChallengeNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief ������ս����
	*/
	void AddChallengeNum(UInt32 teamType, UInt32 teamGrade);
	void ReturnChallengeNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief ��ȡ�������
	*/
	UInt32 GetExtraNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief ��ȡroll����
	*/
	void AddRollNum(UInt32 teamType, UInt32 teamGrade);
	UInt32 GetRollNum(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief �Ƿ���Դ�������
	*/
	bool IsCreateGoldTeam(UInt32 teamType);

	/**
	*@brief ���ƴ���
	*/
	void AddFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage, UInt32 crossDateType);
	UInt32 GetDayFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage);
	UInt32 GetWeekFlopNum(UInt32 teamType, UInt32 teamGrade, UInt32 stage);

	/**
	*@brief ���ּ�����Ҳ��ɷ��ƵĽ׶�
	*/
	void CalNotFlopStage(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief �Ƿ���Է���
	*/
	UInt32 IsCanFlop(UInt32 stage);

	/**
	*@brief ���㲻�ɷ��ƽ׶�
	*/
	bool IsUnlockGrade(UInt32 teamType, UInt32 teamGrade);

	/**
	*@brief ͬ�����ݵ�Scene
	*/
	void PlayerInfoToScene(UInt32 teamType);

	/**
	*@brief �·�����ű�����
	*/
	void PlayerInfoToClient(UInt32 teamType);

	/**
	*@brief �·�����ű�1����
	*/
	void GetTeamCopyOneInfo(UInt32 teamType, CLProtocol::TeamCopyPlayerInfoNotify& notify);

	/**
	*@brief �·�����ű�2����
	*/
	void GetTeamCopyTwoInfo(UInt32 teamType, CLProtocol::TeamCopyPlayerInfoNotify& notify);

	/**
	*@brief �ӽ���
	*/
	void AddRewards(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify = true);

	/**
	*@brief ��������
	*/
	void AddInvite(const TeamCopyInviteInfo& inviteInfo);

	/**
	*@brief ɾ������
	*/
	bool DelInviteInfo(UInt32 teamId);

	/**
	*@brief �·������б�
	*/
	void SendInviteList();

	/**
	*@brief ���͵��ͻ���
	*/
	void SendPacket(Avalon::Packet* packet);
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	*@brief ���͵�Scene
	*/
	void SendToScene(Avalon::Packet* packet);
	void SendToScene(Avalon::Protocol& protocol);

	/**
	*@brief ��������
	*/
	void SetPlayerData(CLProtocol::TeamCopyPlayerInfoSync& protocol);

	/**
	*@brief ����֪ͨ
	*/
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
		SendProtocol(notify);
	}

	/**
*@brief ����udplog
*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_TEAM_COPY_UID);
		if (NULL == pLogger) return;

		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, "", "", GetZoneID(), "", "",
			"", "", "", GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLevel(), (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, "", "", GetZoneID(), "", "",
			"", "", "", GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLevel(), (UInt32)GetOccu(), 0, 0, nTime, 0, 0, 0, 0, 0);
#endif

		pLogger->SetUserIP("");
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);
	}

private:

	/**
	*brief �ַ�������
	*/
	void _EncodeString(std::map<UInt32, UInt32>& strMap, const std::string& str);
	std::string _DecodeString(const std::map<UInt32, UInt32>& strMap);

	/**
	*brief �����ַ�������
	*/
	void _EncodeFlopString(std::map<FlopKey, UInt32>& strMap, const std::string& str);
	std::string _DecodeFlopString(const std::map<FlopKey, UInt32>& strMap);

private:

	// ���ڵ��ű�
	UInt32 m_TeamType;
	// zoneId
	UInt32 m_ZoneId;
	// �˺�Id
	UInt32 m_AccId;
	// ְҵ
	UInt32 m_Occu;
	// �ȼ�
	UInt32 m_Level;
	// vip
	UInt32 m_VipLvl;
	// װ������
	UInt32 m_EquipScore;
	// ����Id
	UInt32 m_ConnId;
	// �ű�id
	UInt32 m_CopyId;
	// ��ǰ��������
	UInt32 m_SceneType;
	// ��ǰ����������
	UInt32 m_SceneSubType;
	// ԭscene
	UInt32 m_SceneNodeId;
	// ԭworld
	UInt32 m_WorldNodeId;
	// ����id
	UInt64 m_GuildId;
	// ����
	UInt32 m_Awaken;
	// �Ƿ�loading
	bool m_IsLoading;

	// ��Ʊ�Ƿ��㹻
	std::map<UInt32, UInt32> m_TicketIsEnoughMap;

	std::map<UInt32, TCSPlayerData> m_TCDataMap;

	// ���ɷ��ƽ׶�(��ս��֮ǰ���)(key->�׶�, val->���ɷ���ԭ��)
	std::map<UInt32, UInt32> m_NotFlopMap;

	// ������Ϣ
	std::map<UInt32, TeamCopyInviteInfo> m_InviteMap;
};

#endif
