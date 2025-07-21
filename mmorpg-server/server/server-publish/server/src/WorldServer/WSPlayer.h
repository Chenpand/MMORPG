#ifndef _WS_PLAYER_H_
#define _WS_PLAYER_H_

#include <CLSyncObject.h>
#include <CLObjectProperty.h>
#include <CLGameDefine.h>
#include <CLRequest.h>
#include <AvalonNetConnection.h>
#include <AvalonProtocol.h>
#include <AvalonSimpleTimer.h>
#include <CLVipDataEntry.h>
#include <CLMatchProtocol.h>
#include <CLSysNotify.h>
#include <CLChatProtocol.h>
#include <CLAuctionDefine.h>
#include <CLTeamDefine.h>
#include <CLDungeonDefine.h>
#include <kingnet_analyzer.h>
#include <CLSceneDefine.h>
#include <CLGuildDefine.h>
#include <CLActivityInfo.h>
#include <CLCheatDefine.h>

#include "MailBox.h"
#include "WSRelationMgr.h"
#include "OfflineEventMgr.h"
#include "NewAuctionMgr.h"
#include "AccountChargeRecord.h"
#include "AccountInfo.h"
#include "MallGiftPackMgr.h"
#include "CondActivateMgr.h"
#include "Room.h"
#include "WSMasterSystem.h"
#include "Monopoly.h"
#include "NewTitle.h"
#include "DailyTodo.h"

class WSScene;
class WSTeam;
class Tribe;

class ItemTransaction;

class Pasture;

class AdventurePassInfo;

typedef std::function<void(WSPlayer*)> PlayerOfflineLoadCallBack;

/**
 *@brief ���״̬
 */
enum PlayerStatus
{
	PS_INIT = 0,	    //��ʼ״̬
	PS_NORMAL = 1,	    //����״̬
    PS_LEAVEGAME = 2,   // �뿪�������ȴ�����״̬
	PS_FINAL = 3,	    //������״̬
};

//��ȡ�����������
enum LoadType
{
	LT_NONE = 0,		
	LT_MAKE_FRIEND = 1 << 0,		//ͬ����Ӻ���
	LT_MAKE_MASTER = 1 << 1,		//ͬ���ʦ
	LT_MAKE_DISCIPLE = 1 << 2,		//ͬ����ͽ
	LT_ADDTO_BLACKLIST = 1 << 3,    //������
	LT_MAKE_MASTERSECT_BROTHER = 1 << 4, //����ͬ��ʦ�ֹ�ϵ
	LT_MAKE_MASTERSECT_MASTER = 1 << 5,  //����ͬ��ʦ����ϵ���Ϻ�ˢ�Ѵ��ڹ�ϵ
	LT_MAKE_MASTERSECT_DISCIPLE = 1 << 6, //����ͬ��ͽ�ܹ�ϵ
	LT_REMOVE_MASTERSECT_DISCIPLE = 1 << 7, //�Ƴ�ͽ��
	LT_PLAYER_SHAREDATA_GETMASTERTASK = 1 << 8,  //�鿴���ʦ������
	LT_ONREMOVE_FRIEND = 1 << 9,	//���Է��Ƴ�����
	LT_AUCTION_QUERY_ITEM = 1 << 10, //�����в�ѯ����
	LT_2V2_SORTLIST_REWARD = 1 << 11, //2v2���а�ͷ�ν���
	LT_FRIEND_PRSENT = 1 << 12, //��������
};

#define PLAYERINFO(T) \
	"accid(" << T->GetAccId() << ") playerid(" << T->GetID() << ") occu(" << T->GetOccu() << ")"

// ��ɫ������Ϣ
struct RoleBaseInfo
{
	ObjID_t roleid;
	UInt16	level;
};

/**
 *@brief ���  Noted by aprilliu, �յ�scene server������WorldRegisterPlayerReq ���д����� ��ObjID ���ǽ�ɫid��һ��guid��
 */
class WSPlayer : public CLSyncObject
{
	//��������ʱʱ��
	const static UInt32 ITEM_TRANS_TIMEOUT = 5;
	//�뿪��Ϸ��ʱ
	const static UInt32	LEAVEGAME_TIMEOUT = 30;

public:
	WSPlayer();
	~WSPlayer();

public:
	/**
	*@brief ��ʼ��
	*@param id ��ҵ�id
	*@param name �����
	*@param sceneid ���ڳ���id
	*@param gateid ��¼����id
	*/
	bool Init(ObjID_t id, UInt32 accid, const std::string& name, UInt32 sceneid, UInt32 gateid);

	/**
	*@brief ��ȡ���ؽڵ�id
	*/
	UInt32 GetGateID() const;
	void SetGateID(UInt32 nodeId);

	/**
	*@brief �˺���Ϣ
	*/
	AccountInfo* GetAccountInfo() const;
	void SetAccountInfo(AccountInfo* info);

	/**
	 *@brief ���û�ȡ״̬
	 */
	void SetGameStatus(UInt8 status)
	{ 
		m_Status = status; 
		m_StatusTime = (UInt32)CURRENT_TIME.Sec();
	}
	UInt8 GetGameStatus() const{ return m_Status; }
	UInt32 GetStatusTime() const { return m_StatusTime; }

	/**
	 *@brief ���û�ȡ��id
	 */
	void SetZoneId(UInt16 zoneid){ m_ZoneId = zoneid; }
	UInt16 GetZoneId() const{ return m_ZoneId; }

	/**
	*@brief ���û�ȡ������id
	*/
	void SetBornZoneId(UInt32 zoneId) { m_BornZoneId = zoneId; }
	UInt32 GetBornZoneId() const { return m_BornZoneId; }

	/**
	 *@brief ���û�ȡԭ��id
	 */
	void SetSrcZoneId(UInt32 srcZoneId){ m_SrcZoneId = srcZoneId; }
	UInt32 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	 *@brief ���û�ȡ�ȼ�
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief ���û�ȡְҵ
	 */
	void SetOccu(UInt8	occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; }

	/**
	*@brief ���û�ȡ����
	*/
	void SetAwaken(UInt8 awaken) { m_Awaken = awaken; }
	UInt8 GetAwaken() const { return m_Awaken; }

	/**
	*@brief ������ˢ��ʱ��
	*/
	void SetAuctionRefreshTime(UInt32 time) { m_AuctionRefreshTime = time; }
	UInt32 GetAuctionRefreshTime();
	void RefreshAuctionRefreshTime();

	/**
	*@brief �����ж��⹺�����λ
	*/
	void SetAuctionAdditionBooth(UInt32 num) { m_AuctionAdditionBooth = num; }
	UInt32 GetAuctionAdditionBooth() const { return m_AuctionAdditionBooth; }

	/**
	 *@brief ���û�ȡ�Ա�
	 */
	void SetSex(UInt8 sex){ m_Sex = sex; }
	UInt8 GetSex() const{ return m_Sex; }

	/**
	*@brief ���û�ȡͷ���
	*/
	void SetHeadFrame(UInt32 headFrameId){ m_HeadFrame = headFrameId; }
	UInt32 GetHeadFrame() const{ return m_HeadFrame; }
	void OnHeadFrameChanged(UInt32 headFrameId);

	void SetSceneID(UInt32 id);
	UInt32 GetSceneID() const { return m_SceneId; }

	void SetSceneType(SceneType type) { m_SceneType = type; }
	SceneType GetSceneType() const { return m_SceneType; }

	bool IsInTown() const;

	/**
	 *@brief ���û�ȡvip�ȼ�
	 */
	void SetVipLvl(UInt8 vip);
	UInt8 GetVipLvl() const;

	/**
	*@brief ��ȡ����ʱ��
	*/
	UInt32 GetOnlineTime() const { return (UInt32)m_OnlineTime; }

	/**
	*@brief ���û�ȡvip����
	*/
	void SetVipExp(UInt32 exp);
	UInt32 GetVipExp() const;

	/**
	*@brief ���û�ȡ�ܳ�ֵ���
	*/
	void SetTotalChargeNum(UInt32 num);
	UInt32 GetTotalChargeNum() const;

	/**
	*@brief ���û�ȡ��ɫ�ܳ�ֵ���
	*/
	void SetTotalPlayerChargeNum(UInt32 num) { m_totalPlayerChargeNum = num; }
	UInt32 GetTotalPlayerChargeNum() const { return m_totalPlayerChargeNum; }

