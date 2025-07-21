#ifndef _GS_PLAYER_H_
#define _GS_PLAYER_H_

#include <CLSceneObject.h>
#include <AvalonProtocol.h>
#include <AvalonNetConnection.h>
#include <AvalonSimpleTimer.h>
#include <set>
#include <kingnet_analyzer.h>
#include <CLDefine.h>
#include <CLGameDefine.h>
#include <CLMaskProperty.h>

#include "PlayerPacketBuffer.h"

/**
 *@brief �������״̬
 */
enum GatePlayerStatus
{
	GPS_VERIFY,			//��֤�׶�
	GPS_QUEUE,			//�Ŷӵȴ���
	GPS_WAITROLEINFO,	//�ȴ���ɫ��Ϣ
	GPS_CREATEROLE,		//������ɫ������
	GPS_SELECTROLE,		//ѡ���ɫ
	GPS_ENTERGAME,		//������Ϸ�׶�
	GPS_NORMAL,			//������Ϸ�׶�
    GPS_OFFLINE,        //���߽׶�
	GPS_RECONNECT,		//��������
	GPS_REPLACE,		//����
	GPS_FINAL,			//��ֹ�׶�
};

/**
*@brief ������Ϸ��ʽ
*/
enum EnterGameMode
{
	EGM_NORMAL = 0,		//һ������
	EGM_ROLE_SWITCH,	//��ɫ�л�
};

#define PLAYERINFO(T) "accid(" << T->GetAccID() << ")"

/**
 *@brief  ��ɫ��Ϣ
 */
struct GSRoleInfo
{
	GSRoleInfo()
		:id(0), mapid(0), sceneid(0), sex(0), occu(0), level(0), tourZoneId(0), bornZoneId(0), createtime(0), offlineTime(0), deleteTime(0), newBoot(0), preOccu(0), isAppointmentOccu(0), returnTime(0), returnLevel(0), addPreferencesTime(0),delPreferencesTime(0){}

	// ��ɫ�Ƿ��Ѿ�������ɾ��
	bool IsForeverDeleted()
	{
		if (deleteTime == 0)
		{
			return false;
		}

		if (level < DIRECT_DELETE_PLAYER_LEVEL || deleteTime + DELETE_PLAYER_SAVE_TIME < CURRENT_TIME.Sec())
		{
			return true;
		}

		return false;
	}

	//Ψһid
	ObjID_t		id;
	//��ɫ��
	std::string	name;
	//��ͼid
	UInt32		mapid;
	//����id
	UInt32		sceneid;
	//λ��x
	CLPosition  pos;
	//�Ա�
	UInt8		sex;
	//ְҵ
	UInt8		occu;
	//�ȼ�
	UInt16		level;
	//������
	UInt32		tourZoneId;
	//ԭ��
	UInt32		bornZoneId;
	//����ʱ��
	UInt32		createtime;
	//����ʱ��
	UInt32		offlineTime;
	//ɾ��ʱ��
	UInt32		deleteTime;
	//���
	PlayerAvatar avatar;
	//��������
	UInt32		newBoot;
	//Ԥתְ
	UInt8		preOccu;
	//�Ƿ���ԤԼ��ɫ
	UInt8		isAppointmentOccu;
	//�ع�ʱ��
	UInt32		returnTime;
	//�ع�ȼ�
	UInt32		returnLevel;
	//��ұ�ǩ��Ϣ
	PlayerLabelInfo playerLabelInfo;
	//����ƫ��ʱ��
	UInt32 addPreferencesTime;
	//ȡ��ƫ��ʱ��
	UInt32 delPreferencesTime;
};

class GSScene;

/**
 *@brief �������  noted by aprilliu, GSPlayer�� object id ���Ǵ�����ɫ��id��һ��guid
 */
class GSPlayer : public CLSceneObject
{
public:
	typedef std::list<GSRoleInfo>	RoleList; 
	typedef std::set<ObjID_t>		ObjIDSet;

public:
	GSPlayer();
	~GSPlayer();