	/**
	*@brief ���û�ȡ�¿�����ʱ��
	*/
	void SetMonthCardExpireTime(UInt32 time);
	UInt32 GetMonthCardExpireTime() const;
	bool IsInMonthCard() const;

	/**
	*@brief ���û�ȡ��ƿ���ɫ
	*/
	void SetMoneyManageStatus(UInt8 status);
	UInt8 GetMoneyManageStatus();

	/**
	 *@brief ����qq��Ȩ��Ϣ
	 */
	void SetQQVipInfo(const QQVipInfo& info){ m_qqVipInfo = info; }
	const QQVipInfo& GetQQVipInfo() const{ return m_qqVipInfo; }
	UInt8 GetSuperBlue() const{ return m_qqVipInfo.IsSuperBlue() ? 1 : 0; }

	/**
	*@brief ���û�ȡս����
	*/
	void SetPower(UInt32 power) { m_Power = power; }
	UInt32 GetPower() const { return m_Power; }

	/**
	 *@brief ���û�ȡ�ㄻ
	 */
	void SetPoint(UInt32 point);
	UInt32 GetPoint() const;

	/**
	 *@brief ���û�ȡ���õㄻ
	 */
	void SetCreditPoint(UInt32 point);
	UInt32 GetCreditPoint() const;

	/**
	*@brief ���û�ȡ����
	*/
	void SetGuildID(ObjID_t id);
	ObjID_t GetGuildID() const { return m_GuildId; }

	void SetGuildLvl(UInt32 lvl) { m_GuildLvl = lvl; };
	UInt32 GetGuildLvl() const { return m_GuildLvl; }

	//��ȡȨ�޵�ֵ
	void GetVipValue(VipPrivilegeType type, UInt32& value);
	UInt32 GetVipValue(VipPrivilegeType type);

	//��ȡȨ�޵�ֵ
	void GetVipValue(VipPrivilegeType type, bool& value);

	/**
	 *@brief ���û�ȡ����ʱ��
	 */
	void SetCreateTime(UInt32 time){ m_CreateTime = time; }
	UInt32 GetCreateTime() const{ return m_CreateTime; }
	UInt32 GetCreateFromNowOnDay();

	/**
	 *@brief ���û�ȡ���ڳ���������
	 */
	bool SetSceneNode(UInt32 node);
	UInt32 GetSceneNode() const;

	//void IncExp(UInt32 exp); /******** �ýӿ�δ��ʹ�� ********/

	/**
	*@brief world�����Ӿ���
	*/
	void IncExpActive(const std::string& reason, UInt64 incExp);

	/**
	 *@brief ��ȡ�˺�id
	 */
	UInt32	GetAccId() const{ return m_AccId; }

	/**
	*@brief �˺�guid
	*/
	ObjID_t GetAccGuid() const { return m_AccGuid; }
	void SetAccGuid(ObjID_t id) { m_AccGuid = id; }

	/**
	 *@brief ����openid
	 */
	void SetOpenId(const std::string& openid){ m_OpenId = openid; }
	const std::string& GetOpenId() const { return m_OpenId; }

	/**
	 *@brief ���û�ȡ����
	 */
	void SetCity(const std::string& city){ m_City = city; }
	const std::string& GetCity() const{ return m_City; }

	/**
	 *@brief ���û�ȡip
	 */
	void SetIp(const std::string& ip);
	const std::string& GetIp() const{ return m_Ip; }
	UInt32 GetIpValue() const{ return m_IpValue; }

	/**
	 *@brief ���û�ȡƽ̨id
	 */
	void SetPf(const std::string& pf){ m_Pf = pf; }
	const std::string& GetPf() const{ return m_Pf; }

	/**
	*@brief ���û�ȡ�豸id
	*/
	void SetDeviceId(const std::string& did) { m_DeviceId = did; }
	const std::string& GetDeviceId() const { return m_DeviceId; }

	/**
	*@brief ���û�ȡ�豸�ͺ�
	*/
	void SetModel(const std::string& model) { m_Model = model; }
	const std::string& GetModel() const { return m_Model; }

	/**
	*@brief ���û�ȡ�豸�汾
	*/
	void SetDeviceVersion(const std::string& deviceVersion) { m_DeviceVersion = deviceVersion; }
	const std::string& GetDeviceVersion() const { return m_DeviceVersion; }

	/**
	*@brief ���û�ȡ��λ
	*/
	void SetSeasonLv(UInt32 seasonLv)	{ m_SeasonLv = seasonLv; }
	UInt32 GetSeasonLv()	{ return m_SeasonLv; }
	/**
	 *@brief ������
	 */
	void KickOff(UInt32 errorCode, const char* msg = NULL);

	/**
	 *@brief �뿪��Ϸ
	 */
	void LeaveGame();

	/**
	 *@brief ���û�ȡ����ʱ��
	 */
	void SetOfflineTime(UInt64 time){ m_OfflineTime = time; }
	UInt64 GetOfflineTime() const{ return m_OfflineTime; }
	UInt32 GetOfflineTimeSec() const { return (UInt32)(m_OfflineTime / 1000); }
	/**
	 *@brief ���û�ȡ��������ʱ��
	 */
	void SetLocalOfflineTime(UInt64 time){ m_LocalOfflineTime = time; }
	UInt64 GetLocalOfflineTime() const{ return m_LocalOfflineTime; }

	/**
	*@brief ���û�ȡ��ȡ�����ʼ�ʱ��
	*/
	void SetOfflineMailTime(UInt64 time);
	void SaveOfflineMailTime(UInt64 time);
	UInt64 GetOfflineMailTime() const { return m_OfflineMailTime; }

	/**
	 *@brief ���û�ȡ������Ϣ
	 */
	void SetOfflineScene(UInt32 sceneid){ m_OfflineScene = sceneid; }
	UInt32 GetOfflineScene() const{ return m_OfflineScene; }
	void SetOfflinePos(const CLPosition& pos){ m_OfflinePos = pos; }
	const CLPosition& GetOfflinePos() const{ return m_OfflinePos; }

	/**
	 *@brief �򳡾�ͬ������ϵ����
	 */
	void SyncRelationDataToScene();

	/**
	 *@brief ��ȡָ����������������
	 */
	static UInt32 GetServerPlayerNum(UInt32 nodeId);

	/**
	 *@brief ֪ͨ�����뿪
	 */
	void NotifyLeaveGame(UInt8 isOfflineTimeOut = 0);

	/**
	 *@brief �ж��ܷ��ٴ���ļ
	 */
	bool CheckCopySceneRecruit();

	/**
	 *@brief ����֧������
	 */
	void SendBillingResult(UInt32 billingId, UInt8 result);

	/**
	*@brief ���á���̳ǵ��߹�����Ϣ
	*/
	void SetMallItemBuyedNum(UInt32 uId, UInt16 uNum);
	UInt16 GetMallItemBuyedNum(UInt32 uId);

	/**
	*@brief ���á���ÿ����̳��������߹�����Ϣ
	*/
	void SetGSMallBuyInfo(UInt32 uInfo) { m_uGSMallBuyInfo = uInfo; }
	UInt32 GetGSMallBuyInfo() const { return m_uGSMallBuyInfo; }

	/**
	 *@brief �ӻ���
	 */
	void AddMoneyItem(const char* reason, UInt8 type, UInt32 num);
	
	/**
	*@brief �ӵ���
	*/
	void AddItem(const std::string& reason, const ItemRewardVec& itemVec, bool isNotify = true);

	/**
	*@brief ֪ͨ�ͻ��˼��˵���
	*/
	void NotifyClientAddItem(const std::string& reason, const ItemRewardVec& rewards, UInt8 notifyType = 1);

	/**
	*@brief ���
	*/
	PlayerAvatar GetAvatar() const { return m_Avatar; }
	void SetAvatar(const PlayerAvatar& avatar);

	/**
	*@brief ���ͷ��
	*/
	PlayerIcon GetIcon() const;

	/**
	*@brief oppo�û�����ȼ�
	*/
	void SetOppoAmberLevel(UInt8 level) { m_OppoAmberLevel = level; }
	UInt8 GetOppoAmberLevel() const { return m_OppoAmberLevel; }

	/**
	*@brief ��ȡ��ұ�ǩ��Ϣ
	*/
	void GetPlayerLabelInfo(PlayerLabelInfo& info) const;

	/*
	*@brief ֪ͨscene��ѯ�������쳣����
	*/
	void NotifySceneQueryWaitHandleAbnormalTrans();

public://�������
	/**
	 *@brief ��ȡ���������
	 */
	RequestMgr& GetRequestMgr(){ return m_Requests; }

	/**
	 *@brief ͬ������
	 */
	void SyncRequest(UInt8 type, WSPlayer* pRequester, const std::string& param = "");

public://�ͷ����
	/**
	 *@brief �����жϳͷ���־λ
	 */
	void SetPunishFlag(UInt32 flag);
	UInt32 GetPunishFlag() const{ return m_PunishFlag; }
	bool CheckPunishFlag(int type) const{ return (m_PunishFlag & (1 << type)) != 0; }

	/**
	 *@brief ���÷�ŵ���ʱ��
	 */
	void SetForbitLoginDueTime(UInt32 duetime){ m_ForbitLoginDueTime = duetime; }

	/**
	 *@brief ���û�ȡ���Ե���ʱ�䣨������
	 */
	void SetForbitTalkDueTime(UInt32 duetime){ m_ForbitTalkDueTime = duetime; }
	UInt32 GetForbitTalkDueTime() const{ return m_ForbitTalkDueTime; }

	void SetLeaveTimeOut(int leaveTimeout) { m_LeaveTimeout = leaveTimeout; }

public://�������
	/**
	 *@brief ׼����������
	 */
	bool BeginItemTransaction(ItemTransaction* transaction);

	/**
	 *@brief ��ȡ��ǰ�ĵ�������
	 */
	ItemTransaction* GetItemTransaction() const{ return m_pItemTrans; }

	/**
	 *@brief �����Ʋ�������
	 */
	void UnLockAsserts();

	/**
	 *@brief �Ƴ�ָ�����ߣ��ڵ���ʹ����������
	 */
	void RemoveItem(ObjID_t itemId);

	void OnCheckItemRet(UInt16 serial, UInt32 result, UInt32 counterTimes);


public://�������
	/**
	 *@brief ֪ͨ
	 */
	//by huchenhui
	//void SendNotify(UInt32 id, ...);

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

	void Notify(UInt16 type, UInt8 color, const char* word);

public://�ʼ����
	/**
	 *@brief ��ȡ����
	 */
	MailBox& GetMailBox(){ return m_MailBox; }

public://�����¼�
	/**
	 *@brief ��ȡ�����¼�������
	 */
	OfflineEventMgr& GetOfflineEventMgr(){ return m_OfflineEventMgr; }

	/**
	 *@brief ����һ�������¼�
	 */
	void PutOfflineEvent(ObjID_t playerId, OfflineEvent* offlineevent);
	bool CanPutOfflineEvent(UInt16 type, std::vector<OfflineEvent*>& events);

public://�̳���ʱ���
	MallGiftPackMgr& GetMallGiftPackMgr() { return m_MallGiftPackMgr; }

	/**
	*@brief ��ȡ��������������
	*/
	CondActivateMgr& GetCondActivateMgr() { return m_CondActivateMgr; }

public://�������

	/**
	*@brief �Ƿ񿪷���ӹ���
	*/
	bool IsOpenTeamFunction();

	/**
	*@brief �Ƿ����ܼ������ĳ���
	*/
	bool IsSceneCanJoinTeam();

	/**
	 *@brief ���û�ȡ����
	 */
	void SetTeam(WSTeam* team);
	WSTeam* GetTeam() const;
	UInt32 GetTeamID() const { return m_TeamId; }

	/**
	 *@brief ��������
	 */
	void CreateTeam(UInt32 target, bool cancelMatch = true);

	/**
	 *@brief �������
	 */
	UInt32 InviteTeam(ObjID_t target);
	UInt32 ReplyInviteTeam(UInt32 teamId, bool agree);

	/**
	 *@brief �������
	 */
	void RequestTeam(ObjID_t target, const std::string& passwd);
	void RequestJoinTeam(UInt32 teamId, const std::string& passwd);
	void ReplyRequestTeam(ObjID_t requester);

	/**
	*@brief ������Ҽ��빫��
	*/
	void InviteJoinGuild(ObjID_t targetId);

public://��ϵ���
	/**
	 *@brief ��ȡ��ϵ������
	 */
	WSRelationMgr& GetRelationMgr() { return m_RelationMgr; }

	/**
	 *@brief �������
	 */
	void InviteFriend(ObjID_t target, std::string tarName);
	void ReplyInviteFriend(ObjID_t requester);
	bool MakeFriend(WSPlayer* other);
	void RemoveFriend(ObjID_t target);
	bool AddToBlackList(WSPlayer* target);

	bool MakeFriendNew(WSPlayer* other);
	bool MakeFriendOfflineCb(WSPlayer* other);

	void OnRemovedFried(ObjID_t targetId);
	/**
	*@brief ʦͽ
	*/
	bool CanReqMaster(ObjID_t target, bool sendNotice);
	void ReqMaster(ObjID_t targetId, std::string tarName);
	void ReplyMaster(ObjID_t requesterId);

	bool CanReqDisciple(ObjID_t target, bool sendNotice);
	void ReqDisciple(ObjID_t targetId, std::string tarName);
	void ReplyDisciple(ObjID_t requesterId);

	bool MakeMasterDisciple(WSPlayer* master, WSPlayer* disciple, UInt8 type);
	void RemoveMaster(ObjID_t targetId);
	void RemoveDisciple(ObjID_t targetId);

	/**
	 *@brief ����,��ȡ�۳����ܶ�
	 */
	void AddIntimacy(ObjID_t friendId, UInt16 value);
	UInt16 GetIntimacy(ObjID_t friendId);
	UInt16 DecIntimacy(ObjID_t friendId, UInt16 value);

	/**
	*@biref �ͻ��˼������������� �ڴ���
	*/
	void SetOpenFlag(UInt8 type, UInt8 flag) { m_mapOpenFlags[type] = flag; };
	UInt8 GetOpenFlag(UInt8 type) const;

	/**
	*@brief ��ȡʦ��ϵͳ
	*/
	WSMasterSystem& GetMasterSystem() { return m_MasterSys; }
	//ʦ������������
	std::map<UInt64, UInt8> m_pubishTaskFlag;
	void SetPubishTasFlag(UInt64 discpleId);
	bool CheckPublishTaskFlag(UInt64 discpleId);
	void ResetPubishTasFlag(UInt64 discpleId);
public://�������
	/**
	 *@brief ���û�ȡ��ǰ��������
	 */
	void SetPasture(Pasture* pasture){ m_pPasture = pasture; }
	Pasture* GetPasture() const{ return m_pPasture; }

public:
	// �������

	/**
	*@brief ��ӹ��Ἴ��
	*/
	void AddGuildSkill(UInt16 id, UInt8 level);

	/**
	*@brief ��ȡ���Ἴ��
	*/
	SkillBase* GetGuildSkill(UInt16 id);

	/**
	*@brief ��ȡ���Ἴ�ܵȼ�
	*/
	UInt8 GetGuildSkillLevel(UInt16 id);

	/**
	*@brief ��ȡ���Ἴ�ܵȼ�
	*/
	void SetGuildSkillLevel(UInt16 id, UInt8 level, UInt32 skillType = 1);

public://�������
	/**
	 *@brief ��齨���������
	 */
	bool CheckCreateCopyInterval(){ return m_CreateCopyTimer.IsTimeout(CURRENT_TIME); }

public:	// ����������־���
	/**
	*@brief ����udplog
	*/
	//void SendUdpLog(const char *str1, const char *str2, const char *str3, const char *str4, const char *str5, const char *str6, const char *strType, Int32 nCount = 1);

	/**
	*@brief ����udplog
	*/
	template <typename... Args>
	void SendUdpLog(const char* typeInfo, const char* format, const Args & ... args)
	{
		if (typeInfo == NULL || format == NULL) return;
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_WORLD_UID);
		if (NULL == pLogger) return;

		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		UInt16 zoneId = (GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID);
		UInt32 monthCardExpireTime = m_MonthCardExpireTime;
		UInt32 bronZoneId = GetBornZoneId();
		UInt32 isReturn = IsVeteranReturn() ? 1 : 0;

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccId(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, GetTotalPlayerChargeNum(), GetGuildLvl());
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(), m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccId(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, GetTotalPlayerChargeNum(), GetGuildLvl());
#endif