	/**
	 *@brief ��ʼ��
	 *@param accid �˺�id
	 */
	bool Init(UInt32 accid, UInt8 gmauthority, const std::string& openid, 
		const QQVipInfo& info, const std::string& source1, const std::string& source2, const std::string& pf, const std::string& did,
		const std::string& model, const std::string& deviceVersion);

	/**
	*@brief �����˺�guid
	*/
	void SetAccountGuid(DBKey_t guid) { m_AccountGuid = guid; }
	DBKey_t GetAccountGuid() { return m_AccountGuid; }

	/**
	 *@brief ��ȡ�˺�id
	 */
	UInt32 GetAccID() const { return m_Accid; }

	/**
	 *@brief ��ȡgmȨ��
	 */
	UInt8 GetGmAuthority() const { return m_GmAuthority; }
	void SetGmAuthority(UInt8 flag) { m_GmAuthority = flag; }

	/**
	*@brief �ֻ��󶨵Ľ�ɫID
	*/
	ObjID_t GetPhoneBindRoleID() const { return m_PhoneBindRoleId; }
	void SetPhoneBindRoleID(ObjID_t id) { m_PhoneBindRoleId = id; }

	/**
	 *@brief ���ý�ɫɾ��ʱ��
	 */
	void SetRoleDeleteTime(UInt32 time) { m_RoleDeleteTime = time; }

	/**
	*@brief ���ý�ɫ�ָ�ʱ��
	*/
	void SetRoleRecoverTime(UInt32 time) { m_RoleRecoverTime = time; }

	/**
	*@brief ��ȡ��ɫɾ��/�ָ� ������Ϣ
	*/
	std::string GetRoleUpdateLimitInfo() const { return m_RoleUpdateLimitInfo; }

	/**
	 *@brief ��ȡobject����
	 */
	SceneObjectType GetType() const{ return SOT_PLAYER; }

	/**
	 *@brief ����Դ��id                                                                     
	 */
	void SetSrcZoneId(UInt32 srcZoneId){ m_SrcZoneId = srcZoneId; }
	UInt32 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	 *@brief ���û�ȡ�ȼ�
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief ��ȡopenid                                                                     
	 */
	const std::string& GetOpenId() const{ return m_OpenId; }

	/**
	 *@brief ��ȡqq����ȼ�                                                                     
	 */
	const QQVipInfo& GetQQVip() const{ return m_qqVipInfo; }

	/**
	 *@brief ��ȡip                                                                     
	 */
	const std::string& GetIp() const{ return m_Ip; }

	/**
	*@brief �ܷ������Ϸ
	*/
	bool CanEnterGame() const { return m_CanEnterGame; }
	void SetCanEnterGame(bool can) { m_CanEnterGame = can; }

	/**
	 *@brief ��ȡ��Դ                                                                     
	 */
	const std::string& GetSource1() const{ return m_Source1; }
	const std::string& GetSource2() const{ return m_Source2; }

	const std::string& GetPf() const{ return m_Pf; }

	const UInt8 GetStorageSize() const { return m_StorageSize; }
	void SetStorageSize(UInt8 sz) { m_StorageSize = sz; }

    inline void SetConn(Avalon::NetConnectionPtr conn) { m_Conn = conn; }
	inline Avalon::NetConnectionPtr GetConn() const { return m_Conn; }

	void SetLastRecvSequence(UInt32 sequence);
	UInt32 GetLastRecvSequence() const { return m_LastRecvSequence; }

	void ClearPacketBuffer() { m_packetBuffer.Clear(); }
	const PlayerPacketBuffer& GetPlayerPacketBuffer() const { return m_packetBuffer; }

	void SetMonitored(bool monitored);
	bool IsMonitored() const { return m_IsMonitored; }