		pLogger->SetUserIP(m_Ip.c_str());
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);

	}

	void SendLoginUdpLog();

	void SendLogoutUdpLog();

	/**
	*@brief �����������ϼܵ���־
	*/
	void SendAuctionUpLog(AuctionType type, UInt64 itemId, UInt32 itemDataId, UInt32 itemNum, MoneyType moneyType, UInt32 price, UInt32 deposit, UInt32 duetime);

	/**
	*@brief �����������¼ܵ���־
	*/
	void SendAuctionDownLog(zjy_auction::AuctionSendItemType type, AuctionType auctionType, UInt64 itemId, UInt32 itemDataId, UInt32 itemNum);

	/**
	*@brief ���������г��۵���־
	*/
	void SendAuctionBuyLog(UInt64 targetId, UInt32 ownerVipLev, AuctionType type, bool abnormalDeal, ObjID_t srcItemId, UInt64 itemId,
		zjy_auction::AuctionMainItemType mainType, UInt32 itemDataId, UInt32 itemNum, MoneyType moneyType, UInt32 price,
		UInt32 tax, UInt8 strengthen, UInt32 averPrice, UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp,
		std::string sellerDeviceId, UInt32 targetAccId, UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid,
		UInt8 isAttented, UInt32 itemAttentNum, UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
		UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice, UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice
		, UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax);

	/**
	*@brief ������Ҹ������־
	*/
	void SendReviveUdpLog(UInt32 dungeonId, UInt64 targetId, UInt32 areaId);

	/**
	*@brief ���Ϳ�ʼ���³ǵ���־
	*/
	void SendDungeonStartUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt8 isTeam, UInt8 isHaveFriend, UInt8 hellAutoClose);

	/**
	*@brief �����巿����־
	*/
	void SendDungeonClearAreaUdpLog(UInt32 dungeonId, DungeonType type, std::string dungeonName, DungeonHellMode hellmode, UInt32 areaId, UInt32 usedTime);

	/**
	*@brief ���͵��³ǳ�ҩЭ��
	*/
	void SendDungeonUsePotionUdpLog(UInt64 raceId, UInt32 dungeonId, UInt32 areaId, UInt32 itemTypeId, UInt8 isTeam);

	/**
	*@brief ���͵��³ǵ���
	*/
	void SendDungeonDropUdpLog(UInt32 dungeonId, UInt32 itemId, UInt32 color, UInt32 itemLevel);

	/**
	*@brief ���͹���ս�¼���־
	*/
	void SendGuildPvpUdpLog(UInt64 guildId, std::string guildName, UInt64 targetId, std::string targetName, PkType type, GuildBattleType battleType, UInt32 result, UInt32 oldGuildScore, UInt32 newGuildScore);

	/**
	*@brief ����ƥ��ɹ���־
	*/
	void SendMatchSuccessLog(UInt32 zoneId, ObjID_t opponentId, UInt8 pkType, UInt32 seasonLevel, UInt32 matchScore, UInt32 usedTime);

	/**
	*@brief ����ȡ��ƥ����־
	*/
	void SendMatchCancelLog(UInt8 pkType, UInt32 seasonLevel, UInt32 matchScore, UInt32 matchTime);

	/**
	*@brief �ϱ�������־�����ʹ�ø��ֹ���֮���ϱ�
	*/
	void SendFunctionUdpLog(FunctionLogType type, UInt32 p1, UInt32 p2);

	/**
	*@brief ��������
	*/
	void SendPvpEndUdpLog(UInt32 targetZoneId, UInt64 targetId, PkType type, PkRaceResult result, UInt32 oldMatchScore, UInt32 newMatchScore, UInt32 oldPkValue, UInt32 newPkValue, UInt32 oldSeasonLevel, UInt32 newSeasonLevel, UInt32 oldSeasonStar, UInt32 newSeasonStar, UInt32 oldSeasonExp, UInt32 newSeasonExp, UInt32 seasonId, UInt32 dungeonId, UInt32 usedTime);

	//void SendGuildStorageUdpLog(std::string reason, UInt64 itemId, UInt32 itemDataId, UInt32 num);

	/**
	*@brief ���������¼�
	*/
	void SendRoomCreateUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief ɾ�������¼�
	*/
	void SendRoomDeleteUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief ���뷿���¼�
	*/
	void SendRoomJoinUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief �˳������¼�
	*/
	void SendRoomQuitUdpLog(UInt32 roomId, UInt32 roomType, UInt32 createTime);

	/**
	*@brief ƥ�俪ʼ
	*/
	void SendRoomMatchStartUdpLog(UInt32 roomId, UInt32 roomType, UInt32 roomPlayerSize, UInt32 createTime);

	/**
	*@brief ƥ��ɹ�
	*/
	void SendRoomBattleStartUdpLog(UInt32 roomId, UInt32 roomType, UInt32 roomPlayerSize, UInt32 createTime);

	/**
	*@brief ����
	*/
	void SendItemUdpLog(const char* reason, UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, std::string itemName, UInt32 itemQuality, Int32 number, UInt32 oldNumber);

	/**
	*@brief ���Ͱ�ʦlog
	*/
	void SendMakeMasterDiscipleUdpLog(ObjID_t discipleId, ObjID_t masterId, UInt32 discipleLevel, UInt32 masterLevel, UInt32 discipleViplev, UInt32 masterViplev, UInt32 timestamp, UInt8 eventType, UInt8 masterOccu, UInt8 discipleOccu);

	/**
	*@brief ���ͳ�ʦlog
	*/
	void SendFinishSchoolUdpLog(UInt8 type, ObjID_t discipleId, ObjID_t masterId, UInt32 discipleLevel, UInt32 masterLevel, UInt32 discipleGrowth, UInt32 apprentTimeSec, UInt32 finishTimeSec);

	/**
	*@brief ����ʦ������ʦ������log
	*/
	void SendMasterPublishDailyTaskLog(ObjID_t discipleId, ObjID_t masterId, UInt32 timestamp);

	/**
	*@brief ������ȡʦ���ճ��������log
	*/
	void SendReceiveDailyTaskGiftLog(UInt32 type, ObjID_t uid, UInt32 timestamp);

	/**
	*@brief ����ʦͽ����log
	*/
	void SendMasterDiscipleInteractionLog(ObjID_t discipleId, ObjID_t masterId, UInt32 dungeonId, UInt32 timestamp);

	/**
	*@brief ��������ԭ��log
	*/
	void SendCheatLog(PlayerCheatReason reason, ObjID_t raceId, UInt32 dungeonId);

	/**
	*@brief �����ʲ����log
	*/
	void SendAssetUdpLog(AssetType type, const char* reason, UInt64 oldAsset, Int64 assetNum, Int64 nowAssetNum, UInt64 nowAsset);

	/**
	*@brief ����ð�ն����ָı�log
	*/
	void SendAdventureTeamNameChangeUdpLog(const std::string& oldName, const std::string& newName);

	/**
	*@brief ������ǲԶ��log
	*/
	void SendDispatchExpeditionTeamUdpLog(UInt8 mapId, UInt32 duration, UInt32 memberNum);

	/**
	*@brief ����ȡ��Զ��log
	*/
	void SendCancelExpeditionTeamUdpLog(UInt8 mapId);

	/**
	*@brief ������ȡԶ������log
	*/
	void SendGetExpeditionTeamRewardsUdpLog(UInt8 mapId, UInt32 duration, const std::vector<UInt32>& condIds);
	
	/**
	**@brief �����������˽���log
	*/
	void SendBlackMarketTransUdpLog(UInt8 opType, UInt64 transid, UInt64 auctId, UInt8 auctType, UInt32 auctItemDataId, UInt64 auctItemId, UInt32 auctPrice);

	/**
	**@brief ����ð�ն���Ϣlog
	*/
	void SendAdventureTeamInfoUdpLog();

	/**
	**@brief ������ֵ����log
	*/
	void SendRechargePushUdpLog(UInt32 pushId, UInt32 price);

	/**
	**@brief �������������˳�log
	*/
	void SendGuildJoinExitUdpLog(UInt32 opType);

	/**
	*@brief ���͵��³�roll��־
	*/
	void SendGungeonRollUdpLog(UInt32 dungeonId, UInt32 rollIndex, UInt32 rollItem, UInt32 rollItenNum, UInt32 rollType, UInt32 point);


	/**
	*@brief ����ð��ͨ��֤��־
	*/
	void SendAdventurePassUdpLog(UInt32 level, UInt32 passType);

	/**
	*@brief������Ӻ���������־
	*/
	void SendAddFriendUdpLog(UInt32 guildId, UInt32 tLevel, UInt32 tVipLev, UInt32 tOccu, UInt32 tGuildId);

	/**
	*@brief���ͻ��Ҵ�����־
	*/
	void SendMoneyStockUdpLog();

public://���ݿ����
	/**
	 *@brief ���Ӽ�������������ر��μ������
	 */
	UInt8 IncLoadDataCount();

	/**
	 *@brief ��������
	 */
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);

public://�¼�
	/**
	 *@brief ����
	 */
	void OnOnline(bool isReconnect = false);

	/**
	 *@brief ����
	 */
	void OnOffline(bool serverQuit = false);

	/**
	*@brief ��Gate�Ͽ����ӣ����ϣ�
	*/
	void OnDisconnect();

	/**
	*@brief �����ɹ�
	*/
	void OnReconnectSuccess();

	/**
	 *@brief �����������л�
	 */
	void OnServerChanged(UInt32 oldNodeId);

	/**
	 *@brief �ȼ��ı�
	 */
	void OnLevelChanged(UInt16 oldLv);

	/**
	*@brief ְҵ�ı�
	*/
	void OnOccuChanged();

	/**
	 *@brief vip�ȼ��ñ�
	 */
	void OnVipLvlChanged();

	/**
	*@brief �����ȼ��ı�
	*/
	void OnSeasonLvChanged();

	/**
	*@brief ��������
	*/
	void OnExpIncrease(UInt64 addExp, const std::string& reason);

	/**
	 *@brief ��ɱ��
	 */
	void OnKilled(WSPlayer* attacker);

	/**
	 *@brief ����
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	 *@brief �����������
	 */
	void OnLoadDataFinished(bool isReconnect = false);

	/**
	*@brief �������ȫ����Ϣ�ص�
	*/
	void OnLoadGlobalInfo(CLRecordCallback *pCallback);

	/**
	*@brief �����һ�ε�¼
	*/
	void OnFirstLogin();

	/**
	 *@brief ս���ı�
	 */
	void OnPowerChanged();

	/**
	*@brief ��������
	*/
	void HandlerFriendGive(WSPlayer* target, WSRelation* relation);
	void OnFriendGive(WSPlayer* target);

	/**
	*@brief ������ �ϼ�
	*/
	void OnAddAuction(zjy_auction::AuctionInfo* auctionInfo);

	/**
	*@brief ������ �¼�
	*/
	void OnDownAuction(zjy_auction::AuctionSendItemType downType, AuctionType type, UInt64 itemId, UInt32 itemDataId, UInt32 num);

	/**
	*@brief ������ ����
	*/
	void OnSellAuction(UInt32 money);

	/**
	*@brief ������ ����
	*/
	void OnBuyAuction(AuctionType type, bool abnormalDeal, ObjID_t owner, UInt32 ownerVipLev, ObjID_t srcItemId,
		ObjID_t itemId, zjy_auction::AuctionMainItemType mainType, UInt32 itemDataId, UInt32 num,
		MoneyType priceType, UInt32 price, UInt32 counterFee, UInt8 strengthen, UInt32 averPrice,
		UInt32 onSealTime, ItemQuality itemQuality, std::string sellerIp, std::string sellerDeviceId, UInt32 ownerAccount,
		UInt8 isTreas, UInt32 sysRecomPrice, UInt32 minPriceOnSale, UInt64 transGuid, UInt8 isAttented, UInt32 itemAttentNum,
		UInt32 mountMagicCardId, UInt32 mountMagicCardAverPrice, UInt32 mountBeadId, UInt32 mountBeadAverPrice, UInt32 eqQualityLv, UInt32 remainSealCount,
		UInt64 minPriceGuid, UInt32 minPriceOnsalePrice, UInt32 minPriceQualityLv, UInt32 minPriceMountBeadId, UInt32 minPriceMountBeadAverPrice,
		UInt32 minPriceMountMagicId, UInt32 minPriceMountMagicAverPrice, UInt32 auctionPrice, UInt32 auctionTransNum, UInt32 auctionTransMax);

	/**
	*@brief �����¼�
	*/
	void OnSyncSceneEvent(SyncEventType type, ObjID_t param1, ObjID_t param2, ObjID_t param3);

	/**
	*@brief �ű��¼�
	*/
	void OnTeamCopyEvent(ObjID_t param1, ObjID_t param3);

	/**
	*@brief ����ı�
	*/
	void OnGuildChanged();

	/**
	*@brief ���빤��
	*/
	void OnJoinGuild();

	/**
	*@brief ����GlobalServer��Ϣ
	*/
	void RequestGlobalServerInfo();

	/**
	*@brief ���ӹ���ս����
	*/
	void OnIncGuildBattleScore(UInt32 score);

	/**
	*@brief ռ�����
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief �̳ǹ���
	*/
	void OnMallItemBuy(UInt32 mallItemId, UInt32 buyNum);

	/**
	*@brief ɱ��
	*/
	void OnOwnedNpcDied(UInt32 npcid, UInt8 type, UInt32 dungeon);

	/**
	*@brief ���ͨ��
	*/
	void OnPlayerClearDungeon(const DungeonDataEntry* dungeonData, LevelScore score, UInt32 usedTime, UInt8 isAsist);

	/**
	*@brief ƣ������
	*/
	void OnRemoveFatigue(UInt32 removeFatigue);

public://ƥ�����

	/**
	*@brief ����ƥ����
	*/
	void SetMatching(bool matching) { m_IsMatching = matching; }
	bool IsMatching() const { return m_IsMatching; }

	/**
	*@brief �ܷ�ʼƥ��
	*/
	bool CanStartMatch();

	/**
	*@brief ��ʼƥ��
	*/
	void OnStartMatch();

	/**
	*@brief ȡ��ƥ��
	*/
	void OnCancleMatch();

	/**
	*@brief ��ʼpkս��
	*/
	void OnStartPvPRace();

	/**
	*@brief ��ȡս����Ҫ�������Ϣ
	*/
	RacePlayerInfo GetRacePlayerInfo();

	/**
	*@brief ������ս���
	*/
	void RequestPkChallenge(ObjID_t otherId, RequestType type = REQUEST_CHALLENGE_PK);

	/**
	*@brief ��ս������󷵻�
	*/
	void ReplyPkChallenge(ObjID_t requesterId, bool agree, RequestType type = REQUEST_CHALLENGE_PK);

	/**
	*@brief �ܷ�ʼ��ս�� type������������pk����͹�ƽ������pk����
	*/
	UInt32 CanStartPkChallenge(bool isChallenger, RequestType type = REQUEST_CHALLENGE_PK);

	/**
	*@brief ��ȡս��״̬�����С�æµ��
	*/
	FriendMatchStatus GetMatchStatus();
	void SetMatchStatus(FriendMatchStatus status, bool forceSync = false);

	/**
	*@brief ����ս��״̬�����С�æµ��
	*/
	FriendMatchStatus CountMatchStatus();

	/**
	*@brief ����ս��״̬
	*/
	void UpdateMatchStatus();

public:// �������
	/**
	*@brief ��ȡ������������Ʒ����
	*/
	UInt8 GetAuctionNum(AuctionType type);

	/**
	*@brief	���������Ʒ
	*/
	void ClearAuction();

	/**
	*@brief ���������Ʒ
	*/
	void AddAuction(AuctionType type, ObjID_t id);

	/**
	*@brief ɾ��������Ʒ
	*/
	void DelAuction(AuctionType type, ObjID_t id);

	/**
	*@brief ��ȡ�����б�
	*/
	std::vector<ObjID_t> GetAuctionList(AuctionType type);