	/**
	*@brief ð�ն�
	*/
	void SetAdventureTeamInfo(const std::string& name, UInt16 level, UInt64 exp);
	const AdventureTeamInfo& GetAdventureTeamInfo() const { return m_AdventureTeamInfo; }

	/**
	*@brief ����չ��ɫ��λ��������
	*/
	void SetUnlockedExtensibleRoleField(UInt32 num) { m_UnlockeExtensibleRoleField = num; }
	UInt32 GetUnlockedExtensiblRoleField() const { return m_UnlockeExtensibleRoleField; }

	/**
	*@brief �˺Żع�ʱ��
	*/
	void SetAccountReturnTime(UInt32 time) { m_AccountReturnTime = time; }
	UInt32 GetAccountReturnTime() const { return m_AccountReturnTime; }

	/**
	*@brief ��ɫ�ع�ʱ��
	*/
	void SetRoleReturnTime(UInt32 time) { m_RoleReturnTime = time; }
	UInt32 GetRoleReturnTime() const { return m_RoleReturnTime; }

	/**
	*@brief ��ɫ�ع�ȼ�
	*/
	void SetRoleReturnLevel(UInt32 lvl) { m_RoleReturnLevel = lvl; }
	UInt32 GetRoleReturnLevel() const { return m_RoleReturnLevel; }

	/**
	*@brief �Ƿ��ٴλع�
	*/
	void SetAgainReturn(bool isAgain) { m_AgainReturn = isAgain ? 1:0; }
	UInt8 GetAgainReturn() const { return m_AgainReturn; }

	/**
	*@brief �Ƿ��ڻع��ڼ�
	*/
	bool IsVeteranReturn();

	/**
	*@brief ��ɫ�б�
	*/
	const RoleList& GetRoleList() { return m_Roles; }

	/**
	*@brief �رռ�ؼ�¼���ļ�
	*/
	void CloseMonitorFile();

public:
	/**
	 *@brief ���һ����ɫ
	 */
	bool AddRole(const GSRoleInfo& info);

	/**
	 *@brief ɾ��һ����ɫ                                                                     
	 */
	UInt32 DeleteRole(ObjID_t roleId);

	/**
	 *@brief �ָ�һ����ɫ                                                                     
	 */
	UInt32 RecoverRole(ObjID_t roleId);

	/**
	 *@brief ��ȡ��ɫ��
	 */
	UInt32 GetRoleNum() const;

	/**
	*@brief ��ȡ�Ѿ�ɾ���Ľ�ɫ���������Ѿ�����ɾ���ģ�
	*/
	UInt32 GetDeletedRoleNum();

	/**
	*@brief ��ȡ���մ�����ɫ����
	*/
	UInt32 GetTodayCreatedRoleNum();

	/**
	*@brief �ܷ񴴽��µĽ�ɫ
	*/
	UInt32 CanCreateNewRole();

	/**
	*@brief ����ɫ��λ�Ƿ�ﵽ����
	*/
	bool RoleFieldReachUpperLimit();
	/**
	 *@brief ����ƫ��
	 */
	UInt32 SetPreferences(ObjID_t roleId, RolePreferences flag);
	/**
	 *@brief ��ȡƫ�������
	 */
	UInt32 GetPreferencesRoleNum();
public:
	void StartToQueryRoleInfo(UInt8 enterGameMode = 0);

	/**
	 *@brief ���û�ȡ���ڵĳ���
	 */
	void SetScene(GSScene* scene){ m_pScene = scene; }
	GSScene* GetScene() const { return m_pScene; }

	/**
	 *@brief ���û�ȡ״̬
	 */
	void SetStatus(GatePlayerStatus status);
	GatePlayerStatus GetStatus() const { return m_Status; }

	/**
	*@brief ��ѯ�˺���Ϣ
	*/
	void BeginQueryAccountInfo(UInt8 enterGameMode = 0);