public: //�������
	//��÷���ID
	UInt32 GetRoomId() { return m_RoomId; }
	//���÷���ID
	void SetRoomId(UInt32 roomId) { m_RoomId = roomId; }

	//��ȡ�˳�����id
	UInt32 GetQuitRoomId() { return m_QuitRoomId; }
	//�����˳�����id
	void SetQuitRoomId(UInt32 roomId) { m_QuitRoomId = roomId; }

	//��ȡ�˳������ʱ��
	UInt32 GetQuitRoomTime() { return m_QuitRoomTime; }
	//�����˳������ʱ��
	void SetQuitRoomTime(UInt32 quitRoomTime) { m_QuitRoomTime = quitRoomTime; }

	UInt16 GetRoomLimitLevel();

	UInt32 GetRoomLimitSeasonLevel();

	void OnStartRoomRace();

	void OnCancleRoomMatch();

	bool IsOpenRoomFunction();

public: //33�
	UInt32 GetScoreWarLoseStreak() { return m_ScoreWarLoseStreak; }
	void SetScoreWarLoseStreak(UInt32 loseStreak) { m_ScoreWarLoseStreak = loseStreak; }

	UInt32 GetScoreWarWinStreak() { return m_ScoreWarWinStreak; }
	void SetScoreWarWinStreak(UInt32 winStreak){ m_ScoreWarWinStreak = winStreak; }

	UInt32 GetMatchScore() { return m_MatchScore; }
	void SetMatchScore(UInt32 matchScore) { m_MatchScore = matchScore; }

	UInt32 GetScoreWarBattleCount() { return m_ScoreWarBattleCount; }
	void SetScoreWarBattleCount(UInt32 battleCount) { m_ScoreWarBattleCount = battleCount; }

public: //�ڱ����
	//��ȡ�ڱ���ͼID
	UInt32 GetDigMapId() { return m_DigMapId; }
	//�����ڱ���ͼID
	void SetDigMapId(UInt32 mapId) { m_DigMapId = mapId; }

public://��ֵ���
	/**
	*@brief ���ĳ����Ʒ��ֵ����
	*/
	UInt32 GetChargeGoodsTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief �����Ʒ����Ĺ������
	*/
	UInt32 GetTodayChargeTimes(ChargeMallType mallType, UInt32 goodsId, ObjID_t roleId);

	/**
	*@brief ��ֵ�ɹ��ص�
	*/
	void AddChargeGoodsTimes(UInt32 accid, ObjID_t roleId, ChargeMallType mallType, UInt32 goodsId, UInt32 times);

	/**
	*@brief ���ó�ֵ��¼
	*/
	void ResetChargeGoodsTimes();

	/**
	*@brief �ܷ����¿�
	*/
	bool CanBuyMonthCard();

	/**
	*@brief �ܷ�����ƿ�
	*/
	bool CanBuyMoneyManageCard();

public:// ս�����
	/**
	*@brief ��ǰ���еı���ID
	*/
	ObjID_t	GetRaceID() const { return m_RaceID; }
	void SetRaceID(ObjID_t id) { m_RaceID = id; }

	/**
	*@brief ��ȡrelay��ַ
	*/
	const Avalon::SockAddr& GetRelayAddr() const { return m_RelayAddr; }
	void SetRelayAddr(const Avalon::SockAddr& addr) { m_RelayAddr = addr; }

public:// ���
	/**
	*@brief ���ͺ���ɹ�
	*/
	void OnSendRedPacketSuccess(ObjID_t redPacketId, UInt16 reason, UInt8 num, const std::string& name);

	/**
	*@brief ���ͺ��ʧ��
	*/
	void OnSendRedPacketFailure(UInt32 errorCode);

public: //��ħֵ
	/**
	*@brief ��ħֵ
	*/
	void SetResistMagic(UInt32 value) { m_ResistMagic = value; }
	UInt32 GetResistMagic() { return m_ResistMagic; }

	void HandleSceneSyncResistMagic(UInt32 value);

public://���˻
	PlayerActivityInfo<WSPlayer>& GetPlayerActivityInfo() { return m_PlayerActivityInfo; }

public:// ð�ն�
	/**
	*@brief ð�ն�
	*/
	void TryCreateAdventureTeam();
	bool HasAdventureTeam();

	/**
	*@brief ð�ն�����
	*/
	const std::string GetAdventureTeamName() const;
	bool RenameAdventureTeam(const std::string& newName);

	/**
	*@brief ð�նӵȼ�
	*/
	UInt16 GetAdventureTeamLevel() const;

	/**
	*@brief ð�նӾ���
	*/
	UInt64 GetAdventureTeamExp() const;

	/**
	*@brief ð�ն�����
	*/
	const std::string& GetAdventureTeamGrade() const;

	UInt8 GetAdventureTeamGradeNum() const;

	/**
	*@brief ð�նӽ�ɫ������
	*/
	UInt32 GetAdventureRoleScore() const;

	/**
	*@brief ͬ��ð�ն���Ϣ
	*/
	void SyncAdventureTeamInfo(bool create = false);

	/**
	*@brief ��ӽ�������ְҵ
	*/
	void TryAddUnlockedNewOccu();

	/**
	*@brief ͬ����������ְҵ
	*/
	void SyncUnlockedNewOccus(bool isOnOnline = false);

	/**
	*@brief ��ȡð�նӵ��³Ǽӳ�
	*/
	void GetAdventureTeamDungeonAddition(DungeonAddition& addition, const DungeonDataEntry* dungeonData);

	/**
	*@brief �͸�ˮ������
	*/
	UInt64 GetBlessCrystalExp() const;

	/**
	*@brief �͸�ˮ������
	*/
	UInt32 GetBlessCrystalNum() const;

	/**
	*@brief ����ף������
	*/
	UInt64 GetInheritBlessExp() const;

	/**
	*@brief ����ף������
	*/
	UInt32 GetInheritBlessNum() const;

	/**
	*@brief ʹ�ô���ף��
	*/
	UInt32 UseInheritBless(UInt32 num = 1);

	/**
	*@brief �ͽ�
	*/
	UInt32 GetBounties() const;

	/**
	*@brief ͬ��ð�ն���ػ���
	*/
	void SyncAdventureTeamMoney();

public://��ɫ����
	/**
	*@brief ��ɫ����ͬ��
	*/
	void OnRoleValueScoreSync(UInt32 roleValueScore);

	/**
	*@brief ���н�ɫ������
	*/
	UInt32 GetTotalScoreOfAllRoleValue() const;

public://��ɫ��λ
	/**
	*@brief �����Ŀ���չ��ɫ��λ
	*/
	void SetUnlockedExtensibleRoleField(UInt32 num);
	UInt32 GetUnlockedExtensibleRoleField();

	/**
	*@brief ���ӽ����Ŀ���չ��ɫ��λ
	*/
	bool IncUnlockedExtensibleRoleField(UInt32 incNum = 1);

	/**
	*@brief ��������չ��ɫ��λ
	*/
	void UnlockExtensibleRoleField(ObjID_t costItemUId, UInt32 costItemDataId);

	/**
	*@brief ��������չ��ɫ��λ����
	*/
	void SendUnlockExtensibleRoleFieldRespose(UInt32 errorCode);

	/**
	*@brief ��ȡӵ�еĽ�ɫ��λ��
	*/
	UInt32 GetRoleFileds();

public:// �ؾ������
	/**
	*@brief ���û�ȡ�ؾ����������
	*/
	void SetGnomeCoinNum(UInt32 num);
	UInt32 GetGnomeCoinNum() const;

	/**
	*@brief ���ӵؾ����������
	*/
	void IncGnomeCoinNum(const std::string& reason, UInt32 incNum, bool checkCond = true);

public:// �˺���ؽ�Ǯ

	/*
	*@brief �ж��ܷ��Ǯ
	*/
	bool CheckRmAccountMoney(ItemSubType moneyType, UInt32 cost);
	bool RemoveAccountMoney(const std::string& reason, ItemSubType moneyType, UInt32 num);

	/*
	*@brief �����˺Ž�Ǯ
	*/
	bool IncAccountMoney(const std::string& reason, ItemSubType moneyType, UInt32 incNum);

	/*
	*@brief �ж��ܷ���˺ż���
	*/
	bool CheckRmAccountCounterNum(AccountCounterType type, UInt64 cost);
	bool RemoveAccountCounterNum(const std::string& reason, AccountCounterType type, UInt64 num);

public:// �̳�
	/**
	*@brief �̳ǹ���������Ϣͬ��
	*/
	void MallBuyGotInfoSync(MallBuyGotType type);
	void MallBuyGotInfoSync();

public:// ��Ӫ�
	/**
	*@brief ��Ӫ��ر�
	*/
	void OnOpActivityClose(const OpActivityRegInfo& opAct);

public://����
	/**
	 *@brief ���͵�����
	 */
	void SendToGate(Avalon::Protocol& protocol);

	/**
	 *@brief ���͵��ͻ���
	 */
	void SendProtocol(Avalon::Protocol& protocol);
	void SendPacket(Avalon::Packet* packet);

	/**
	 *@brief ���͵����ڳ���
	 */
	void SendToScene(Avalon::Protocol& protocol);
	void SendToScene(Avalon::Packet* packet);

	/**
	*@brief ͬ���¼���Scene
	*/
	void SyncEventToScene(SyncEventType type, ObjID_t param1 = 0, ObjID_t param2 = 0);

public://���ݿ����
	void LoadData();
	void Saveback();

	void LoadGlobalInfo();
	void SaveTribeSkill();
	void SaveMallBuyInfo();
	void LoadAccountInfo();
	void LoadMallGiftPack();
	void LoadAccountActivityTask();
	void LoadMallItem();
	void LoadAccountShopItemRoleBuyRecord();

public:
	inline void SetRefTime(UInt64 time)	{ m_RefTime = time; }
	inline UInt64 GetRefTime()	{ return m_RefTime; }

public: //���߼���������
	void SetOfflineReplyCtx(WSPlayer* player, LoadType lt, PlayerOfflineLoadCallBack cb = nullptr);
	void ClearOfflineReplyCtx();
	void OfflineLoadCb();
	bool IsOfflineLoad();

public:
	// ��ȫ��
	void HandlerSecurityLockDataReq(const std::string& deviceID);
	void HandlerSecurityLockOpReq(UInt32 _type, const std::string& _passwd);
	void ChangeSecurityPasswdReq(const std::string& _oldPasswd, const std::string& _newPasswd);
	void HandlerCommonDeviceReq(UInt32 type, const std::string& deviceID);

	void SyncSecurityStatusToScene();
	void SyncSecurityStatusToScene(UInt32 _state);

	void SecurityLockForceCheck();
	void ClearSecurityLockNotifyClient();
	void NotifySecurityPasswdErrorNum();

	// �����ȫ����ס������false����֪ͨ�ͻ���
	bool SecurityLockCheck();

public:
	// ���õ�ǰ�ɹ���������Ʒid
	void SetCanBuyAuctionGuids(const std::vector<ObjID_t>& guids);
	bool CheckCanBuyAuction(const ObjID_t& guid);
	void SetAuctionProhibitTimeStamp(UInt64 timeStamp) { m_AuctionProhibitTimeStamp = timeStamp; }
	UInt64 GetAuctionProhibitTimeStamp() { return m_AuctionProhibitTimeStamp; }

	void SetAuctionProhibitQueryPriceTimeStamp(UInt64 timeStamp) { m_AuctionProhibitQueryPriceListTimeStamp = timeStamp; }
	UInt64 GetAuctionProhibitProhibitTimeStamp() { return m_AuctionProhibitQueryPriceListTimeStamp; }
public:
	void SetGuildDungeonCD() { m_GuildDungeonCD = CURRENT_TIME.Sec(); }
	UInt64 GetGuildDungeonCD() { return m_GuildDungeonCD; }

	void SetGuildDungeonDebugDamage(UInt64 val) { m_DebugGuildDungeonMaxDamage = val; }
	UInt64 GetGuildDungeonDebugDamage() { return m_DebugGuildDungeonMaxDamage; }

public: // ��Ϸ����
	
	// ��ȡ��Ϸ���õ�����
	template<typename T>
	T GetGameSetValue(UInt32 type, UInt32 subType = 0);
	// ������Ϸ��������
	void ParseGameSet(const std::string& gameSet);

public:	// �ع�

	// �ع���������
	void ReturnRset();

	// �Ƿ��ϱ��ع�
	bool IsVeteranReturn() const;
	// �ϱ��ع���
	void OnCheckVeteranReturn();

	// ��ɫ�ع�ʱ��
	void SetRoleReturnTime(UInt32 time) { m_RoleReturnTime = time; }
	UInt32 GetRoleReturnTime() const { return m_RoleReturnTime; }

	// �ع��״ε�½
	void SetReturnFirstLogin(UInt32 firstLogin) { m_ReturnFirstLogin = firstLogin; }
	bool IsReturnFirstLogin() { return m_ReturnFirstLogin != 0; }
	void UpdateReturnFirstLogin();

	// �ع�����ƺ�
	void SetReturnYearTitle(UInt32 title) { m_ReturnYearTitle = title; }
	UInt32 GetReturnYearTitle() const { return m_ReturnYearTitle; }

	// ��װ�������
	void SetSuit(bool bo) { m_HaveSuit = bo; }
	bool GetSuit() const { return m_HaveSuit; }

	void SendAdventurePassStatus();

public: // �˺ż���

	// ͬ���˺ż�����
	void SyncAccountCounterToClient(UInt32 type);
	void SyncAccountCounterToScene(UInt32 type);
	void SyncAllAccountCounterToClient();
	void SyncCustomerAccountCounterToClient(const std::vector<UInt32>& types);

	// ͬ���˺����ݵ�scene
	void SyncAccountDataToScene(UInt32 type);


public: //ͷ��
	PlayerTitleRoleMgr& GetTitleMgr() { return m_TitleMgr; }
	// ���õ�ǰ������ͷ��id
	void SetCurNewTitleGuid(UInt64 titleGuid) { m_curWearTitleGuid = titleGuid; }
	// ��ȡ��ǰ������ͷ��id
	UInt64 GetCurNewTitleGuid() { return m_curWearTitleGuid; }

public: //����
	MonopolyInfo& GetMonopolyInfo();

public: //ÿ�ձ���
	
	DailyTodoMgr& GetDailyTodoMgr() { return m_DailyTodoMgr; }

	DailyTodoMgr	m_DailyTodoMgr;

	/**
	*@brief ������Ȩ
	*/
	bool HaveGiftRightCard() const { return m_HaveGiftRight; }
	void SetHaveGiftRightCard(bool bo) { m_HaveGiftRight = bo; }

	/**
	*@brief �ܷ���������Ȩ��
	*/
	bool CanBuyGiftRightCard() const { return m_CanBuyGiftRight; }
	void SetCanBuyGiftRightCard(bool bo) { m_CanBuyGiftRight = bo; }

	/**
	*@brief �ܷ�����Ԩ��Ȩ��
	*/
	bool CanBuyDungeonRightCard() const { return m_CanBuyDungeonRight; }
	void SetCanBuyDungeonRightCard(bool bo) { m_CanBuyDungeonRight = bo; }

public: // ������ɫ������Ϣ
	 /**
	 *@brief ����������ɫ������Ϣ
	 */
	void SetOtherRoleBaseInfo(const std::vector<RoleBaseInfo>& info) { m_OtherRoleInfo = info; }
	const std::vector<RoleBaseInfo>& GetOtherRoleBaseInfo() const { return m_OtherRoleInfo; }

	/**
	 *@brief �Ƿ��н�ɫ����ȼ�Ҫ��(�����Լ�)
	 */
	bool HasRoleLevelLimit(UInt16 level) const;

	/**
	 *@brief �˺��½�ɫ��ߵȼ�(�����Լ�)
	 */
	UInt32 MaxRoleLevel() const;