	/**
	 *@brief ��ʼ������Ϸ
	 */
	bool BeginEnterGame(ObjID_t roleId, UInt8 option, const std::string& city, UInt32 inviter);
	bool BeginEnterGameForTour(UInt8 option, const std::string& city, UInt32 inviter);

	/**
	*@brief ������������ؿ�
	*/
	void OnFinishNewbeeGuide(ObjID_t roleId, UInt32 id);

	/**
	 *@brief ����������Ϸ
	 */
	void EndEnterGame();

	/**
	 *@brief ���û�ȡ�ϴ����߳�����λ��
	 */
	void SetLastMapID(UInt32 mapId){ m_LastMapID = mapId; }
	UInt32 GetLastMap() const{ return m_LastMapID; }
	UInt32 GetLastScene() const{ return m_LastSceneID; }
	const CLPosition& GetLastPos() const{ return m_LastPos; }

	void SetSession(std::vector<UInt8> session) { m_Session = session; }
	std::vector<UInt8> GetSession() { return m_Session; }

	/**
	 *@brief ���ͽ�ɫ��Ϣ
	 */
	void SendRoleInfo();

	/**
	*@brief ���ͽ�ɫ��Ϣ��Center
	*/
	void SendRoleInfoToCenter();

	/**
	 *@brief ������                                                                     
	 */
	void CheckChangeZone(UInt32 dstZoneId, UInt32 dstMapId);

    /**
    *@brief �������߶�ʱ��
    */
    void SetOfflineTimer(UInt32 sec);

    /**
    *@brief �Ƿ����߳�ʱ
    */
    bool IsOfflineTimeout(const Avalon::Time& now);

	/**
	*@brief ֪ͨworld server����뿪��Ϸ,��world ֪ͨscene server �����������߼�
	*/
	void NotifyLeaveGame(ObjID_t roleid, UInt8 isOfflineTimeout = 0);

	/**
	*@brief ֪ͨworld server����뿪��Ϸ
	*/
	void NotifyDisconnect();

	/**
	*@brief ֪ͨworld server�����ɹ�
	*/
	void NotifyReconnectSuccess();

	/**
	*@brief ͬ��ð�ն���Ϣ
	*/
	void SyncAdventureTeamInfo();

	// ͬ��ϵͳ����
	void NotifySysSwitch();

public://��ϵ���
	/**
	 *@brief ���ú�����
	 */
	void SetBlackList(const std::vector<ObjID_t>& blacklist){ m_BlackList.insert(blacklist.begin(), blacklist.end()); }

	/**
	 *@brief ����ɾ��������
	 */
	void AddBlackList(ObjID_t id){ m_BlackList.insert(id); }
	void RemoveBlackList(ObjID_t id){ m_BlackList.erase(id); }

	/**
	 *@brief �ж��Ƿ��ں�������
	 */
	bool IsInBlackList(ObjID_t id) const{ return m_BlackList.find(id) != m_BlackList.end(); }

public:// ԤԼְҵ���
	/**
	*@brief ����ɫ�Ƿ񻹴���ԤԼ״̬��
	*/
	void CheckRoleAlreadyInAppointment(GSRoleInfo& info);

	/**
	*@brief ��ȡԤԼ��ɫ����
	*/
	UInt32 GetAppointmentRoleNum();

public:// �ع�

	/**
	*@brief ���»ع�ʱ��
	*/
	void UpdateRoleReturnInfo(UInt64 roleId, UInt32 time);
	void UpdateAccountReturnTime();

public:// ð�ն�(Ӷ����)

	/**
	*@brief �Ƿ���Ҫ������������ѡ��
	*/
	bool NeedPopupNoviceGuideChoose();

	/**
	*@brief ������������ѡ���־
	*/
	void SetNoviceGuideChooseFlag(ObjID_t roleid, UInt8 chooseFlag);

	/**
	*@brief ֪ͨ���¼����ɫ������
	*/
	void NotifyRecalculateTotalRoleValueScore(const std::string& reason);

public://�¼�
	
	void OnTick(const Avalon::Time& now);

	void NotifyNodeRoleConnect(bool isLeave = true);
	
public: //��־

	/**
	*@brief ����udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_GATE_UID);
		if (NULL == pLogger) return;

		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		UInt16 zoneId = (GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID);
		UInt32 bronZoneId = m_BornZoneId;
		UInt32 isReturn = IsVeteranReturn() ? 1 : 0;

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), 0, (UInt32)m_Occu, 0, 0, nTime, bronZoneId, isReturn, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), 0, (UInt32)m_Occu, 0, 0, nTime, bronZoneId, isReturn, 0, 0, 0);
#endif

		pLogger->SetUserIP(m_Ip.c_str());
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);

	}

	void SendDeleteUdpLog(UInt32 deleteTime);

	void SendCreateUdpLog(std::string name, UInt8 occu, ObjID_t roleId, UInt8 preOccu, UInt8 isAppointMentOccu);

	void SendAccountLoginUdpLog();

	void SendRolePreferencesUdpLog(UInt32 isPreferences, UInt32 roleLocation);
public://����

	/**
	 *@brief ����һ��Э��
	 */
	void SendProtocol(Avalon::Protocol& protocol);

	/**
	 *@brief ����һ����
	 */
	void SendPacket(Avalon::Packet* packet);

	/**
	 *@brief ���͵����ڳ���
	 */
	void SendToSceneServer(Avalon::Protocol& protocol);

	/**
	 *@brief ת��������
	 */
	void TransToSceneServer(Avalon::Protocol& protocol);

	/**
	 *@brief ת����world
	 */
	void TransToWorldServer(Avalon::Protocol& protocol);

	/**
	*@brief ת����globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	/**
	*@brief ת����Battle
	*/
	void SendToBattle(Avalon::Protocol& protocol);
	void SendToBattle(Avalon::Packet* packet);

	/**
	*@brief ת����TeamCopy
	*/
	void SendToTeamCopy(Avalon::Protocol& protocol);
	void SendToTeamCopy(Avalon::Packet* packet);

	/**
	*@brief ת����Union
	*/
	void SendToUnion(Avalon::Protocol& protocol);
	void SendToUnion(Avalon::Packet* packet);

	

	/**
	 *@brief �Ͽ�����
	 */
	void Disconnect();

	/**
	*@brief ����
	*/
	UInt32 BeginReconnect(Avalon::NetConnectionPtr conn, std::vector<UInt8> session, UInt32 sequence);

	/**
	*@brief ����Ϣ��¼���ļ�
	*/
	void RecordRecord(Avalon::Packet* packet, bool isRecv);

private:
	void EnterGame();
	UInt32 GenPacketSequence() { return ++m_packetSequenceSeed; }

	/**
	*@brief ��ȡ��ǰ���ڵĽ�ɫ����ߵȼ�
	*/
	UInt32 GetRoleMaxLevel();

	/**
	*@brief �ж��Ƿ����ƽ�ɫɾ����ָ�
	*/
	bool IsLimitRoleUpdate(UInt32 updateType, UInt32 intervalTime, UInt32 roleUpdateLimitTime);

	/**
	*@brief ��ʱ��ת��Ϊ�ַ���
	*/
	std::string TimeToStr(UInt32 secs);

	/**
	*@brief �����Ϣͳ�Ƶ��ļ�
	*/
	void RecordPacketCountToFile();

	/**
	*@brief ͳ����Ϣ��
	*/
	void PacketCount(UInt32 packetId, UInt32 packetSize);

private:
	//�˺�guid
	DBKey_t						m_AccountGuid;
	//�˺�id
	UInt32						m_Accid;
	//gmȨ��
	UInt8						m_GmAuthority;
	//����
	std::string					m_City;
	//ip��ַ
	std::string					m_Ip;
	//openid
	std::string					m_OpenId;
	//qq��Ȩ��Ϣ
	QQVipInfo					m_qqVipInfo;
	//��Դ1
	std::string					m_Source1;
	//��Դ2
	std::string					m_Source2;
	//������
	UInt32						m_Inviter;
	//ƽ̨
	std::string					m_Pf;
	//�豸ID
	std::string					m_DeviceId;
	//�豸�ͺ�
	std::string					m_Model;
	//�豸�汾
	std::string					m_DeviceVersion;