private:
	//���ڳ�������
	Avalon::NetConnectionPtr	m_SceneConn;
	//��������id
	Avalon::NetConnectionPtr	m_Conn;

	//����������������
	static std::map<UInt32, UInt32>	m_ScenePlayerNum;

	//�˺�id
	UInt32	m_AccId;
	//�˺ű�guid
	ObjID_t	m_AccGuid;
	// �˺���Ϣ
	AccountInfo*	m_AccountInfo;

	//���״̬
	UInt8	m_Status;
	//�޸�״̬��ʱ��
	UInt32	m_StatusTime;

	//�Ƿ�֪ͨ�����뿪��Ϸ
	UInt16	m_LeaveTimeout;
	//����ʱ��(s)
	UInt64	m_OnlineTime;
	//����ʱ��
	UInt64	m_OfflineTime;
	//��������ʱ��
	UInt64	m_LocalOfflineTime;
	//���߳���
	UInt32	m_OfflineScene;
	//����λ��
	CLPosition	m_OfflinePos;
	
	//��ȡ�����ʼ�ʱ��
	UInt64	m_OfflineMailTime;

	//��id
	UInt16	m_ZoneId;
	//ԭ��id
	UInt32	m_SrcZoneId;
	//������
	UInt32	m_BornZoneId;

	//�ȼ�
	UInt16	m_Level;
	//ְҵ
	UInt8	m_Occu;
	//����
	UInt8	m_Awaken;
	//�Ա�
	UInt8	m_Sex;
	// ͷ���
	UInt32	m_HeadFrame;
	//qq��Ȩ��Ϣ;
	QQVipInfo	m_qqVipInfo;
	//ս����
	UInt32	m_Power;

	//������ˢ��ʱ��
	UInt32		m_AuctionRefreshTime;

	//�����ж��⹺�����λ
	UInt32		m_AuctionAdditionBooth;

	//openid
	std::string	m_OpenId;
	//����
	std::string	m_City;
	//ip
	std::string	m_Ip;
	//��ֵip
	UInt32		m_IpValue;
	//ƽ̨
	std::string	m_Pf;
	//�豸id
	std::string m_DeviceId;
	//�豸�ͺ�
	std::string m_Model;
	//�豸�汾
	std::string m_DeviceVersion;

	//�ͷ���־λ
	UInt32	m_PunishFlag;
	//��ŵ���ʱ��
	UInt32	m_ForbitLoginDueTime;
	//���Ե���ʱ��
	UInt32	m_ForbitTalkDueTime;

	//��ɫ����ʱ��
	UInt32	m_CreateTime;

	//��������
	UInt8	m_LoadDataCount;
	//�������
	UInt32	m_LoadDataSerial;

	//��������
	ItemTransaction*	m_pItemTrans;
	//��������ʱ��ʱ
	UInt8			m_ItemTransTimeout;

	//����
	MailBox			m_MailBox;
	//����ID
	UInt32			m_TeamId;

	//����������ʱ�������쳣��
	Avalon::SimpleTimer m_CreateCopyTimer;
	//��ļ���ʱ��
	Avalon::SimpleTimer	m_RecruitTimer;

	//���������
	RequestMgr		m_Requests;

	//��ϵ������
	WSRelationMgr	m_RelationMgr;

	//ͬ�Ź�ϵ������
	WSMasterSystem	m_MasterSys;

	//�����¼�������
	OfflineEventMgr	m_OfflineEventMgr;

	//�̳���ʱ���������
	MallGiftPackMgr	m_MallGiftPackMgr;

	// ��������������
	CondActivateMgr m_CondActivateMgr;

	// ���似��
	std::map<UInt16, UInt8>	m_mapTribeSkills;

	// �̳ǵ��߹�����Ϣ
	std::map<UInt32, UInt16>	m_mapMallBuyInfo;
	UInt32						m_uGSMallBuyInfo;

	//ȫ��counter�����Ƿ��
	std::map<UInt8, UInt8> m_mapOpenFlags;

	//�߼���ʱ�¼�
	UInt64		m_TickTime[LOGICTICK_MAX];

	//��ǰ��������
	Pasture*	m_pPasture;

    // ������Ʒ�б�
    std::vector<ObjID_t>    m_auctoinList[AT_NUM];

	// ������ڳ���
	SceneType	m_SceneType;
	UInt32		m_SceneId;

	UInt8		m_bDayChanged;
	UInt8		m_bLogicDayChanged;

	ObjID_t		m_GuildId;
	UInt32		m_GuildLvl;

	// ���Ἴ��
	std::vector<SkillBase>	m_GuildSkills;

	// ���
	PlayerAvatar			m_Avatar;

	// ��ȡ��������������� LoadType
	UInt32					m_LoadPlayerType;

	// ����������ҵ���
	ObjID_t					m_RequestTarget;

	// ���߼�����ɻص�����
	PlayerOfflineLoadCallBack	m_playerOfflineLoadCb;

	//��λ
	UInt32					m_SeasonLv;

	//�Ƿ�ƥ����
	bool					m_IsMatching;

	//ƥ��״̬
	FriendMatchStatus		m_MatchStatus;

	//�¿�����ʱ��
	UInt32					m_MonthCardExpireTime;

	// ���ݱ����õ�ʱ��
	UInt64					m_RefTime;

	// ʦ������
	std::string				m_MasterNote;

	// ʦ���Ƿ���ͽ
	bool					m_IsRecvDisciple;

	//���ڷ���ID
	UInt32					m_RoomId;
	//��һ���˳�����ID
	UInt32					m_QuitRoomId;
	//��һ���˳�����ʱ��
	UInt32					m_QuitRoomTime;

	//��ҹ�ע���ڱ���ͼ
	UInt32					m_DigMapId;

	//oppo�û�����ȼ�
	UInt8					m_OppoAmberLevel;

	//��ɫ��ֵ�ܽ��
	UInt32					m_totalPlayerChargeNum;

	//���߼����Ƿ�ʧ��
	bool					m_loadOfflineDataFail;

	//�Ƿ��ڿ������
	bool					m_inCrossScene;

	//��ħֵ
	UInt32					m_ResistMagic;

	// ս�����
	ObjID_t					m_RaceID;
	Avalon::SockAddr		m_RelayAddr;

	//���˻
	PlayerActivityInfo<WSPlayer> m_PlayerActivityInfo;

	//33����
	UInt32 m_ScoreWarLoseStreak;
	UInt32 m_ScoreWarWinStreak;
	UInt32 m_MatchScore;
	UInt32 m_ScoreWarBattleCount;

	// ��ǰ���ߵ��豸
	std::string m_curOnlineDev;

	// ��ǰ�ɼ������������Ʒid
	std::vector<ObjID_t> m_AuctionIds;

	//�����в�ѯ��ֹʱ���
	UInt64				 m_AuctionProhibitTimeStamp;

	// ������³���սCD
	UInt64				 m_GuildDungeonCD;

	// ���õĹ�����³�����˺�ֵ
	UInt64				 m_DebugGuildDungeonMaxDamage;

	// ��ɫ�ع�ʱ��
	UInt32				m_RoleReturnTime;
	// �ع��״ε�½
	UInt32				m_ReturnFirstLogin;
	// �ع�����ƺ�
	UInt32				m_ReturnYearTitle;
	//��װ�������
	bool m_HaveSuit;

	// ��Ϸ����
	std::map<UInt32, std::string>	m_GameSetMap;

	//��ɫͷ��
	PlayerTitleRoleMgr	m_TitleMgr;

	//��ǰ����ͷ��GUID
	UInt64		m_curWearTitleGuid;

	//�����в�ѯ��ֹ��ѯ�۸��б�ʱ���
	UInt64				 m_AuctionProhibitQueryPriceListTimeStamp;

	//������Ȩ
	bool					m_HaveGiftRight = false;

	//�ܷ���������Ȩ��
	bool					m_CanBuyGiftRight = false;	
	
	//�ܷ�����Ԩ��Ȩ��
	bool					m_CanBuyDungeonRight = false;

	//������ɫ������Ϣ
	std::vector<RoleBaseInfo>			m_OtherRoleInfo;


};

typedef CLVisitor<WSPlayer> WSPlayerVisitor;


/**
 *@brief ��ѯ������ݷ���
 */
class SelectRoleInfoCallback : public CLRecordCallback
{
public:
	SelectRoleInfoCallback(WSPlayer* player)
		:m_PlayerId(player->GetID()), m_LoadSerial(player->IncLoadDataCount()){}
	~SelectRoleInfoCallback(){}

public:
	virtual void OnFinished(WSPlayer* player) = 0;
	virtual void OnFailed(WSPlayer* player) = 0;

private:
	void OnFinished();

	void OnLoadFailed();

private:
	//���id
	ObjID_t m_PlayerId;
	//�������
	UInt8	m_LoadSerial;
};

#endif