	//ԭ��id
	UInt32						m_SrcZoneId;
	//������id
	UInt32						m_TourZoneId;
	//ԭ��id
	UInt32						m_BornZoneId;
	//�ȼ�
	UInt16						m_Level;
	//ְҵ
	Occupation					m_Occu;

	//����
	Avalon::NetConnectionPtr	m_Conn;
	//״̬
	GatePlayerStatus			m_Status;
	//��ɫ�б�
	RoleList					m_Roles;
	//�ͻ���ѡ��
	UInt8						m_ClientOption;
	//�ϴ����ߵ�ͼ
	UInt32						m_LastMapID;
	//�ϴ����߳���
	UInt32						m_LastSceneID;
	//�ϴ�����λ��
	CLPosition					m_LastPos;
	//������Ϸ���Դ���
	UInt8						m_TryTimes;
	//�붨ʱ��
	Avalon::SimpleTimer			m_SecTimer;

    //���߶�ʱ��
    Avalon::SimpleTimer			m_offlineTimer;

	//10�붨ʱ��
	Avalon::SimpleTimer			m_TenSecTimer;

	//���Ӷ�ʱ��
	Avalon::SimpleTimer			m_MinSecTimer;

	//���ڳ���
	GSScene*					m_pScene;

	//������
	ObjIDSet					m_BlackList;

	//�ֿ��С
	UInt8						m_StorageSize;

	//��Ϣ�������
	UInt32						m_packetSequenceSeed;

	//��ϢBuffer
	PlayerPacketBuffer			m_packetBuffer;

	// ��Ϣ����
	HashMap<UInt32, PacketStatistics>		m_packetCountMap;

	//Session
	std::vector<UInt8>			m_Session;

	//���һ���յ�����Ϣ���
	UInt32						m_LastRecvSequence;

	//�Ƿ񱻼��
	bool						m_IsMonitored;

	//��¼�����Ϣ�Ļ�����Ϣ���ļ�
	FILE*						m_NetSimpleFile;
	//��¼��Ҿ�����Ϣ���ļ�
	FILE*						m_NetDetailFile;
	//��¼��ҵ���Ϣͳ��
	FILE*						m_PacketCountFile;

	//��ɫɾ��ʱ��
	UInt32						m_RoleDeleteTime;

	//��ɫ�ָ�ʱ��
	UInt32						m_RoleRecoverTime;

	//��ɫɾ��/�ָ� ʱ������Ϣ
	std::string					m_RoleUpdateLimitInfo;
	
	//�ֻ��󶨵Ľ�ɫID
	ObjID_t						m_PhoneBindRoleId;

	//�ܷ������Ϸ
	bool						m_CanEnterGame;

	//��¼������Ϣ
	std::vector<LoginPushInfo>	m_LoginPushInfos;

	//ð�ն���Ϣ
	AdventureTeamInfo			m_AdventureTeamInfo;

	//����չ��ɫ��λ��������
	UInt32						m_UnlockeExtensibleRoleField;

	// �˺Żع�ʱ��
	UInt32						m_AccountReturnTime;

	// ��ɫ�ع�ʱ��
	UInt32						m_RoleReturnTime;

	// ��ɫ�ع�ȼ�
	UInt32						m_RoleReturnLevel;

	// �Ƿ��ٴλع�
	UInt8						m_AgainReturn;

};

/**
 *@brief ������
 */
typedef CLVisitor<GSPlayer> GSPlayerVisitor;

#endif
