#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <CLPlayerDataEntry.h>
#include <CLVipDefine.h>
#include <CLVipDataEntry.h>
#include <CLChatDefine.h>
#include <CLMaskProperty.h>
#include <CLRequest.h>
#include <map>
#include <CLMatchProtocol.h>
#include <CLSysNotify.h>
#include <CLPkLevelDataEntry.h>
#include <kingnet_analyzer.h>
#include <CLDeathTowerAwardDataEntry.h>
#include <AvalonDailyTimer.h>
#include <CLGuildDefine.h>
#include <CLGameSessionProtocol.h>
#include <CLBillingProtocol.h>
#include <CLRelationDefine.h>
#include <CLFigureStatueDefine.h>
#include <CLMoneyManageDefine.h>
#include <CLScoreWarDefine.h>
#include <CLActivityInfo.h>

#include "Creature.h"
#include "ItemMgr.h"
#include "TaskMgr.h"
#include "QuickBar.h"
#include "ItemCD.h"
#include "CounterMgr.h"
#include "ActionCounterMgr.h"
#include "RoutineMgr.h"
#include "InviteMgr.h"
#include "EquipShopMgr.h"
#include "DungeonMgr.h"
#include "Fatigue.h"
#include "ShopMgr.h"
#include "PkStatisticMgr.h"
#include "RetinueMgr.h"
#include "Vip.h"
#include "OpenJarMgr.h"
#include "DungeonMgr.h"
#include "SeasonLevel.h"
#include "PetMgr.h"
#include "MallCheckBuyMgr.h"
#include "AbnormalTransactionMgr.h"
#include "ActivityAttributeMgr.h"
#include "HeadFrameMgr.h"
#include <CLLostDungeonDefine.h>

class Trade;
class Team;
class QuickBuyTransaction;
class PlayerLoadingCallback;
class PlayerSenceShareData;

/**
 *@brief ���״̬
 */
enum PlayerStatus
{
	PS_INIT,		//�մ����ȴ����ݻ�ע�ᣬ��״̬����world�򳡾�
	PS_NORMAL,		//����״̬�����ڳ�����world��
	PS_LEAVEGAME,	//�뿪��Ϸ��ʱ�׶�
    PS_RECONNECT,   //����
	PS_FINAL,		//�浵
	PS_DESTORY,		//���ݻ�
	PS_NUM
};

/**
*@brief ������״̬
*/
enum WudaoStatus
{
	// ��ʼ״̬��δ�μӣ�
	WUDAO_STATUS_INIT,
	// ������
	WUDAO_STATUS_PLAYING,
	// �����
	WUDAO_STATUS_COMPLETE,
};

/**
*@brief ��ʼ�жϴ�����������ҵȼ�
*/
#define PLAYER_LEVEL_ON_BEGIN_CHECK_ACTIVATE_COND 7

/**
*@brief �����Ұ���ȼ�
*/
struct PlayerSightPriority
{
	ObjID_t	id;
	UInt16	level;
	bool	isFriend;

	bool operator<(const PlayerSightPriority& other) const
	{
		if (isFriend != other.isFriend)
		{
			return isFriend ? true : false;
		}

		if (level != other.level)
		{
			return level > other.level;
		}

		return id < other.id;
	}
};

#define PLAYERINFO(T) \
	"accid(" << T->GetAccID() << ") playerid(" << T->GetID() << ") occu(" << T->GetOccu() << ")"

/**
 *#brief �����  notedy by aprilliu, Player����scene server�յ�world server������SceneEnterGameReqʱ���д����ģ� Object_ID���Ǵ����Ľ�ɫid (һ��guid)
 */
class Player : public Creature
{
	AVALON_DEFINE_OBJECTPOOL(Player, Avalon::NullMutex)  //by aprilliu,�ײ�ʹ��Avalon::ObjectPool���洢Player����ͬʱʹ��PlayerMgr ������Player����std::map ��Object_tΪkey��

	typedef std::map<ObjID_t, UInt16>	FriendMap;

public:
	//��������ӳ�, 30����
	const static UInt32 LEAVEGAME_TIMEOUT = 3 * 1000;
	//�Ʋ�����ʱ��
	const static UInt8	ASSET_LOCK_TIME = 5;
	//�߼�����Сʱ
	const static UInt8 LOGIC_DAYCHANGE_HOUR = 6;

public:
	Player();
	~Player();

public:
	/**
	 *@brief ��ʼ����
	 */
	bool SetupBase();

	/**
	 *@brief ���û�ȡ״̬
	 */
	void SetGameStatus(UInt8 status)
	{ 
		m_GameStatus = status;
		m_StatusTime = (UInt32)CURRENT_TIME.Sec();
	}
	UInt8 GetGameStatus() const { return m_GameStatus; }
	UInt32 GetStatusTime() const { return m_StatusTime; }

	/**
	 *@brief ���û�ȡ����id
	 */
	bool SetGateID(UInt32 nodeid);
	UInt32 GetGateID() const;

	/**
	 *@brief ���û�ȡ�˺�id
	 */
	void SetAccID(UInt32 accid){ m_AccId = accid; }
	UInt32 GetAccID() const{ return m_AccId; }

	/**
	*@brief �Ƿ��ֻ���
	*/
	void SetPhoneBind(bool bind){ m_IsPhoneBind = bind; }
	bool IsPhoneBind() const{ return m_IsPhoneBind; }

	/**
	*@brief ���û�ȡ�˺�guid
	*/
	void SetAccGuid(ObjID_t guid){ m_AccountGuid = guid; }
	ObjID_t GetAccGuid() const{ return m_AccountGuid; }

	/**
	*@brief ����id�������Ƿ��䣩
	*/
	void SetMajorCityID(UInt32 id){ m_MajorCityId = id; }
	UInt32 GetMajorCityID() const{ return m_MajorCityId; }


	/**
	 *@brief ��ȡbase64����
	 */
	const char* GetBase64ID() const;

	SceneObjectType GetType() const { return SOT_PLAYER; }

	/**
	 *@brief �������
	 */
	UInt64 GetExp() const { return m_Exp; }
	UInt64 DecExp(const char* reason, UInt64 exp);
	void   IncExp(const char* reason, UInt64 exp);
	bool   AddExpFromRewardAdapter(UInt32 key, UInt16 subKey);

	/**
	*@brief ���ݵ�ǰ�ȼ��ľ�����Ӱٷֱ�
	*/
	void   AddExpByLevelPercent(const char* reason, UInt32 percent);

	/**
	*@brief ���ݵȼ��鿴��Ҫ�ľ���
	*/
	UInt64 GetLevelExp(UInt8 level);

	/**
	 *@brief �ж��Ƿ�����(���鶼����)
	 */
	bool IsFullLevel() const;

	/**
	*@brief ���á���þ���ӳ�buff
	*/
	void SetExpAddBuff(Buff *pBuff) { m_pExpAddBuff = pBuff; }
	Buff* GetExpAddBuff() { return m_pExpAddBuff; }

	/**
	 *@brief ��ȡְҵ
	 */
	UInt8 GetOccu() const { return m_Occu; }
	UInt8 GetPreOccu() const { return m_PreOccu; }
	UInt8 GetMainOccu() const { return m_Occu / 10; }
	UInt8 GetOrigOccu() const { return GetMainOccu() * 10; }
	bool IsSameOccu(int occu) { return m_Occu / 10 == occu / 10; }	//by huchenhui
	void ChangeOccu(UInt8 occu);		//by huchenhui
	bool IsInitOccu() { return m_Occu % 10 == 0; }		//by huchenhui �Ƿ��ʼְҵ
	Occupation GetBaseOccu() const { return (Occupation)((m_Occu / 10) * 10); }
	Occupation GetOccupation() { return (Occupation)(m_Occu.GetDBValue()); }
	bool IsChangeOccu(UInt8 occu)	{ return (occu % 10) == 0 ? false : true; }			//�Ƿ���תְ

	/**
	*@brief �������
	*/
	UInt32 Awaken();
	void SetAwaken(AwakenStatus awaken) { m_Awaken = awaken; }
	UInt8 GetAwaken() { return m_Awaken; }

	/**
	*@brief ����������У������Ƿ��䣩
	*/
	UInt32 GetBirthCity();

	/**
	 *@brief �ж��Ƿ�ӵ��ĳ��gmȨ��
	 */
	void SetGmAuthority(UInt8 authority){ m_GmAuthority = authority; }
	bool HasGmAuthority(UInt8 authority) const{ return m_GmAuthority >= authority; }

	/**
	 *@brief �����жϿͻ���ѡ��
	 */
	void SetClientOption(UInt8 option){ m_ClientOption = option; }
	bool HasClientOption(UInt8 flag) const{ return (m_ClientOption & flag) != 0; }

	/**
	*@brief ������ˢ��ʱ��
	*/
	void SetAuctionRefreshTime(UInt32 time) { m_AuctionRefreshTime = time; }
	UInt32 GetAuctionRefreshTime() const { return m_AuctionRefreshTime; }

	/**
	*@brief �����ж��⹺�����λ
	*/
	void SetAuctionAdditionBooth(UInt32 num) { m_AuctionAdditionBooth = num; }
	UInt32 GetAuctionAdditionBooth() const { return m_AuctionAdditionBooth; }

	/**
	*@brief Oppo Vip�ȼ�
	*/
	UInt8 GetOppoVipLevel() const { return m_OppoVipLevel; }
	void SetOppoVipLevel(UInt8 level) { m_OppoVipLevel = level; }

	/**
	 *@brief ��ȡ����ʱ��
	 */
	UInt64 GetOnlineTime() const{ return m_OnlineTime; }
	UInt64 GetOfflineTime();

	//������ߵ�������,����һ��Ϊһ��,����:1������,4������,���Ϊ3��
	UInt32 GetOfflineDay();

	//���ָ��ʱ�̵���������,����һ��Ϊ����
	UInt32 GetOfflineDay(UInt32 Hour);

	//���ָ��ʱ����ڵ���������,����һ��Ϊ����
	UInt32 GetIntervalDay(UInt64 startTime, UInt64 endTime, UInt32 hour);

	////���ָ��ʱ�̵���������,����һ��Ϊ����, updateTime:��һ�θ��µ�ʱ��
	UInt32 GetOfflineDay(UInt32 Hour, UInt32 updateTime);

	UInt32 GetDayOnlineTime();
	/**
	 *@brief ���û�ȡ�����浵ʱ��
	 */
	void SetLocalSavebackTime(UInt64 time){ m_LocalSavebackTime = time; }
	UInt64 GetLocalSavebackTime() const{ return m_LocalSavebackTime; }

	/**
	 *@brief ͬ�����Ա仯
	 */
	void SyncProperty();

	/**
	 *@brief ���������ȼ�
	 */
	void SetTestLevel(UInt16 level);
	void Levelup();
	void LevelDown();

	/**
	 *@brief ���û�ȡ����
	 */
	void SetCity(const std::string& city){ m_City = city; }
	const std::string& GetCity() const{ return m_City; }

	/**
	 *@brief ���û�ȡip
	 */
	void SetIp(const std::string& ip){ m_Ip = ip; }
	const std::string& GetIp() const{ return m_Ip; }

	/**
	 *@brief ���÷�æʱ��
	 */
	void SetBusyTime(){ m_BusyTime = CURRENT_TIME.MSec(); }
	UInt64 GetBusyTime(){ return m_BusyTime; }

	/**
	 *@brief ��ȡ����ʱ��
	 */
	UInt32 GetCreateTime() const{ return m_CreateTime; }
	UInt32 GetCreateFromNowOn();
	UInt32 GetCreateFromNowOnDay();
	//���������ڵ���������6������
	UInt32 GetCreateFromNowOnLogicDay();

	/**
	 *@brief ������Դ
	 */
	void SetSource1(const std::string& source1){ m_Source1 = source1; }
	void setSource2(const std::string& source2){ m_Source2 = source2; }

	/**
	 *@brief ��ȡ��id
	 */
	UInt16 GetZoneId() const{ return m_ZoneId; }

	/**
	 *@brief ���û�ȡԭ��id
	 */
	void SetSrcZoneId(UInt16 zoneid){ m_SrcZoneId = zoneid; }
	UInt16 GetSrcZoneId() const{ return m_SrcZoneId; }

	/**
	*@brief ���û�ȡ������id
	*/
	void SetBornZoneId(UInt32 zoneId) { m_BornZoneId = zoneId; }
	UInt32 GetBornZoneId() const { return m_BornZoneId; }

	/**
	 *@brief �뿪��Ϸ, ע����ô˽ӿڻ��ͷŶ���
	 */
	void OnLeaveTimeout();
	void LeaveGame();
	void UnRegisterWorldPlayer();
	void LeaveGameForTour(UInt32 dstZoneId, UInt32 mapId, const Avalon::SockAddr& addr, UInt8* key);

	//�߼�����,����������߻�û�������ͷ�ʱ�Ĵ���
	void OnLogicOffLine();

	/**
	 *@brief �ߵ���������
	 */
	void KickOff(UInt32 erroCode = 0);

	/**
	 *@brief ������Ϊ��־
	 *@return true �״����� false ԭ��������
	 */
	bool SetActionFlag(UInt32 flag);
	bool TestActionFlag(UInt32 flag) const;

	/**
	*@brief ������ҵȼ�
	*/
	void IncLevel();

	/**
	 *@brief ͬ������
	 */
	void SyncRequest(UInt8 type, Player* pRequester, const std::string& param = "");

	/**
	 *@brief ���û�ȡopenid
	 */
	void SetOpenId(const std::string& openid){ m_OpenId = openid; }
	const std::string& GetOpenId() const{ return m_OpenId; }
	void ClearOpenIdDirtyFlag() { m_OpenId.ClearDirty(DB_SYNC_TYPE); }

	/**
	 *@brief ����������
	 */
	void SetInviter(UInt32 inviter){ m_Inviter = inviter; }

	/**
	 *@brief ���û�ȡƽ̨id
	 */
	void SetPf(const std::string& pf){ m_Pf = pf; }
	const std::string& GetPf() const{ return m_Pf; }
	const char* ScriptGetPf() const{ return m_Pf.c_str(); }
	void ClearPfDirtyFlag() { m_Pf.ClearDirty(DB_SYNC_TYPE); }

	/**
	*@brief ���û�ȡ�豸id
	*/
	void SetDeviceId(const std::string& did) { m_DeviceId = did; }
	const std::string& GetDeviceId() const{ return m_DeviceId; }
	const char* ScriptGetDeviceId() const { return m_DeviceId.c_str(); }

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
	*@brief �ͻ����Ƿ���loading��
	*/
	void SetClientLoading(bool loading) { m_IsClientLoading = loading; }
	bool IsClientLoading() const { return m_IsClientLoading; }

	/**
	 *@brief ��ȡ���Ǻ�������½����                                                                     
	 */
	UInt32 GetContinuousOnlineDays() const{ return m_ContinuousDays; }

	/**
	 *@brief ���                                                                     
	 */
	void ChangeZone(UInt32 zoneId, UInt32 mapId);
	void ReturnZone();

	/**
	*@brief ֪ͨworldһЩ�¼�
	*/
	void SyncEventToWorld(SyncEventType type, ObjID_t param1);

	/**
	*@brief ��ȡ��ұ�ǩ��Ϣ
	*/
	void GetPlayerLabelInfo(PlayerLabelInfo& info);

	/**
	*@brief ���û�ȡð�ն���
	*/
	void SetAdventureTeamName(const std::string& name) { m_AdventureTeamName = name; }
	
public: //�ͷ����
	/**
	 *@brief �����жϳͷ���־
	 */
	void SetPunishFlag(UInt32 flag){ m_PunishFlag |= (1 << flag); }
	bool CheckPunishFlag(int type) const{ return (m_PunishFlag & ( 1 << type)) != 0; }

	/** 
	 *@brief ���ý��Ե���ʱ��
	 */
	void	SetForbitTalkDueTime(UInt32 duetime){ 
		if (m_ForbitTalkDueTime < duetime)
		{
			m_ForbitTalkDueTime = duetime;
		}
	}
	UInt32	GetForbitTaleDueTime() const{ return m_ForbitTalkDueTime; }
	void	ClearForbitTalkDueTime()	{ m_ForbitTalkDueTime = 0; }

public:	//vip���
	/**
	 *@brief ���û�ȡvip�ȼ�
	 */
	void SetVipLvl(UInt8 level);
	UInt8 GetVipLvl();
	UInt32 GetVipExp();

	/**
	*@brief �ܵĳ�ֵ���
	*/
	void InitTotalChargeNum(UInt32);
	void AddTotalChargeNum(UInt32 num);
	
	/**
	*@brief �ܵĳ�ֵ����ɫ��
	*/
	UInt32 GetTotalChargeNumOnRole() const { return m_TotleChargeNum; }

	/**
	 *@brief ����vip����buff                                                                     
	 */
	void SetVipBuff(Buff* pVipBuff){ m_pVipBuff = pVipBuff; }
	Buff* GetVipBuff() const{ return m_pVipBuff; }

	/**
	 *@brief ���û�ȡvip����ʱ��
	 */
	void SetVipDueTime(UInt32 dueTime){ m_VipDueTime = dueTime; }
	UInt32 GetVipDueTime() const{ return m_VipDueTime; }

	/**
	 *@brief ����qq��Ȩ��Ϣ
	 */
	void SetQQVipInfo(const QQVipInfo& info){ m_qqVipInfo = info; }
	const QQVipInfo& GetQQVipInfo() const{ return m_qqVipInfo; }

	//����VIP����
	void AddVipExp(const char* reason, UInt32 exp) { m_Vip.AddExp(reason, exp); }

	//��ȡȨ�޵�ֵ
	void GetVipValue(VipPrivilegeType type, UInt32& value) { m_Vip.GetValue(type, value); }

	//��ȡȨ�޵�ֵ
	UInt32 GetVipValue(VipPrivilegeType type) { UInt32 value = 0; m_Vip.GetValue(type, value); return value; }

	//��ȡȨ�޵�ֵ
	void GetVipValue(VipPrivilegeType type, bool& value) { m_Vip.GetValue(type, value); }

	//��ȡȨ�޵�ֵ
	UInt32 GetVipValueById(UInt32 id) { return m_Vip.GetValueById(id); }


	//�������
	UInt32 BuyGiftItem(UInt8 vipLevel) { return m_Vip.BuyGiftItem(vipLevel); };

	//��ʼ��������
	void InitVip(UInt8 level, UInt32 exp) { return m_Vip.Init(level, exp); }
	void AdjestChargeInfo(UInt8 vipLevel, UInt32 vipExp, UInt32 point, UInt32 totalChargeNum);

public://�л��������
	/**
	*@brief �ܷ���볡��
	*/
	bool CanEnterMap(UInt32 uMapId);

	bool IsInDungeon();

	bool IsInPK();

    /**
    *brief ��������
    */
    const ScenePos GetScenePos() const;
    void SetScenePos(UInt32 x, UInt32 y);
    void UpdateScenePosition();
    void RequestStopMoving(ScenePos pos);
    void AdjuestScenePosition(int &x, int &y);

    /**
    *brief �����л�����
    */

    void OnSceneChangeReq(UInt32 curDoorId, UInt32 dstSceneId, UInt32 dstDoorId);

    /**
    *brief �˶�����
    */
    const SceneDir GetSceneDir() const { return m_SceneDir; }
    void SetSceneDir(SceneDir dir) { m_SceneDir = dir; }

    /**
    *brief �˶�
    */
    bool CanMoveToTarget(const ScenePos& pos) const;
    const bool IsRuning() const { return m_Runing; }
    void SetRuning(bool runing) { m_Runing = runing; }

    /**
    *brief ��һ���ƶ�ʱ��
    */
    void SetLastMoveTime(Avalon::Time now) { m_LastMoveTime = now; }
    Avalon::Time GetLastMoveTime() const { return m_LastMoveTime; }

public://�������

	//���ص�������
	bool LoadItem(Item* item, bool newPos);

	//ʹ�õ���
	UInt32 UseItem(ObjID_t uid, UInt8 useAll, UInt32 param1, UInt32 param2);
	//���ʹ�õ���
	UInt32 QuickUseItem(UInt8 idx, UInt32 dungenID);

	//���۵���
	UInt32 SellItem(ObjID_t id, UInt16 num = 1);
	//�������۵���
	UInt32 SellItemBat(std::vector<UInt64> itemUids);
	//��ֿ�
	UInt32 PushStorage(ObjID_t id, UInt16 num = 1, UInt8 storageType = PACK_STORAGE);
	//ȡ�ֿ�
	UInt32 PullStorage(ObjID_t id, UInt16 num = 1);
	//һ����ֿ�
	UInt32 OneKeyPushStorage();

	Item* FindCarryOnItem(ObjID_t id);
	Item* FindItem(ObjID_t id);
	Item* FindItem(const ItemPos &pos);
	
	void GetEquips(std::vector<Item*>& equips) const;
	void GetSecondWeapons(std::vector<Item*>& weapons);
	Item* GetEquip(UInt8 subType);

	void GetRaceItem(std::vector<RaceItem>& items);
	void GetRaceBuffs(std::vector<RaceBuffInfo>& buffs);
	void GetRaceRetinues(std::vector<RaceRetinue>& retinues);
	void GetRacePets(std::vector<RacePet>& pets);
	
	//���ٵ������� ��������Ϊ0ʱ ����ɾ��
	UInt32 ReduceItemNum(const char* reason, Item* item, UInt16 num=1);
	void VisitItems(UInt8 pack, ItemVisitor& visitor);
	
	//��ȡʣ�౳������
	UInt32 GetPackGridLeft() const{ return m_ItemMgr.GetPackage(PACK_MATERIAL)->GetCapacity(); }
	UInt32 GetPackGridLeftByPack(PackType pack) const{ return m_ItemMgr.GetPackage(pack)->GetCapacity(); }
	UInt32 GetPackItemNum(UInt8 pack) const;

	//�������
	void TrimItems(UInt8 pack){ m_ItemMgr.TrimItems(pack); }
	//�����ֽ�װ��
	UInt32 DecomposeEquip(const char* reason, std::vector<ObjID_t>& uidList, ItemRewardVec& getItems, bool sendMail = false);
	//�����ֽ�װ��
	UInt32 DecomposeEquip(ObjID_t uid);
	//ǿ��װ��
	UInt32 StrengthenEquip(ObjID_t equipUid, bool useUnbreak, ObjID_t strTicket);
	//ģ��ǿ��for test
	bool SimStrength(UInt32& strLv, UInt32 equipLv, UInt32 color, UInt32 subType, UInt32 destStrLv, 
		UInt32& fix1Ct, UInt32& fix2Ct, UInt32& strNum, UInt32& failNum, UInt32 lv2failNum[], 
		UInt32& useProtectNum, UInt32& costGold, UInt32& costColor, UInt32& costColorless);

	//װ����װ
	UInt32 CheckSealEquip(ObjID_t equipUid, UInt16& needNum, UInt32& needId);
	UInt32 SealEquip(ObjID_t equipUid);
	//װ��Ʒ������
	UInt32 ReMakeEquipQlv(ObjID_t equipUid, UInt8 autoUsePoint, UInt8 usePerfect); 
	//װ��ת��
	UInt32 TransferEquip(ObjID_t equipUid, ObjID_t transferId);
	//ʱװ�ϳ�
	UInt32 FashionCompose(ObjID_t fashionA, ObjID_t fashionB);
	void SendFashionComposeBack(UInt32 code, UInt8 type, UInt32 commonId, UInt32 advanceId, UInt32 wingId);
	//ʱװ��ʱװ�ϳɷ���
	void SendFashionCompBackChangeActive(UInt32 ret, UInt8 type, UInt32 commonId, UInt32 advanceId, bool allMerged);

	//װ������
	UInt32 EuqipMake(UInt32 equipId);

	/**
	 *@brief ���û�ȡ������ʹ��ʱ��
	 */
	void SetItemUseTime(UInt8 id, UInt64 usetime, UInt32 maxTime){ m_ItemCD.SetUseTime(id, usetime, maxTime); }
	UInt64 GetItemUseTime(UInt8 id) const{ return m_ItemCD.GetUseTime(id); }

	/**
	 *@brief ���ɾ������
	 */
	UInt32 GetItemNum(UInt32 id);
	UInt32 GetItemNumWithEqualItem(UInt32 id);  
	bool RemoveItem(const char* reason, UInt32 id, UInt32 num);
	Int32 RemoveItemWithEqualItem(const char* reason, UInt32 id, UInt32 num);
	UInt32 GetBindItemNum(UInt32 id);
	UInt32 GetNoBindItemNum(UInt32 id);
	void RemoveNoBindItem(const char* reason, UInt32 id, UInt32 num);

	/**
	*@brief �����Ƴ����ֽϵ͵ĵ���
	*/
	bool RemoveLessScoreItem(const char* reason, UInt32 id, UInt32 num);

	/**
	 *@brief �Ƴ����ߣ��˽ӿڲ���ɾ��item����
	 */
	void RemoveItemDirect(const char* reason, Item* item);

	/**
	 *@brief �����ӵ���
	 */
	bool CheckAddItem(UInt32 id, UInt16 num);


	UInt16 AddItem(const char* reason, UInt32 id, UInt32 num, UInt8 qualityLv = 0, UInt8 strenth = 0, bool getNotify = true);

	UInt16 AddItemWithCallBack(const char* reason, UInt32 id, UInt32 num,
		const std::function<bool(Item&)>& initCallback,
		const std::function<void(Item&)>& makedCallback,
		UInt8 qualityLv = 0, UInt8 strenth = 0, bool getNotify = true);

	UInt16 AddItemDirect(const char* reason, Item* item, bool notify = true, bool mailGive = false);


	//for test
	void EquipItem(UInt32 id);

	/**
	 *@brief �����轱��
	 */
	RewardGroup* GetRewardGroup();
	RewardGroup* GetRewardGroupNoClear();
	inline void ClearRewardGroup() { m_RewardGroup.ClearRewards(); };

	//��鱳���Ƿ��ܼӵ���,srcItem��������������߼�,��ʾ����Դ����, srcNum�Ƕ�Ӧ����,�������Դ���߱�ɾ������ӵı߽����
	//ע��÷���������Ҫ��ɾ�������,������п����ڱ߽���������ʧ��
	UInt32 CheckAddRewards(RewardGroup* group, Item* srcItem = NULL, UInt16 srcNum = 0);
	UInt32 CheckAddLimitItems(RewardGroup* group);
	//��������Ҫ���ʼ��Ŀ���ֱ��ʹ��AddRewards
	void AddRewardsBase(const char* reason, RewardGroup* group, bool notice, UInt32 validDay, bool sendMail, bool checkGetLimit, bool openJar, bool mailGive = false, bool notifyNewItem = true);
	void AddRewards(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY, bool sendMail = true, bool checkGetLimit = false);
	void AddJarRewards(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY);
	void AddRewardsByMailGive(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY, bool sendMail = true, bool checkGetLimit = false);
	UInt32 CheckAddRewardsScript(RewardGroup* group);
	void AddRewardsNotNotify(const char* reason, RewardGroup* group, bool notice = false, UInt32 validDay = MAIL_VALID_DAY, bool sendMail = true, bool checkGetLimit = false);
	/**
	*@brief ������Դ��ȡ�ʼ���Ϣ
	*/
	void GetMailInfoByReason(const char* reason, std::string& senderName, std::string& title, std::string& content);

	/**
	 *@brief ������
	 */
	void AddGold(const char* reason, UInt32 num);
	UInt32 GetGold() const { return m_Gold; }
	UInt32 RemoveGold(const char* reason, UInt32 num);

    /**
    *@brief ����֮�����
    */
    void AddWarriorSoul(const char* reason, UInt32 num);
    UInt32 GetWarriorSoul() const { return m_WarriorSoul; }
    void RemoveWarriorSoul(const char* reason, UInt32 num);

	/**
	*@brief ���������
	*/
	void AddPkCoin(const char* reason, UInt32 num);
	UInt32 GetPkCoin() const { return m_PkCoin; }
	void RemovePkCoin(const char* reason, UInt32 num);
	UInt32 AddPkCoinFromRace(UInt32 num);
	UInt32 AddPkCoinFromActivity(UInt32 num);

	/**
	*@brief ���ӻ���
	*/
	void AddGoldJarPoint(const char* reason, UInt32 num);
	void AddMagJarPoint(const char* reason, UInt32 num);
	inline UInt32 GetGoldJarPoint()	{ return m_GoldJarPoint.GetDBValue(); }
	inline UInt32 GetMagJarPoint()	{ return m_MagJarPoint.GetDBValue(); }
	void RemoveGoldJarPoint(const char* reason, UInt32 num);
	void RemoveMagJarPoint(const char* reason, UInt32 num);

	/**
	*@brief ÿ�մ�ս���л�þ����ҵ��������
	*/
	UInt32 DailyMaxPkCoinFromRace();

	/**
	*@brief ÿ�մӻ�л�þ����ҵ��������
	*/
	UInt32 DailyMaxPkCoinFromActivity();

	/**
	*@brief ���������
	*/
	void AddReviveCoin(const char* reason, UInt32 num);
	UInt32 GetReviveCoin() const { return m_reviveCoin; }
	void RemoveReviveCoin(const char* reason, UInt32 num);

	/**
	*@brief SP���
	*/
	void AddSP(const char* reason, UInt32 num);
	void AddPveSP(const char* reason, UInt32 num);
	void AddPvpSP(const char* reason, UInt32 num);
	UInt32 GetPveSP() const { return m_SP; }
	UInt32 GetPvpSP() const { return m_PvpSP; }
	void SetPveSP(UInt32 num) { m_SP = num; }
	void SetPvpSP(UInt32 num) { m_PvpSP = num; }
	bool RemovePveSP(const char* reason, UInt32 num);
	bool RemovePvpSP(const char* reason, UInt32 num);
	UInt32 GetAllSP();

	/**
	*@brief ƣ��ֵ���
	*/
	void AddFatigue(const char* reason, UInt16 num, bool notify = false) { m_Fatigue.AddFatigue(reason, num, notify); }
	void RemoveFatigue(const char* reason, UInt16 num) { m_Fatigue.RemoveFatigue(reason, num); }
	UInt16 GetFatigue() { return m_Fatigue.GetFatigue(); }
	UInt16 GetUseFatigue() { return m_Fatigue.GetUseFatigue(); }
	UInt16 GetLostFatigue() { return m_Fatigue.GetLostFatigue(); }
	UInt32 GetLastFatigue()	{ return m_Fatigue.GetLastFatigue(); }
	UInt16 GetMaxFatigue()	{ return m_Fatigue.GetMaxFatigue(); }

	/**
	 *@brief ������
	 */
	void AddBindGold(const char* reason, UInt32 num);
	UInt32 GetBindGold() const { return m_BindGold; }
	void RemoveBindGold(const char* reason, UInt32 num, UInt32 itemId = 0, UInt32 itemNum = 0);

	/**
	 *@brief ��ȯ���
	 */
	void AddPoint(const char* reason, UInt32 num);
	UInt32 GetPoint() const { return  m_Point; }
	void RemovePoint(const char* reason, UInt32 num, UInt32 itemId = 0, UInt32 itemNum = 0);
	void InitPoint(UInt32 point) { m_Point = point; }

	/**
	 *@brief  �󶨵�ȯ���                                                                 
	 */
	void AddBindPoint(const char* reason, UInt32 num);
	UInt32 GetBindPoint() const { return m_BindPoint; }
	void RemoveBindPoint(const char* reason, UInt32 num);

	/**
	 *@brief ����Ǯ�����
	 */
	bool CheckAddMoneyItem(UInt8 type, UInt32 num);
	void AddMoneyItem(const char *reason, UInt8 type, UInt32 num);

	void SetCustomField(UInt32 field) { m_CustomField = field; }

	/**
	 *@brief ��ȡ��ң������󶨽�ң�
	 */
	UInt32 GetGoldBoth() const { return  m_BindGold + m_Gold; }
	/*
	 *@brief �Ƴ���ң����Ȱ󶨽�ȣ�
	 *@return true �Ƴ��˰󶨽�� false û��
	 */
	bool RemoveGoldBoth(const char* reason, UInt32 num);

	//�ж��ܷ��Ǯ
	bool CheckRmMoney(ItemSubType moneyType, UInt32 cost);
	bool CheckRmMoney(ItemDataEntry* data, UInt32 cost);
	bool RemoveMoney(const char* reason, ItemSubType moneyType, UInt32 num);

	//��ȯ���
	UInt32 GetPointBoth() const { return m_BindPoint + m_Point; }
	bool RemovePointBoth(const char* reason, UInt32 num);

	/**
	 *@brief ����֧�����
	 */
	void SendBillingResult(UInt32 billingId, UInt8 result);

	/**
	 *@brief ���������С
	 */
	UInt32 EnlargePackage();
	UInt32 EnlargeStorage(UInt8 count = 0);
	void InitPackage();
	void InitStoragePack(UInt8 sz);

	// �Ѿ���չ�ı�������
	UInt32 GetEnlargePackageSize();

	// �Ѿ���չ�Ĳֿ����
	UInt32 GetEnlargeStoragePackSize();

	/**
	 *@brief ����Ƿ�ɽ��вֿ����
	 */
	bool CheckUseStorage();

	//������
	void SendMagicJarResult(const char* bonusPool, UInt32 getBonusID, UInt16 getNum);

	//��ħ
	UInt32 AddMagic(Item* card, Item* item);
	UInt32 ComposeMagicCard(ObjID_t card_1, ObjID_t card_2, UInt32& cardId);

	/**
	*@brief ����
	*/
	UInt32 AddPreciousBeadToItem(Item* preciousBead, Item* item);

	//��ȡװ������ by huchenhui 2016.09.13
	UInt32 GetEquipSize(UInt16 level, UInt8 quality, ItemSubType subType = ItemSubType::ST_ITEM_INVALID) { return m_ItemMgr.GetEquipSize(level, quality, subType); }

	//��ȡ����װ������ by huchenhui 2016.09.13
	UInt32 GetGreenEquipSize(UInt16 level) { return m_ItemMgr.GetGreenEquipSize(level); }

	//��ȡ����װ������ by huchenhui 2018.03.20
	UInt32 GetFashionSize(UInt8 quality, ItemSubType subType = ItemSubType::ST_ITEM_INVALID) { return m_ItemMgr.GetFashionSize(quality, subType); }

	inline ItemMgr& GetItemMgr() { return m_ItemMgr; }
	inline OpenJarMgr& GetJarMgr()	{ return m_JarMgr; }

	// ��������
	void CountMoveSpeed();

	// ������
	UInt32 OpenJar(UInt32 jarId, UInt8 comboNum, UInt32 opActId);

	// ��¼��������
	void RecordGetSkyFashion(UInt32 fashionId);

	// ���û�������
	void ResetGetSkyRecord(UInt32 fashionId);

	// �Ƿ��ù��������
	bool HasGetSkyFashion(UInt32 fashionId);

	/**
	*@brief ��ȫ��
	*/
	void SetSecurityStatus(UInt32 _status) { m_SecurityStatus = _status; }
	UInt32 GetSecurityStatus() { return m_SecurityStatus; }

	bool SecurityLockCheck(bool _isNotify = true);
	bool SecurityLockCheckByColor(std::vector<ObjID_t>& uidList, UInt32 _color);

private:
	UInt32 GetIncome(UInt32 type);

public://������
	bool LoadJarItem(JarItem* jarItem);
	void InitJarMgr();
	void OpenJarSample(UInt32 type, UInt32 combo, UInt32 num);

	inline UInt32 GetLastGoldJarPtRstTime()	{ return m_LastResetGoldJarPointTime.GetDBValue(); }
	inline void SetLastGoldJarPtRstTime(UInt32 time)	{ m_LastResetGoldJarPointTime = time; }
	inline UInt32 GetLastMagJarPtRstTime()		{ return m_LastResetMagJarPointTime.GetDBValue(); }
	inline void SetLastMagJarPtRstTime(UInt32 time)		{ m_LastResetMagJarPointTime = time; }


public://buff���
	bool AddBuff(UInt32 buffId);

public: //��������
	UInt32	GetAllEquipLvMin();							//���ȫ��װ���ȼ�����(1-9�Ų�λ,�������ƺ�)
	UInt32	GetAllEquipQualityMin();					//���ȫ��װ��Ʒ������(1-9�Ų�λ,�������ƺ�)
	UInt32	GetWearFashionNum();						//����Ѵ���ʱװ����
	UInt8	GetEquipNumByStrLv(UInt32 strLv);			//���ָ��ǿ���ȼ�װ������(1-9�Ų�λ,�������ƺ�)
	UInt8	GetStrMaxLv(UInt8 num, UInt8 needStrLv);								//���װ���е����ǿ���ȼ�
	UInt8	GetStrMaxLvAll();								//���װ���кͱ���������ǿ���ȼ�
	UInt8	GetEquipNumByLv(UInt32 lv);					//���ָ���ȼ�װ������(1-9�Ų�λ,�������ƺ�)
	UInt32	GetTitleNum();								//��óƺ�����
	UInt32	GetWearEquipNumByQua(UInt8 color);			//���ָ��Ʒ�����ϵ��Ѵ���װ������
	UInt32	GetSignInCount();							//��ȡ�ۻ�ǩ������

public://�˺����
	void SyncAccountInfo(AccountInfoType type, UInt32 value);

public://�������
	/**
	 *@brief ����
	 */
	Task* FindTask(UInt32 id);

	Task* FindAchievementTaskTask(UInt32 id);
	/**
	 *@brief ��ȡ���ύ������һ������
	 */
	bool AcceptTask(UInt32 id, UInt8 quality, bool bForce = false);
	bool ScriptAcceptTask(UInt32 id){ return AcceptTask(id, 0); }
	bool SubmitTask(UInt32 id, bool force = false);
	void AbandonTask(UInt32 id);

	/**
	*@brief �����
	*/
	bool SubmitActiveTask(UInt32 id);
	bool AcceptActiveTask(UInt32 id);
	bool RemoveActiveTask(UInt32 id);
	void OnActiveClosed(UInt32 id);

	/**
	 *@brief ����֮·
	 */
	void GetLegendTaskList(std::vector<TaskBriefInfo>& tasks) { m_LegendTaskMgr.GetLegendTasks(tasks); }
	bool SubmitLegendTask(UInt32 taskId);

	/**
	 *@brief ѡȡ���ʵ��ճ�����
	 */
	//UInt32 SelectDailyTask(UInt8 type);

	void ResetTask();
	TaskMgr& GetTaskMgr(){ return m_TaskMgr; }
	DailyTaskMgr& GetDailyTaskMgr() { return m_DailyTaskMgr; }
	AchievementTaskMgr& GetAchievementTaskMgr() { return m_AchievementTaskMgr; }
	CycleTaskMgr& GetCycleTaskMgr() { return m_CycleTaskMgr; }
	RedPacketTaskMgr& GetRedPacketTaskMgr() { return m_RedPacketTaskMgr; }
	LegendTaskMgr& GetLegendTaskMgr() { return m_LegendTaskMgr; }
	MasterDailyTaskMgr& GetMasterDailyTaskMgr() { return m_MasterDailyTaskMgr; }
	MasterAcademicGrowthTaskMgr& GetMasterAcademicGrowthTaskMgr() { return m_MasterAcademicGrowthTaskMgr; }
	bool CheckTaskFinished(UInt32 id);
	void SetTaskFinished(UInt32 id);

	bool AcceptLegendTask(UInt32 taskId);
	bool CheckLegendTaskFinished(UInt32 taskId);

	bool CheckAchievementTaskFinished(UInt32 taskId);

	ActivityAttributeMgr& GetActivityAttributeMgr() { return m_ActivityAttributeMgr; }

	/**
	 *@brief ���ݵȼ���ǰ�������ж������Ƿ�ɽ�
	 */
	bool IsTaskAcceptable(UInt32 id, bool bNotifyErr = false);
	//bool IsDailyTaskAcceptable(UInt8 type, bool bNotifyErr);
	//UInt32 GetDailyTaskMaxNum(UInt8 type);
	//UInt32 GetDailyTaskBaseMaxNum(UInt8 type);

	/**
	*@brief �趨�õ��ճ���������
	*/
	UInt32 GetDailyTaskSelectorNum();

	/**
	 *@brief ��ȡ�ճ�������
	 */
	UInt32 GetDailyTaskNum(UInt8 type){ return m_DailyTaskMgr.GetTaskNum(); }

	/**
	*@brief ��ȡ�ճ�����ѡ����
	*/
	const std::vector<DailyTaskSelector>& GetDailyTaskSelector() { return m_pLevelDataEntry->dailyTaskPool; }

	/**
	 *@brief ���û�ȡ��������
	 */
	void SetZoneTask(UInt32 task){ m_ZoneTask = task; }
	UInt32 GetZoneTask() const{ return m_ZoneTask; }

	/**
	*@brief ��������
	*/
	void LoadTask(UInt64 guid, UInt32 dataId, TaskStatus status, std::string parameter, UInt32 finTime, UInt8 submitCount);

	/**
	*@brief �����������
	*/
	void OnLoadTaskFinished();

	/**
	*@brief ����id��ȡtask����
	*/
	BaseTask* GetTask(UInt32 taskId);

	/**
	*@brief �Ͻ�������Ʒ
	*/
	UInt32 SetTaskItem(UInt32 taskId, std::vector<UInt64> items);

	/**
	*@brief ������������б��С
	*/
	//UInt32 GetXuanShangListSize() { return m_TaskMgr.GetXuanShangListSize(); }

	UInt32 FinishAllTask(TaskType taskType = TaskType::TASK_MAX, UInt32 taskId = 0);

	/**
	*@brief ÿ���������
	*/
	void IncDailyTaskScore(UInt32 score);
	UInt32 GetDailyTaskScore() { return m_DailyTaskScore; }
	void ResetDailyTaskScore() { m_DailyTaskScore = 0; }

	/**
	*@brief ÿ��������ֽ���
	*/
	inline bool CheckDailyTaskRewardMask(UInt32 boxId) const { return m_DailyTaskRewardMask.CheckMask(boxId); }
	inline void SetDailyTaskRewardMask(UInt32 boxId){ m_DailyTaskRewardMask.SetMask(boxId); }
	inline void ResetDailyTaskRewardMask() { m_DailyTaskRewardMask.ClearMask(); }

	/**
	*@brief �ɾͻ��ֽ���
	*/
	inline bool CheckAchievementRewardMask(UInt32 rewardId) const { return m_AchievementTaskRewardMask.CheckMask(rewardId); }
	inline void SetAchievementRewardMask(UInt32 rewardId) { m_AchievementTaskRewardMask.SetMask(rewardId); }

	/**
	*@brief ��ȡÿ��������ֽ���
	*/
	UInt32 ReceiveDailyTaskScoreReward(UInt32 boxId);

	/**
	*@brief ��ȡ�ɾͻ��ֽ���
	*/
	UInt32 ReceiveAchievementScoreReward(UInt32 rewardId);

	/**
	*@brief �ɾͻ���
	*/
	void AddAchievementScore(UInt32 score);
	inline UInt32 GetAchievementScore() const { return m_AchievementScore; }

public://����
	/**
	 *@brief ��ʼһ������
	 */
	bool StartStory(UInt32 id);

	/**
	 *@brief �ͻ���֪ͨ����һ������
	 */
	void StopStory(UInt32 id);

	/**
	*@brief ����һ������
	*/
	void SetBootFlag(UInt32 id);

	/**
	*@brief ����һ������
	*/
	void ResetBootFlag(UInt32 id);

	/**
	 *@brief ��ֹ����
	 */
	void CancelStory();

	/**
	*@brief ������������
	*/
	void SetNewBoot(UInt32 id);
	UInt32 GetNewBoot();

	void CheckNewBoot();

	/**
	*@brief ������������״̬
	*/
	void ResetSeasonPlayStatus();

	/**
	*@brief ��ȡ��������״̬
	*/
	UInt8 GetSeasonPlayStatus() { return m_SeasonLevel.GetSeasonPlayStatus(); }

	//����ҩˮ����
	bool SetPotionPos(UInt8 pos, UInt32 potionId)	{ return m_potionSet.SetPotion(pos, potionId); }
	std::vector<UInt32> GetPotionPos()	{ return m_potionSet.GetPosionPos(); }

public://�������
	/**
	 *@brief �����Ӧ����
	 */
	bool InviteTrade(ObjID_t id);
	void AcceptTrade(ObjID_t id);

	/**
	 *@brief ���û�ȡ����
	 */
	Trade* GetTrade() const{ return m_pTrade; }
	void SetTrade(Trade* trade){ m_pTrade = trade; }

	/**
	 *@brief �˳�����
	 */
	void CancelTrade();

public: //�ʼ����
	/**
	 *@brief ����ϵͳ�ʼ�
	 */
	void SendSysMail(const char* senderName, const char* title, const char* content, const char* reason, RewardGroup* rewards = NULL);

public:
	/**
	 *@brief ���͹۲����ݣ�������
	 */
	void SendWatchData(ObjID_t watcher, const std::string& tribe);

public://��ϵ���

	/** 
	 *@brief �Ƿ����
	 */
	bool IsTeammate(Creature* creature) const;

	/**
	 *@brief ���ù�ϵ�б�
	 */
	void SetRelationList(RelationType type, std::map<ObjID_t, UInt16>& list);

	/**
	 *@brief ����ɾ����ϵ
	 */
	void AddRelation(RelationType type, ObjID_t tarId);
	void RemoveRelation(RelationType type, ObjID_t tarId);

	// ��ָ������Ƿ��й�ϵ
	bool HasRelation(RelationType type, ObjID_t tarId);
	// �Ƿ���ָ����ϵ�����
	bool HasRelation(RelationType type);
	// ���ʦ��
	Player* GetOnlineMaster();
	// ʦ����ʦ��
	bool IsMaster(ObjID_t roleId);
	ObjID_t GetMasterId();

	// ʦͽƣ�ͷ���������ƣ��ʱ���ã�
	void AddFatigueToMaster(UInt32 usedFatigue);

	// ʦͽ����
	UInt8 GetMasterDiscipleType();
	/**
	 *@brief �������ܶ�
	 */
	void OnSyncIntimacy(ObjID_t id, UInt16 intimacy, UInt16 incIntimacy);
	void AddIntimacy(ObjID_t id, UInt16 intimacy);
	UInt16 GetIntimacy(ObjID_t id) const;
	UInt16 GetTeamIntimacyBuff() const{ return m_TeamIntimacyBuff; }
	bool IsFriend(ObjID_t id) const{ return m_Friends.find(id) != m_Friends.end(); }

	/**
	 *@brief ͬ��ף�����Ѵ���                                                                     
	 */
	void SyncBlessTimes();

	/**
	 *@brief ף���������
	 */
	UInt32 GetBlessExp() const{ return m_BlessExp; }
	void DecBlessExp(UInt32 exp);
	void IncBlessExp(UInt32 exp);

	/**
	 *@brief ���ͺ���������Ϣ                                                                     
	 */
	void SendFriendPresentInfo();

	/**
	 *@brief ���Ѹ�����                                                                     
	 */
	bool FriendGiveGift(const std::string& friendName, UInt32 itemId);
	void OnGiveFriendGiftRet(UInt32 gamestartDay, UInt8 type, UInt8 result);

public://�������
	/**
	 *@brief ����
	 */
	void Speak(UInt8 channel, const std::string& word, const std::string voiceKey = "", UInt8 voiceDur = 0, ObjID_t targetId = 0, UInt8 isShare = 0);
	UInt32 ParseSpeakWords(const std::string inWord, std::string& outWords, std::set<Item*>& linkItems, std::set<ObjID_t>& linkAuctionItems,
		bool& isLink, UInt32 maxShowWidth = CL_MAX_SHOW_WIDTH);
	void ScriptSpeak(UInt8 channel, const char* word);

	/**
	*@brief ��������
	*/
	UInt32 SendHorn(const std::string& content, UInt8 num);

	/**
	*@brief �ϱ��Զ�����־
	*/
	void ReportCustomLog(CustomLogReportType type, const std::string& param);

	/**
	 *@brief ��������ʱ��
	 */
	void SetChatTime(UInt8 channel, UInt64 time){ m_ChatTimer.SetChatTime(channel, time); }

	/**
	 *@brief �������λ��
	 */
	void SendPos(UInt8 channel, ObjID_t targetId);
	void SendNotifyById(UInt32 id);

	/**
	 *@brief ����֪ͨ
	 */
	// added by huchenhui
	template <typename... Args>
	void SendNotify(UInt32 id, const Args & ... args)
	{
		UInt16 type = 0;
		UInt8 color = 0;
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(id, type, color, buffer, CL_MAX_CHAT_LEN, args...);

		if (!ret) return;

		Notify(type, color, buffer);
	}

	template <typename... Args>
	void SendNotify(const char* ctx, const Args & ... args)
	{
		char buffer[CL_MAX_CHAT_LEN + 1];
		memset(buffer, 0, sizeof(buffer));
		bool ret = SysNotifyMgr::Instance()->MakeNotify(ctx, buffer, CL_MAX_CHAT_LEN, args...);
		if (!ret) return;
		Notify(20, 0, buffer);
	}

	void Notify(UInt16 type, UInt8 color, const char* word);

public://�������
	/**
	 *@brief ��������
	 */
	void CreateTeam();

	/**
	 *@brief ���û�ȡ����
	 */
	void SetTeam(Team* team);
	Team* GetTeam() const{ return m_pTeam; }
	void UpdateTeamInfo();

	/**
	 *@brief �ж��Ƿ�ӳ�
	 */
	bool IsTeamMaster() const;

	/**
	*@brief ��ȡ��������
	*/
	UInt8 GetTeamSize() const;

public://�������

	PvpSkillMgr& GetPvpSkillMgr() { return m_PvpSkillMgr; }

	/**
	 *@brief ��ȡ������
	 */
	SkillBar& GetSkillBar(){ return m_SkillBar; }

	/**
	*@brief ��ȡPVP������
	*/
	SkillBar& GetPvpSkillBar() { return m_PvpSkillBar; }

	/**
	*@brief ��ȡԤ��תְ����
	*/
	std::vector<UInt16> GetPreOccuSkillId();

	/**
	*@brief ���Ԥתְ����
	*/
	void ClearPreOccuSkills() { GetSkillMgr().ClearPreOccuSkills(); GetPvpSkillMgr().ClearPreOccuSkills(); }

	/**
	*@brief ��ȡ������skillbar�е�λ��
	*/
	UInt8 GetSkillSlot(UInt16 skillId) { return m_SkillBar.GetSlot(skillId); }

	/**
	*@brief ��ȡPVP������skillbar�е�λ��
	*/
	UInt8 GetPvpSkillSlot(UInt16 skillId) { return m_PvpSkillBar.GetSlot(skillId); }

	/**
	*@brief ��ȡ��Ʒ��
	*/
	ItemBar& GetItemBar() { return m_ItemBar; }

	/**
	*@brief ������Ʒ��
	*/
	void SetItemBar(UInt8 slot, UInt32 id);

	/**
	*@brief ��ȡ������λ�� huchenhui
	*/
	UInt32 GetSkillGridSize() { return m_pLevelDataEntry->skillGridSize; }

	/**
	*@brief ��ȡ��Ʒ��λ�� huchenhui
	*/
	UInt32 GetItemGridSize() { return m_pLevelDataEntry->itemGridSize; }

	/**
	 *@brief ����ѡ��
	 */
	void SetOptions(UInt32 options){ m_Options = options; }

	/**
	*@brief ��ȡս��������Ϣ
	*/
	void GetRaceSkills(std::vector<RaceSkillInfo>& raceSkills, bool isPvp);

	/**
	*@brief ��world������Ʒ����
	*/
	void WorldCacheItemData(Item* item);

	/**
	*@brief ��world�����������
	*/
	void WorldCacheRetinueData(Retinue* retinue);

	/**
	*@brief �Ƿ����SP
	*/
	bool IsCheckSP() { return m_IsCheckSp == 1; }

	/**
	*@brief ���SP���
	*/
	void FinishCheckSP() { m_IsCheckSp = 1; }

	/**
	*@brief װ�����Ƿ���Ч
	*/
	bool IsWeaponInvalid() { return m_WeaponBar.IsInvalid(); }

	/**
	*@brief װ�����Ƿ���ڸ�װ��
	*/
	bool IsWeaponExist(ObjID_t id) { return m_WeaponBar.IsExist(id); }

	/**
	*@brief ����װ����
	*/
	UInt32 SetWeaponBarById(ObjID_t oldId, ObjID_t newId) { return m_WeaponBar.SetBarById(oldId, newId); }

	/**
	*@brief ����װ����
	*/
	UInt32 SetWeaponBarByIndex(UInt32 index, ObjID_t newId);

public:	//�������
	/**
	 *@brief ���ӻ�ȡ����
	 */
	UInt32 IncCounter(const char* name, UInt32 value){ return m_CounterMgr.IncCounter(name, value); }
	UInt32 GetCounter(const char* name) { return m_CounterMgr.GetCounter(name); }
	UInt32 GetCounterWithoutCycle(const char* name) { return m_CounterMgr.GetCounterWithoutCycle(name); }
	void SetCounter(const char* name, UInt32 value){ m_CounterMgr.SetCounter(name, value); }

	/**
	 *@brief ��ȡ����������
	 */
	CounterMgr& GetCounterMgr(){ return m_CounterMgr; }

	/**
	 *@brief ��ȡ��Ϊ����������                                                                     
	 */
	UInt32 IncActionCounter(int type) { return m_ActionCounterMgr.IncCounter(type); }
	ActionCounterMgr& GetActionCounterMgr() { return m_ActionCounterMgr; }

	/**
	 *@brief ��ȡ�������볡����λ��                                                                     
	 */
	UInt32 GetEnterSceneID() const{ return m_EnterScene; }
	const CLPosition& GetEnterPos() const{ return m_EnterPos; }
	UInt64 GetEnterDyncSceneID() const { return m_EnterSceneDyncId; }
	/**
	*@brief ��ȡ����������ǰ�ĳ�����λ��
	*/
	void SetWudaoPrevSceneID(UInt32 id) { m_WudaoPrevSceneId = id; }
	UInt32 GetWudaoPrevSceneID() const{ return m_WudaoPrevSceneId; }
	const CLPosition& GetWudaoPrevPos() const{ return m_WudaoPrevScenePos; }

	/**
	*@brief �������������ID
	*/
	void SetCrossSceneNodeID(UInt32 id) { m_CrossSceneNodeId = id; }
	UInt32 GetCrossSceneNodeID() const { return m_CrossSceneNodeId; }

	/**
	*@brief ԭ����������ID
	*/
	void SetSrcSceneNodeID(UInt32 id) { m_SrcSceneNodeId = id; }
	UInt32 GetSrcSceneNodeID() const { return m_SrcSceneNodeId; }

	/**
	*@brief ս������������ID
	*/
	void SetBattleSceneNodeID(UInt32 id) { m_BattleSceneNodeId = id; }
	UInt32 GetBattleSceneNodeID() const { return m_BattleSceneNodeId; }

	/**
	*@brief ԭ����������ID
	*/
	void SetNotifyChangeScene(bool notify) { m_NotifyChangeScene = notify; }
	bool IsNotifyChangeScene() const { return m_NotifyChangeScene; }

public: //�ճ����
	/**
	 *@brief ��ȡ�ճ�������
	 */
	RoutineMgr& GetRoutineMgr() { return m_RoutineMgr; }

	/**
	 *@brief ��Ծ���¼����
	 */
	void	OnActiveEventFinished(UInt32 eventId){ GetRoutineMgr().OnActiveEventFinished(eventId); }

	/**
	*@brief ��ȡÿ������ʱ��
	*/
	UInt32 GetDailyOnlineTime() const { return m_RoutineMgr.GetDailyOnlineTime(); }

public: //����������

	InviteMgr& GetInviteMgr() { return m_InviteMgr; }

public:  //adder by huchenhui 2016.07.24

	//��ȡ��ӹ�����
	RetinueMgr& GetRetinueMgr() { return m_RetinueMgr; }

	//���һ�����
	UInt32 AddRetinue(UInt32 dataId) { return m_RetinueMgr.AddRetinue(dataId); }

	//����ItemID���һ�����
	UInt32 AddRetinueByItemId(UInt32 itemId);

	//����ItemID���������
	UInt32 CheckAddRetinueByItemId(UInt32 itemId);

	//��ȡ��Ӹ���
	UInt32 GetRetinueSize() { return m_RetinueMgr.GetRetinueSize(); }

	//��ȡָ��Ʒ�����ϵ�����
	UInt32 GetRetinueSizeByQuality(RetinueQuality quality) { return m_RetinueMgr.GetRetinueSizeByQuality(quality); }

	//��ȡ��ʷϴ������
	UInt32 GetChangeSkillTotal() { return m_RetinueMgr.GetChangeSkillTotal() + 1; }

	//��ȡ���ܸ���
	UInt32 GetSkillCount(UInt32 id, UInt8 level) { return m_RetinueMgr.GetSkillCount(id, level); }

	//���ݼ���ID��ȡ���ܸ���
	UInt32 GetSkillCountById(UInt32 id) { return m_RetinueMgr.GetSkillCount(id, 0); }

	//���ݼ���Level
	UInt32 GetSkillCountByLevel(UInt8 level) { return m_RetinueMgr.GetSkillCount(0, level); }

	//��ȡ��Ӳ�λ����
	UInt32 GetMaxRetinueNum() { return m_pLevelDataEntry->retinueSize; }

	//��ȡ�����ӵȼ�
	UInt32 GetMaxRetinueLevel() { return m_RetinueMgr.GetMaxRetinueLevel(); }

	//��ȡ����Ǽ�
	UInt32 GetMaxRetinueStar() { return m_RetinueMgr.GetMaxRetinueStar(); }

public:

	//��ȡ���������
	PetMgr& GetPetMgr() { return m_PetMgr; }

	/**
	*@brief ���ӳ���
	*/
	UInt32 AddPet(UInt32 itemId) { return m_PetMgr.AddPet(itemId); }

	/**
	*@brief ���ó����λ
	*/
	UInt32 SetPetSlot(PetType petType, UInt64 petId) { return m_PetMgr.SetPetSlot(petType, petId); }

	/**
	*@brief ��ȡ�����λ�б�
	*/
	std::vector<UInt64> GetBattlePets() { return m_PetMgr.GetBattlePets(); }

	/**
	*@brief ιʳ����
	*/
	UInt32 FeedPet(ObjID_t id, PetFeedType petFeedType, UInt8& isCritical, UInt32& value) { return m_PetMgr.FeedPet(id, petFeedType, isCritical, value); }

	/**
	*@brief ���۳���
	*/
	UInt32 SellPet(ObjID_t id) { return m_PetMgr.SellPet(id); }

	/**
	*@brief ������＼��
	*/
	UInt32 ChangePetSkill(ObjID_t id, UInt8 skillIndex){ return m_PetMgr.ChangePetSkill(id, skillIndex); }

	/**
	*@brief ���ø���ĳ���
	*/
	UInt32 SetPetFollow(ObjID_t id) { return m_PetMgr.SetPetFollow(id); }

	/**
	*@brief ��ȡ����ĳ���
	*/
	UInt64 GetPetFollow() { return m_PetMgr.GetFollowPetId(); }

	/**
	*@brief ��ʳ����
	*/
	UInt32 DevourPet(ObjID_t id, std::vector<ObjID_t> petIds, UInt32& exp) { return m_PetMgr.DevourPet(id, petIds, exp); }

	/**
	*@brief ��ȡ���������Ϣ
	*/
	void GetPetBaseInfos(std::vector<PetBaseInfo>& pets) { return m_PetMgr.GetPetBaseInfos(pets); }

	/**
	*@brief �۳����Ｂ��ֵ
	*/
	void RemovePetHunger(UInt16 hunger) { return m_PetMgr.RemovePetHunger(hunger); }

	/**
	*@brief ���ӳ��Ｂ��ֵ
	*/
	void AddPetHunger(UInt16 hunger) { return m_PetMgr.AddPetHunger(hunger); }

	/**
	*@brief ��ȡ��������
	*/
	UInt32 GetPetSize(PetType petType, PetQuality petQualiy, UInt16 level) { return m_PetMgr.GetPetSize(petType, petQualiy, level); }

public:	//������λ
	//��ȡ������λ
	UInt32 GetSeasonLevel() { return m_SeasonLevel.GetSeasonLevel(); }

	//��ȡ�����Ǽ�
	UInt32 GetSeasonStar() { return m_SeasonLevel.GetSeasonStar(); }

	//��ȡ��������
	UInt32 GetSeasonExp() { return m_SeasonLevel.GetSeasonExp(); }

	//��ȡ��������
	UInt8 GetSeasonAttr() { return m_SeasonLevel.GetSeasonAttr(); }

	//��ȡ����ӡ��
	UInt32 GetSeasonKingMarkCount() { return m_SeasonLevel.GetSeasonKingMarkCount(); }

	//��ȡ������������¼
	std::vector<UInt8> GetSeasonUplevelBattle() { return m_SeasonLevel.GetSeasonUplevelBattle().GetBattleRecords(); }

	//������Ϣ���
	void UpdateSeasonInfo() { return m_SeasonLevel.UpdateSeasonInfo(); }

	//����������λ������
	bool SetSeasonLevel (UInt32 level, UInt32 star = 0) { return m_SeasonLevel.SetSeasonLevel(level, star); }

	//���ý�����ս��
	bool SetSeasonUplevelBattleRecord(std::vector<UInt8> vecRecords) { return m_SeasonLevel.SetSeasonUplevelBattleRecord(vecRecords); }

	//������������
	void SetSeasonAttr(UInt32 time, UInt8 attr = 0) { m_SeasonLevel.SetSeasonAttr(time, attr); }

	//��ȡ��������ʱ��
	UInt32 GetChangeSeasonAttrTime() { return m_SeasonLevel.GetChangeSeasonAttrTime(); }

	//������������
	void AddSeasonExp(UInt32 exp) { m_SeasonLevel.IncExp(exp); }

	//�۳���������
	void DecSeasonExp(UInt32 exp) { m_SeasonLevel.DecExp(exp); }

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
		CUserLogger	*pLogger = _analyzer.GetInstance(UDPLOG_SCENE_UID);
		if (NULL == pLogger) return;

		char userLogBuffer[MAX_USERMSG_LENGTH];
		memset(userLogBuffer, 0, MAX_USERMSG_LENGTH);
		UInt32 nTime = (UInt32)Avalon::Time::CurrentTime().Sec();
		UInt16 zoneId = (GetSrcZoneId() != 0 ? GetSrcZoneId() : ZONE_ID);
		UInt32 monthCardExpireTime = m_MonthCardExpireTime;
		UInt32 bronZoneId = GetBornZoneId();
		UInt32 isReturn = IsVeteranReturn() ? 1 : 0;

#ifndef AVALON_OS_WINDOWS
		snprintf(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(),
			m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, 0, 0);
#else
		_snprintf_s(userLogBuffer, sizeof(userLogBuffer), LOG_USERMSG_FORMAT, GAME_VERSION.c_str(), m_OpenId.c_str(), zoneId, m_Pf.c_str(), m_DeviceId.c_str(),
			m_Model.c_str(), m_DeviceVersion.c_str(), m_Ip.c_str(), GetID(), GetAccID(), (UInt32)GetLevel(), (UInt32)GetVipLvl(), (UInt32)GetOccu(), monthCardExpireTime, UInt32(m_CreateTime), nTime, bronZoneId, isReturn, 0, 0, 0);
#endif

		pLogger->SetUserIP(m_Ip.c_str());
		pLogger->SetUserMsg(userLogBuffer);
		pLogger->LogMsg(typeInfo, format, args...);

	}

	/**
	*@brief ���͵�¼log
	*/
	void SendLoginUdpLog();

	/**
	*@brief ���͵ǳ�log
	*/
	void SendLogoutUdpLog();

	/**
	*@brief ���͵��߿��log
	*/
	void SendItemUdpLog();

	/**
	*@brief ���͵������log
	*/
	void SendItemUdpLog(const char* reason, UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, std::string itemName, UInt32 itemQuality, Int32 number, UInt32 oldNumber);

	/**
	*@brief ����װ����½
	*/
	void SendEquipLoginUdpLog(UInt64 itemId, UInt32 itemDataId, ItemType itemType, ItemSubType itemSubType, ItemQuality quality, UInt32 strengthen, UInt32 qualityLevel, std::string itemName
		, UInt32 preciousId, ItemQuality preciousQuality, UInt32 cardId, ItemQuality cardQuality, UInt32 slot);

	/**
	*@brief ���͵��ߵ�½
	*/
	void SendItemLoginUdpLog(UInt64 itemId, UInt32 itemDataId, UInt32 itemNum, ItemType itemType, ItemSubType itemSubType, ItemQuality quality, UInt32 strengthen, UInt32 qualityLevel, std::string itemName
		, UInt32 preciousId, ItemQuality preciousQuality, UInt32 cardId, ItemQuality cardQuality, UInt32 slot);

	/**
	*@brief ���Ϳ��������log
	*/
	void SendOpenJarUdpLog(UInt32 jarId, UInt32 combo, ItemDataEntry* itemData, UInt32 itemNum);

	/**
	*@brief �����ʲ����log  added by huchenhui 2016.08.11
	*/
	void SendAssetUdpLog(AssetType type, const char* reason, UInt64 oldAsset, Int64 assetNum, Int64 nowAssetNum, UInt64 nowAsset);

	/**
	*@brief �����̳ǵ��߱仯log
	*/
	void SendItemChangeUdpLog(const char *strType, UInt32 uItemId, UInt32 uPrice, UInt16 uNum);

	/**
	*@brief �ϱ�������־�����ʹ�ø��ֹ���֮���ϱ�
	*/
	void SendFunctionUdpLog(FunctionLogType type, UInt32 p1 = 0, UInt32 p2 = 0);

	/**
	*@brief ���ܱ仯UDP��־
	*/
	void SendChangeSkillUdpLog(UInt16 skillId, std::string skillName, Int8 changeLevel, UInt8 sourceLevel, UInt8 newLevel);

	/**
	*@brief ��ӽ���UDP��־
	*/
	void SendRetinueUnlockUdpLog(UInt64 retinueId, UInt32 dataId);

	/**
	*@brief �������UDP��־
	*/
	void SendRetinueUpLevelUdpLog(UInt64 retinueId, UInt32 dataId, UInt8 level);

	/**
	*@brief �������UDP��־
	*/
	void SendRetinueUpStarUdpLog(UInt64 retinueId, UInt32 dataId, UInt8 starLevel);

	/**
	*@brief ���ϴ��UDP��־
	*/
	void SendRetinueChangeSkillUdpLog(UInt64 retinueId, UInt32 dataId, std::string lockSkills, std::string oldSkills, std::string newSkills);

	/**
	*@brief ����������־
	*/
	void SendActiveTaskFinishUdpLog(UInt32 activeId, UInt32 taskId);

	/**
	*@brief �̵깺����־
	*/
	void SendShopBuyUdpLog(UInt32 shopId, UInt32 shopItemId, UInt32 num, UInt32 newNum, UInt32 dungeonId);

	/**
	*@brief �̵���Ʒ��־
	*/
	void SendShopItemUdpLog(UInt32 shopId, UInt32 shopItemId, UInt32 costNum);

	/**
	*@brief �̳ǹ�����־
	*/
	void SendMallBuyUdpLog(UInt32 mallItemId, UInt32 num);

	/**
	*@brief ����������־
	*/
	void SendNewBootUdpLog(UInt32 bootId);

	/**
	*@brief ǿ��װ����־
	*/
	void SendStrengthEquipUdpLog(UInt64 itemId, UInt32 itemDataId, UInt32 oldStrength, UInt32 newStrength, UInt32 errorcode, UInt32 packType, UInt32 useUnbreak, UInt32 isTicket, UInt32 probability);

	/**
	*@brief �״ι�������֮��
	*/
	void SendFirstClearDeathTowerUdpLog(UInt32 dungeonId, UInt32 floor, UInt32 usedSec);

	/**
	*@brief ƥ�俪ʼ��־
	*/
	void SendMatchStartUdpLog(PkType type);

	/**
	*@brief ��������
	*/
	void SendPvpEndUdpLog(UInt32 targetZoneId, UInt64 targetId, PkType type, PkRaceResult result, UInt32 oldMatchScore, UInt32 newMatchScore, UInt32 oldPkValue, UInt32 newPkValue, UInt32 oldSeasonLevel, UInt32 newSeasonLevel, UInt32 oldSeasonStar, UInt32 newSeasonStar, UInt32 oldSeasonExp, UInt32 newSeasonExp, UInt32 seasonId, UInt32 dungeonId, UInt32 usedTime);

	/**
	*@brief ��������־
	*/
	void SendWelfareCardUdpLog(ItemSubType type);

	/**
	*@brief ��ħ���ϳ�
	*/
	void SendComposeMagicCardUdpLog(UInt32 card1, UInt32 card2, UInt32 targetCard, UInt32 code);

	/**
	*@brief ʱװ�ϳ�
	*/
	void SendFashionComposeUdpLog(UInt32 fashion1, UInt32 fashion2, UInt32 commonFashionId, UInt32 skyFashionId);

	/**
	*@brief װ��Ʒ�׵���
	*/
	void SendReMakeEquipQlvUdpLog(UInt64 itemId, UInt32 itemDataId, UInt8 oldLv, UInt8 nowLv, UInt32 oldMatNum, UInt32 newMatNum, UInt32 oldPoint, UInt32 newPoint, UInt8 isFrenzy, UInt8 eqColor, UInt8 eqPost);

	/**
	*@brief �������
	*/
	void SendPayEndUdpLog(std::string orderId, UInt8 mallType, UInt32 chargeGoodsId, UInt32 chargeItemId, UInt32 chargeItemNum, UInt32 vipScore, UInt32 chargeMoney, UInt32 time, UInt32 sendGoodTimeoutTime);

	/**
	*@brief תְ
	*/
	void SendChangeOccuUdpLog(UInt8 mainOccu, UInt8 preOccu, UInt8 occu);

	/**
	*@brief �����½
	*/
	void SendPetLoginUdpLog(UInt64 petId, UInt32 dataId, UInt32 level, UInt32 exp, PetType petType, UInt32 slot);

	/**
	*@brief ���ӳ���
	*/
	void SendPetAddUdpLog(UInt64 petId, UInt32 dataId, UInt32 itemId, PetType petType, PetQuality petQuality);

	/**
	*@brief �������
	*/
	void SendPetBattleUdpLog(UInt64 petId, UInt32 dataId, UInt64 oldPetId, UInt32 oldDataId);

	/**
	*@brief ���ɳ���
	*/
	void SendPetDevourUdpLog(UInt64 petId, UInt32 dataId, UInt16 oldLevel, UInt16 newLevel, UInt32 oldExp, UInt32 newExp, UInt32 exp, UInt64 devourId, UInt32 devourDataId, UInt16 devourLevel, UInt32 devourExp);

	/**
	*@brief ����������ѡ
	*/
	void SendPetChangeSkillUdpLog(UInt64 petId, UInt32 dataId, UInt8 oldIndex, UInt8 newIndex);

	/**
	*@brief �ϱ�ʵʱ������־
	*/
	void SendLiveVoiceDataUdpLog(UInt8 type, const std::string& roomId, const std::string& time);

	/**
	*@brief �ϱ�����������־
	*/
	void SendChatVoiceDataUdpLog(UInt8 type, const std::string& voiceId);

	/*
	*@brief ����ͽ�����ʦ������log
	*/
	void SendDiscipleCompDailyTaskLog(ObjID_t discipleId, ObjID_t masterId, UInt32 timestamp);

	/*
	*@brief ���ͱ�����־
	*/
	void SendPreciousBeadUdpLog(PreciousBeadOpType opType, ObjID_t itemObjectId, UInt32 itemId, UInt32 srcBeadId, UInt32 newBeadId, UInt32 costItemId, UInt32 costItemNum, UInt32 isSuccess, UInt32 param1, UInt32 param2);

	/*
	*@brief ���ͳ����������ۿ���־
	*/
	void SendExtractArtifactJarDiscountUdpLog(UInt32 opActId, UInt32 rate, UInt32 times);

	/*
	*@brief ���ͳԼ�������־
	*/
	void SendChiJiBalanceUdpLog(UInt32 rank, UInt32 survivalTime, UInt32 reason);

	/**
	*@brief ����������
	*/
	void CountAvatar();
	PlayerAvatar GetAvatar() const { return m_avatar; }
	void SetAvatar(const PlayerAvatar& avatar) { m_avatar = avatar; }

	// ����ʱ�޵����Ƿ�֪ͨ�ͻ���
	inline bool IsFirstTimeItemTick()	{ return m_bFirstTimeItemTick; }
	inline void SetFirstTimeItemTick(bool b)	{ m_bFirstTimeItemTick = b; }

public://���ݿ����

    /**
	 *@brief ��ʼ�����������(�������ָ��һ���ص�)
	 */
	void StartLoadData(PlayerLoadingCallback* cb);

	/**
	*@brief ��ʼ�����˺���Ϣ
	*/
	void LoadAccountInfo(PlayerLoadingCallback* cb);

	/**
	*@brief ��ʼ�����˺Ųֿ���Ϣ
	*/
	void LoadAccountStorage();

    /**
	 *@brief ��Ӽ������ݻص�
	 */
	void AddLoadingCallback(PlayerLoadingCallback* cb);

	/**
	 *@brief ��������
	 */
	void Saveback(CLRecordCallback* callback);

	/**
	*@brief ������������
	*/
	void SaveOnlineData();

	/**
	 *@brief ���Ӽ�������������ر��μ������
	 */
	UInt8 IncLoadDataCount();

	/**
	 *@brief ��������
	 */
	bool CheckLoadDataSerial(UInt8 serial);
	void SetLoadDataFinished(UInt8 serial);

    /**
    *@brief �����������
    */
    void LoadingFinish();

	/**
	*@brief �����������
	*/
	void LoadingFailed();

public://���ܿ������
	/**
	 *��顢���ù��ܿ�����־λ
	 */
	bool CheckFuncFlag(UInt32 id) const{ return m_FuncFlag.CheckMask(id); }
	void SetFuncFlag(UInt32 id){ m_FuncFlag.SetMask(id); }

	/**
	 *@brief �ж�Ԥ�湦��                                                                     
	 */
	void UpdateFuncNotify();

	void OpenFunction(UInt32 id);

public://�¼�
	/**
	*@brief ����
	*/
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	void OnHeartbeat(const Avalon::Time& now);

	/**
	*@brief ����ע������¼�
	*/
	void OnRegistGameEvent();

	/**
	*@brief ����ע������¼�
	*/
	void OnUnRegistGameEvent();

	/**
	 *@brief ����
	 */
	void OnOnline();
	void OnOnlineBattle();

	/**
	 *@brief ����
	 *@param bShutdown �Ƿ�ά��
	 */
	void OnOffline(bool bShutdown = false);

	/**
	 *@brief ����ű�����
	 */
	void ScriptOnOnline(UInt64 offlineTime);

	/** 
	 *@brief ����
	 */
	void OnBirth();

	/**
	 *@brief �������볡���¼�
	 */
	void OnBeforeEnterScene();

	/**
	 *@brief ������Ϸ
	 */
	void OnEnterScene();

	/**
	 *@brief �뿪��Ϸ
	 */
	void OnLeaveScene();

	/**
	*@brief ˢ��ƣ��ֵ
	*/
	void OnRefreshFatigue();

	/**
	 *@brief ���ӷ����仯
	 */
	void OnGridChanged();

	/**
	 *@brief ����
	 */
	void OnLevelup();

	/**
	 *@brief ʹ�õ���
	 */
	void OnUseItem(UInt32 itemid);

	/**
	 *@brief ��õ���
	 */
	void OnGetItem(const char* reason, ItemDataEntry* data, UInt8 qualityLv, UInt8 strenth, UInt16 num,
		UInt16 oldNum, Item* pItem = NULL, bool getNotify = true, bool mailGive = false);
	void OnGetRewardGroup(const char* reason, RewardGroup* group, bool mailGive = false);
	/**
	 *@brief ���ĵ���
	 */
	void OnCostItem(const char* reason, ItemDataEntry* data, UInt8 strengthen, UInt16 newNum, UInt16 oldNum, Item* pItem = NULL);

	/**
	*@brief �ֿ����
	*/
	void OnPushStorage(UInt32 itemId, UInt8 quality, UInt8 strengthen, UInt16 num);
	void OnPullStorage(UInt32 itemId, UInt8 quality, UInt8 strengthen, UInt16 num);

	/**
	 *@brief ���й���Ȩ��npc����
	 */
	void OnOwnedNpcDied(Npc* npc, bool bDeathOwner = true);

	/*
	*@ ���������¼��ص�  added by aprilliu, 2016.04.13
	*/
	void OnNpcDead(UInt32 npcId, UInt32 dungeon);

	/**
	 *@brief �ƶ��¼�
	 */
	void OnMoved(const CLPosition& oldpos);

	/**
	 *@brief �����л�
	 */
	void OnSceneChanged();

	/**
	 *@brief ����ڶ���
	 */
	void OnDayChanged();

	/**
	*@brief �����߼��ڶ���(6��)
	*/
	void OnLogicDayChanged();

	/**
	 *@brief ������һ�߼���
	 */
	void OnLogicWeekChanged();

	/**
	 *@brief �����������
	 */
	void OnDataLoaded();

	/**
	*@brief ��ʼ����
	*/
	void OnBeginDungeon(UInt32 dungeonId, bool isHell);

	/**
	*@brief ����ͨ��
	*/
	void OnClearDungeon(UInt32 dungeonId, UInt16 beated, UInt16 areas, LevelScore score, UInt32 usedTime, UInt32 reviveCount, UInt32 endAreaId, std::vector<ObjID_t> teammateIds);

	/**
	 *@brief ����
	 */
	void OnConsumed(UInt32 itemId, UInt16 num, UInt32 cost);

	/**
	 *@brief ���ָı�
	 */
	void OnNameChanged();

	/**
	 *@brief vip�ȼ��ı�
	 */
	void OnVipLvlChanged();

	/**
	*@brief vip����ı�
	*/
	void OnVipExpChanged();

	/**
	 *@brief �Ա�ı�
	 */
	void OnSexChanged();

	/**
	 *@brief ����ɹ�
	 */
	void OnTaskSucced(BaseTask* task);

	/**
	 *@brief ����ʧ��
	 */
	void OnTaskFailed(UInt32 taskId);

	/**
	*@brief ǿ��װ�� Adder by huchenhui 2016.06.24
	*/
	void OnStrengthenEquip(UInt32 code, UInt8 oldStrengthen, UInt64 itemId, UInt32 itemDataId, UInt32 oldPack, UInt32 useUnbreak, UInt32 isTicket, UInt32 probability);

	/**
	*@brief �ֽ�װ�� Adder by huchenhui 2016.06.24
	*/
	void OnDecomposeEquip();

	/**
	*@brief ��Ϸ�ұ仯  numΪ������ʾ����, ������ʾ����   added by huchenhui 2016.06.28
	*/
	void OnChangeMoney(UInt32 type, Int32 num);

	/**
	*@brief ����װ�� Adder by huchenhui 2016.06.29
	*/
	void OnPutEquip(UInt16 level, UInt32 subType, UInt8 color, UInt32 strengthen);

	/**
	*@brief ����ʱװ Adder by huchenhui 2018.03.20
	*/
	void OnPutFashion(ItemSubType subType, UInt8 color);

	/**
	*@brief ������ Adder by huchenhui 2016.08.04
	*/
	void OnAddRetinue(Retinue* retinue);

	/**
	*@brief ������� Adder by huchenhui 2016.10.19
	*/
	void OnRetinueUpLevel(Retinue* retinue);

	/**
	*@brief ������� Adder by huchenhui 2016.11.28
	*/
	void OnRetinueUpStar(Retinue* retinue);

	/**
	*@brief ���ϴ�� Adder by huchenhui 2016.08.20
	*/
	void OnRetinueChangeSkill(Retinue* retinue, std::vector<RetinueSkill> lockSkills, std::vector<RetinueSkill> oldSkills, std::vector<RetinueSkill> newSkills);

	/**
	*@brief pvp�¼� Adder by huchenhui 2016.08.05
	*/
	void OnPVP(const CLProtocol::MatchServerRaceEnd& end);

	/**
	*@brief ����pvp�¼� Adder by huchenhui 2017.12.25
	*/
	void OnRoomPvp(const CLProtocol::MatchServerPkRoomRaceEnd& raceEnd, const Pk3V3PlayerRaceEndInfo& playerInfo);

	/**
	*@brief תְ�¼� Adder by huchenhui 2016.08.20
	*/
	void OnChangeOccu(JobDataEntry* targetJob);

	/**
	*@brief ��ħ�¼� Adder by huchenhui 2016.08.20
	*/
	void OnAddMagic(ItemDataEntry* data, Item* equip);

	/**
	*@brief �ϳɸ�ħ���¼� Adder by huchenhui 2016.09.13
	*/
	void OnAddMagicCard();

	/**
	*@brief �������¼� Adder by huchenhui 2016.08.20
	*/
	void OnOpenMagicJar(JarType type, UInt32 combo);

	/**
	*@brief ���������¼� Adder by huchenhui 2016.08.22
	*/
	void OnFriendGive();

	/**
	*@brief ˢ���̵��¼� Adder by huchenhui 2016.08.23
	*/
	void OnRefreshShop();

	/**
	*@brief ������Ʒ�¼� Adder by huchenhui 2016.11.23
	*/
	void OnShopBuy(UInt32 shopId, UInt32 shopItemId, UInt32 num, UInt32 newNum, UInt32 dungeonId);

	/**
	*@brief �̳ǹ����¼� Adder by huchenhui 2016.11.23
	*/
	void OnBuyMallItem(UInt32 mallItemId, UInt32 num);

	/**
	*@brief ����ƣ���¼� Adder by huchenhui 2016.08.29
	*/
	void OnRemoveFatigue(UInt16 usedFatigue);

	/**
	*@brief ͨ����Ԩ Adder by huchenhui 2016.08.30
	*/
	void OnClearHell(UInt32 dungeonId, UInt32 usedTimec, LevelScore score, std::vector<ObjID_t> teammateIds);

	/**
	*@brief ����֮����ʼ Adder by huchenhui 2016.10.22
	*/
	void OnDeathTowerBegin(UInt32 towerId);

	/**
	*@brief ����֮�� Adder by huchenhui 2016.08.30
	*/
	void OnDeathTower(UInt32 towerId, UInt32 layer, UInt32 usedTime);

	/**
	*@brief ���� Adder by huchenhui 2016.08.31
	*/
	void OnAwaken();

	/**
	*@brief ����� Adder by huchenhui 2016.09.06
	*/
	void OnReviveCoin(bool isSelf);

	/**
	*@brief �������ϼ� Adder by huchenhui 2016.09.14
	*/
	void OnAddAuction();

	/**
	*@brief ������ �۳� Adder by huchenhui 2016.09.14
	*/
	void OnSellAuction(UInt32 money);

	/**
	*@brief ������ �۳� Adder by huchenhui 2018.03.17
	*/
	void OnBuyAuction(UInt32 money);

	/**
	*@brief ���������¼�
	*/
	void OnAcceptTask(UInt32 taskId, TaskType type);

	/**
	*@brief ���������¼�
	*/
	void OnAbandonTask(UInt32 taskId, TaskType type);

	/**
	*@brief ����ճ����� Adder by huchenhui 2016.09.14
	*/
	void OnDailyTaskSubmitted(UInt32 taskId, TaskType type, TaskSubType subType);

	/**
	*@brief ������� Adder by huchenhui 2016.09.14
	*/
	void OnSubmitTaskSucceed(UInt32 taskId, TaskType type);

	/**
	*@brief ��ɻ���� Adder by huchenhui 2016.10.22
	*/
	void OnSubmitActiveTaskSucceed(UInt32 templateId, UInt32 taskId);

	/**
	*@brief ���ѭ������ Adder by huchenhui 2016.11.07
	*/
	void OnSubmitCycleTaskSucceed(UInt32 taskId);

	/**
	*@brief ��ɴ���֮·���� Adder by huchenhui 2017.08.23
	*/
	void OnSubmitLegendTaskSucceed(BaseTask* task);

	/**
	*@brief ������Ʒ�¼� Adder by huchenhui 2016.11.14
	*/
	void OnMakeItem(const char* reason, Item* item, Int32 num, bool isSendLog = true);

	/**
	*@brief ɾ����Ʒ�¼� Adder by huchenhui 2016.11.14
	*/
	void OnRemoveItem(const char* reason, Item* item, Int32 num, UInt32 oldNum);

	/**
	*@brief �ϲ���Ʒ�¼� Adder by huchenhui 2016.11.14
	*/
	void OnMerge(const char* reason, Item* item, Int32 num, UInt32 oldNum);

	/**
	*@brief �¼����¼� Adder by huchenhui 2016.11.14
	*/
	void OnNewSkill(UInt16 skillId, bool isSetting );

	/**
	*@brief ���ܵȼ��仯�¼� Adder by huchenhui 2016.11.14
	*/
	void OnChangeSkill(UInt16 skillId, std::string skillName, Int8 changeLevel, UInt8 sourceLevel, UInt8 newLevel, bool isSetting);

	/**
	*@brief �����ȼ��仯�¼� Adder by huchenhui 2017.04.24
	*/
	void OnSeasonLevelUp();

	/**
	*@brief ����ιʳ Adder by huchenhui 2017.04.24
	*/
	void OnFeedPet(PetFeedType type);

	/**
	*@brief ��ó��� Adder by huchenhui 2018.03.15
	*/
	void OnAddPet(PetType petType, PetQuality petQuality);

	/**
	*@brief �������� Adder by huchenhui 2018.03.21
	*/
	void OnPetUpLevel(PetType petType, PetQuality petQuality, UInt16 level);

	/**
	*@brief ��������ϵ Adder by huchenhui 2017.10.23
	*/
	void OnAddRelation(RelationType type);

	/**
	*@brief Ʒ�׵��� Adder by huchenhui 2018.03.14
	*/
	void OnRemakeEquip(UInt32 itemId, UInt8 qualityLevel);

	/**
	*@brief װ���ּ��� Adder by huchenhui 2018.03.14
	*/
	void OnCalculateValuedScore(ItemSubType subType, ItemQuality quality, UInt32 score);

	/**
	*@brief ��Ƕ���� Adder by huchenhui 2018.03.15
	*/
	void OnAddPreciousBead();

	/**
	*@brief ��Ƕ���� Adder by huchenhui 2018.03.17
	*/
	void OnSendHorn();

	/**
	*@brief ���빤�� Adder by huchenhui 2018.03.17
	*/
	void OnJoinGuild();

	/**
	*@brief ���빤�� Adder by huchenhui 2018.06.06
	*/
	void OnIncGuildBattleScore(UInt32 score);

	/**
	*@brief ռ����� Adder by huchenhui 2018.06.06
	*/
	void OnGuildOccupyTerritory(UInt8 terrId);

	/**
	*@brief ����˺�ʦ���Ĺ�ϵ
	*/
	void OnRelationRelieveMaster(ObjID_t masterId);

	/**
	*@brief ��ʦ�ɹ�
	*/
	void OnRelationApprenticeMaster(ObjID_t masterId);

	/**
	*@brief ͽ�ܳ�ʦ
	*/
	void OnRelationDiscipleFinishSchool(ObjID_t masterId);

	/**
	*@brief ������ʦֵ
	*/
	void OnAddGoodTeacherValueByFinishSchool(UInt64 addValue, ObjID_t discipleId);

	/**
	*@brief ������ʤ�� Adder by huchenhui 2018.03.20
	*/
	void OnWinWudaodahuiPkEnd(UInt32 winNum);

	/**
	*@brief ��չ���� Adder by huchenhui 2018.06.11
	*/
	void OnEnlargePackage(UInt32 size);

	/**
	*@brief ��չ�ֿ� Adder by huchenhui 2018.06.11
	*/
	void OnEnlargeStorage(UInt32 size);

	/**
	*@brief �ܳ���Ԩʧ��
	*/
	void OnWeekHellFail(DungeonDataEntry& dungeonData);

public://����
	/**
	 *@brief ���͵�����
	 */
	void SendToGate(Avalon::Protocol& protocol);

	/**
	 *@brief ���͵��ͻ���
	 */
	void SendProtocol(Avalon::Protocol& protocol); 

	/**
	 *@brief ������Ϣ�����ͻ���
	 */
	void SendPacket(Avalon::Packet* packet);

	/**
	*@brief ת����globalActivity
	*/
	void SendToGlobalActivity(Avalon::Protocol& protocol);
	void SendToGlobalActivity(Avalon::Packet* packet);

	void SendToSelfScene(Avalon::Protocol& protocol);

public:
	/**
	 *@brief ͬ����������
	 *@param bDirty �Ƿ�ֻͬ��������
	 */
	void SyncSelfData(bool bDirty);
	void SyncSelfData(UInt32 type, bool bDirty);

	/**
	 *@brief ͬ���������ݵ���Χ����
	 */
	void SyncTeamAround(bool bDirty);
	void SyncTeamAroundToMe();
	void SyncTeamNewZone();

	/**
	*@brief ͬ�������Ϣ
	*/
	void SyncAvatarToTeam();

	/**
	*@brief ͬ�������Ϣ
	*/
	void SyncAvatarToWorld();

	/**
	 *@brief ͬ����Χ������                
             isForceSendMsg��ʾ�Ƿ�ǿ�Ʒ���
	 */
	void SyncAroundToMe(bool isForceSendMsg = false);

	/**
	 *@brief ֪ͨ�ͻ��˽��볡��
	 */
	void NotifyGateEnterScene();

	/**
	 *@brief ֪ͨ�ͻ��˼��س���
	 */
	void NotifyLoadScene();

	/**
	*@brief worldͬ���������¼����� by huchenhui 2016.08.22
	*/
	void EventProcess(SyncEventType type, ObjID_t param1, ObjID_t param2);

public:
	/**
	*@breif ���ø��°汾��
	*/
	void SetUpdateBulletin(UInt32 updateBulletin) { m_UpdateBulletin = updateBulletin; }

public://һԪ����
	/**
	*@brief ���һ��̳ǵ���
	*@param num:����
	*@return �Ƿ�����
	*/
	bool CheckMallEx(UInt32 num);

	/**
	*@brief �۳��һ��̳ǵ���
	*@param num:����
	*/
	void RemoveMallEx(UInt32 num);

	/**
	*@brief �ж��Ƿ񸻹�����̳ǵ���
	*@param mallId:�̳ǵ���ID
	*/
	bool IsMallPanicBuyId(UInt32 mallId);

public:// ���������
	/**
	*@brief �ֶ�ˢ��������
	*/
	UInt32 OnPlayerRefreshAuction();

	/**
	*@brief ������λ
	*/
	UInt32 OnPlayerBuyAuctionBooth();

	/**
	*@brief ��ȡ�쳣���׹�����
	*/
	AbnormalTransactionMgr& GetAbnormalTransactionMgr() { return m_AbnormalTransactionMgr; }

public:	// ���װ���̵�
	/**
	*@brief ������װ���̵������
	*/
	EquipShopMgr& GetEquipShopMgr() { return m_EquipShopMgr; }

public:
    // �ؿ����
    inline DungeonMgr& GetDungeonMgr() { return m_dungeonMgr; }

	// ֪ͨ�뿪���³�
	void NotifyLeaveDungeon(bool isDisconnect);

	/**
	*@brief �Ƿ�ͨ�ص��³ǵ������Ѷ�
	*/
	bool IsDungeonAnyHardPassed(UInt32 dungeonId);

	/**
	*@brief �Ƿ�ͨ�ص��³�
	*/
	bool IsDungeonPassed(UInt32 dungeonId);

	/**
	*@brief ���������
	*/
	UInt32 RequestBuyDungeonTimes(DungeonSubType subType);
	
	/**
	*@brief ��ȡ���³Ǵ���
	*/
	UInt32 GetDungeonDailyCount(UInt32 dungeonId){ return m_dungeonMgr.GetDailyCount(dungeonId); }

	/**
	*@breif ���ӵ��³Ǵ���
	*/
	void AddDungeonTimes(UInt8 subType, UInt32 times);
	/**
	*@breif ��ȡ���³���óɼ�
	*/
	UInt32 GetDungeonBestScore(UInt32 dungeonId);

public:
	/**
	*@brief ����������֮������
	*/
	UInt32 RequestBuyTowerTimes();

	// ����֮��
	void OnClearDeathTowerFloorMonsters(UInt32 dungeonId, UInt32 areaId, UInt32 usedTime, UInt32 remainHp, UInt32 remainMp);

	// ��ȡ����֮������
	static UInt32 GetTowerFloorByAreaID(UInt32 dungeonId, UInt32 areaId);

	// ��������֮�����㽱��
	DeathTowerAwardDataEntry* GetDeathTowerFloorAward(UInt32 floor);

	/**
	*@brief ����֮����߲�
	*/
	UInt32 GetDeathTowerTopFloor();
	void SetDeathTowerTopFloor(UInt32 floor);

	/**
	*@brief ����֮����߲���ʱ
	*/
	UInt32 GetDeathUsedTime();
	void SetDeathUsedTime(UInt32 usedTime);

	/**
	*@brief ����֮������
	*/
	bool CheckRefreshDeathTowerCount();
	void RefreshDeathTowerCount();

	/**
	*@brief ��������֮��
	*/
	UInt32 OnResetDeathTower();

	/**
	*@brief ��ʼɨ������֮��
	*/
	UInt32 OnBeginWipeoutDeathTower();

	/**
	*@brief ��ȡ����֮��ɨ������
	*/
	UInt32 OnRequestDeathTowerWipeoutResult();

	/**
	*@brief ��ȡ�ƶ���������֮��ɨ������
	*/
	UInt32 OnRequestDeathTowerWipeouttQueryResult(UInt32 beginFloor, UInt32 endFloor);

	/**
	*@brief �����������֮��ɨ��
	*/
	UInt32 OnQuickFinishDeathTowerWipeout();

	/**
	*@brief ����֮����ͨ����
	*/
	UInt32 OnRequestDeathTowerPassFloorAward(UInt32 floor);

	/**
	*@brief ��������֮����¼
	*/
	UInt32 ResetTowerCounter();

public:	// ����
	inline UInt32 GetPackSize() { return m_PackSize; }
	void SetPackSize(UInt32 pksz);
	inline UInt8 GetStorageSize() { return m_StorageSize; }
	void SetStorageSize(UInt8 sz);
	// �����µ��߱�ʶ
	void UpdateNewItem(UInt8 packageType);

	/**
	*@brief ��ȡ���������ӳ�
	*/
	UInt32 GetPackageAdditionSize(PackType type);

public:
	/**
	*@brief �ܷ����ƥ��
	*/
	UInt32 CanStartPkMatch(PkType type);

	/**
	*@brief ���ս����Ϣ
	*/
    RacePlayerInfo GetMatchRacePlayerInfo(bool pvp);

    /**
    *@brief ƥ�����
    */
    UInt32 GetMatchScore() const { return m_MatchScore; }
	void SetMatchScore(UInt32 score);

	/**
	*@brief ���ƥ�����
	*/
	UInt32 GetBestMatchScore();
	void SetBestMatchScore(UInt32 score);

	/**
	*@brief ����������ʤ�����ܼ�¼
	*/
	void UpdateSeasonPkStraightCount(PkRaceResult result);
	UInt32 GetSeasonPkWinStraight();
	UInt32 GetSeasonPkLoseStraight();

    /**
    *@brief ��ȡpkֵ
    */
    UInt32 GetPkValue() const;
	PkLevelType GetPkLevelType() const;
	UInt8 GetPkLevel() const;

    /**
    *@brief ����pkֵ
    */
    void IncPkValue(UInt32 value);

    /**
    *@brief ����pkֵ
    */
    void DecPkValue(UInt32 value);

    PkStatisticMgr& GetPkStatisticMgr() { return m_pkStatisticMgr; }

	/**
	*@brief ��ȡ�����ʤ
	*/
	UInt32 GetMaxWinStreak(PkType type);

	/**
	*@brief ��ȡ��ǰ��ʤ
	*/
	UInt32 GetCurWinStreak(PkType type);

	/**
	*@brief ��ȡʤ������
	*/
	UInt32 GetTotalWinNum(PkType type);

    /**
    *@brief ���õ�ǰʹ�õĳƺ�
    */
    void SetActiveTitle(UInt32 titleId);

	/**
	*@breif ��ȡ��ǰʹ�õĳƺ�
	*/
	UInt32 GetActiveTitle() { return m_titleId; }
public:
	// ������

	/**
	*@brief ������״̬
	*/
	WudaoStatus GetWudaoStatus() const { return (WudaoStatus)(UInt8)m_WudaoStatus; }
	void SetWudaoStatus(WudaoStatus status) { m_WudaoStatus = (UInt8)status; }

	/**
	*@brief �μ�������
	*/
	UInt32 JoinWudaodahui();

	/**
	*@brief ���������
	*/
	void OnWudaodahuiPkEnd(PkRaceResult result);

	/**
	*@brief ��ȡ�����ά��
	*/
	UInt32 RequestWudaodahuiReward();

	/**
	*@brief ���������״̬
	*/
	void CheckWudaodahuiStatus(bool dayChanged);

public:
	// �������

	/**
	*@brief ����ID
	*/
	inline void SetGuildId(ObjID_t guildId) { m_GuildId = guildId; }
	inline ObjID_t GetGuildId() { return m_GuildId; }

	/**
	*@brief ������
	*/
	inline void SetGuildName(const std::string& name) { m_GuildName = name; }
	inline std::string GetGuildName() const { return m_GuildName; }

	/**
	*@brief ����(��Ӧö��GuildPost)
	*/
	inline void SetGuildPost(UInt8 post) { m_GuildPost = post; }
	inline UInt8 GetGuildPost() const { return m_GuildPost; }

	/**
	*@brief ���빫��ʱ��
	*/
	inline void SetGuildEnterTime(UInt32 time) { m_GuildEnterTime = time; }
	inline UInt32 GetGuildEnterTime() { return m_GuildEnterTime; }

	/**
	*@brief ���ṱ��
	*/
	void AddGuildContribution(const char* reason, UInt32 num, bool syncToWorld = false);
	void RemoveGuildContribution(const char* reason, UInt32 num);
	inline UInt32 GetGuildContribution() const { return m_GuildContri; }

	/**
	*@brief ���󹫻�һ�
	*/
	void RequestGuildExchange();

	/**
	*@brief �����̵�
	*/
	inline UInt8 GetGuildShopID() const { return m_GuildShopId; };
	inline void SetGuildShopID(UInt8 id) { m_GuildShopId = id; }

	/**
	*@brief ���Ὠ��
	*/
	void SetGuildBuildingLevel(GuildBuildingType type, UInt8 level);
	UInt8 GetGuildBuildingLevel(GuildBuildingType type);

	/**
	*@brief ���������
	*/
	void SetGuildOccupyCrossTerrId(UInt8 terrId) { m_GuildOccupyCrossTerrId = terrId; }
	UInt8 GetGuildOccupyCrossTerrId() { return m_GuildOccupyCrossTerrId; }
	

	/**
	*@brief ���
	*/
	void SetRedPointFlag(UInt32 flag) { m_RedPointFlag = flag; }
	UInt32 GetRedPointFlag() const { return m_RedPointFlag; }
	void AddRedPoint(RedPointFlag flag) { m_RedPointFlag = m_RedPointFlag | (1 << flag); }
	void ClearRedPoint(RedPointFlag flag) { m_RedPointFlag = m_RedPointFlag & (~(1 << flag)); }

	/**
	*@brief ���ù���ս����
	*/
	void SetGuildBattleNumber(UInt32 num) { m_GuildBattleNumber = num; }

	/**
	*@brief ���ù���ս����
	*/
	void SetGuildBattleScore(UInt32 score) { m_GuildBattleScore = score; }

	/**
	*@brief ���ù���齱״̬
	*/
	void SetGuildBattleLotteryStatus(UInt8 status) { m_GuildBattleLotteryStatus = status; }

	/**
	*@brief ���ù���ս������ȡ���
	*/
	void SetGuildBattleReward(std::string mask);

	/**
	*@brief ����ռ�
	*/
	void SetGuildEmblem(UInt32 lvl);
	UInt32 GetGuildEmblem() { return m_GuildEmblem; }

public:
	// ����
	void OnRemoveFigureStatus(FigureStatueInfo info);

	void OnCreateFigureStatus(FigureStatueInfo info);

public:
	// ����ɸѡ���

	/**
	*@brief �㲥��������
	*/
	virtual void BroadcastNewZone(Avalon::Packet* packet);
	virtual void BroadcastOldZone(Avalon::Packet* packet);

	virtual void Broadcast(Avalon::Protocol& protocol);
	virtual void Broadcast(Avalon::Packet* packet);
	virtual void BroadcastExceptMe(Avalon::Protocol& protocol);
	virtual void BroadcastExceptMe(Avalon::Packet* packet);

	/**
	*@brief �����Χ�����
	*/
	void AddAroundPlayer(Player* player, bool notify);

	/**
	*@brief �����Χ���е����
	*/
	void AddAllAroundPlayers();

	/**
	*@brief �����Χ�����
	*/
	void DelAroundPlayer(Player* player, bool notify);

	/**
	*@brief ɾ����Χ���е����
	*/
	void DelAllAroundPlayers();

	/**
	*@brief ��ȡ��Χ�۲��Լ�����������б�
	*/
	void GetWatchPlayerList(std::vector<ObjID_t>& roleIdList, bool containSelf);

	/**
	*@brief �Ƿ�۲��Ŀ��
	*/
	bool IsWatchThisPlayer(ObjID_t id);

	/**
	*@brief �����µĹ۲��б�
	*/
	void CountNewWatchList(ObjID_t changedRoleId, bool notify);

	/**
	*@brief �����µĹ۲��б�
	*/
	void CountNewWatchList2(bool sync);

	/**
	*@brief �����µĹ۲��б�(3v3�����µ�ͬ����ʽ)
	*/
	void CountNewWatchList_3V3(bool sync);

	/**
	*@brief ��ȡ���۲��������
	*/
	UInt32 GetMaxWatchPlayerNum();

public:
	// ���ٹ������

	/**
	*@brief ��ʼ���ٹ�������
	*/
	UInt32 BeginQuickBuyTransaction(QuickBuyTransaction* trans);

	/**
	*@brief ���ٹ������񷵻�
	*/
	void OnQuickBuyTransactionRes(UInt32 id, UInt32 result);

public:
	/**
	*@brief ������Ҳ鿴�Լ�����Ϣ
	*/
	void OnOhterPlayerWatch(PlayerWatchInfo& info);

public: //added by aprilliu, 2016.04.12
	void HandleProtocol(CLProtocol::SceneAcceptTask& request);
	void HandleProtocol(CLProtocol::SceneSubmitTask& request);

public: //�̵�
	inline ShopMgr& GetShopMgr() { return m_ShopMgr; }

public: //�̳�
	//�����̳���Ʒ
	void OnBuyMallGoods(CLProtocol::WorldMallCheckBuy& req);

	//װ��������ѯ��������
	MallGiftPackActivateCond CheckEquipBrokenActivateCond();

	//װ��������ȡ��������
	MallGiftPackActivateCond GetActivateCond(PlayerLevel level);

	//�����̳���ʱ�������
	void ActivateMallGiftPackReq(MallGiftPackActivateCond cond);

	//����������鼤�����
	void CheckMallGiftPackOnLevelUp();

	/**
	*@brief ���������������ݿ����״̬
	*/
	void SetConditionActivateDBLoadState(UInt8 loadState) { m_ConditionActivateDBLoadState = loadState; }

public: 
	// ���
	void AddRedPacket(UInt16 reason);

	void OnNewYearRedPacket(UInt32 num);

	void SyncRedPacketInfoToCenter();

public:
	// �ͽ�����

	/**
	*@brief �ͽ����������ɹ�
	*/
	void OnPremiumLeagueEnroll();

public:
	// ��ֵ

	/**
	*@brief ���ͳ�ֵ����
	*/
	UInt32 GiveChargeGoods(const CLProtocol::SceneBillingSendGoods& goods);

	/**
	*@brief �¿�
	*/
	void OnBuyMonthCard(const char* reason);

	/**
	*@brief ����¿�����
	*/
	void OnMonthCardWelfare(UInt32 remainDays, bool isRenew, bool bDayChange = false, bool isOnline = false);

	/**
	*@brief ��ʼ���¿�����ʱ��
	*/
	void InitMonthCardExpireTime(UInt32 time);

	/**
	*@brief ��û���¿�
	*/
	bool HasMonthCard() const;

	/**
	*@brief ��ȡ�¿�ʣ������
	*/
	UInt32 GetMonthCardRemainDays() const;

	UInt32 GetTotChargeNum() const;
	UInt32 GetDayChargeNum() const;
	void AddPlayerChargeTotal(UInt32 deltaCharge);
	UInt32 GetPlayerChargeTotal() const;

	/**
	*@brief ��ƿ�
	*/
	void OnBuyMoneyManageCard(const char* reason, bool isSendPoint = true);

	/**
	*@brief ��ʼ����ƹ���
	*/
	void InitMoneyManage(UInt8 status);

	/**
	*@brief ��ȡ��ƹ�����
	*/
	UInt32 GiveMoneyManageReward(UInt32 rewardId);

	/**
	*@brief ��ƿ���½����
	*/
	void UpdateMoneyManage();

public:
	//����
	UInt32 GetRoomId() { return m_RoomId; }
	void SetRoomId(UInt32 roomId) { m_RoomId = roomId; }

public:
	// ԤԼ��ɫ
	void CheckAppointmentOccu();
	void OnAppointmentActivityClose();
	bool IsAppointmentOccu() const { return m_AppointmentOccu != 0; }
	void ClearAppointmentOccu() { m_AppointmentOccu = 0; }


	void AddAppointmentCoin(const char* reason, UInt32 num);
	void RemoveAppointmentCoin(const char* reason, UInt32 num);
	UInt32 GetAppointmentCoin();

public:
	// ����npc
	void SummonTaskNpc(Task* task, bool notify);

	void OnKillCityMonster(ObjID_t raceId, const SceneNpc& cityMonster, std::vector<ObjID_t>& teammateIds);

	UInt32 GetDailyCityMonsterRemainTimes();
	
	void IncDailyCityMonsterTimes();

	UInt32 GetDailyCityMonsterTimes();

	void SyncCityMonsterRemainTimesToWorld();

public:
	//װ������
	//����װ�������ύ����
	UInt32 handleEqRecSubcmt(std::vector<UInt64>& eqUids, std::vector<EqRecScoreItem>& scoreItems, UInt32& score);

	//װ���������
	UInt32 handleEqRecRedeem(UInt64 equid, UInt32& consScore);

	//װ�����ջ�����������
	UInt32 handleEqRecUpscore(UInt64 equid, UInt32& upscore);

public:
	//33������
	UInt32 GetScoreWarScore() { return m_ScoreWarScore; }
	void SetScoreWarScore(UInt32 score) { m_ScoreWarScore = score; }

	UInt8 GetScoreWarBattleCount() { return m_ScoreWarBattleCount; }
	void SetScoreWarBattleCount(UInt8 count) { m_ScoreWarBattleCount = count; }

	UInt8 GetScoreWarWinBattleCount() { return m_ScoreWarWinBattleCount; }
	void SetScoreWarWinBattleCount(UInt8 count) { m_ScoreWarWinBattleCount = count; }
	void IncScoreWarWinBattleCount() { m_ScoreWarWinBattleCount = m_ScoreWarWinBattleCount + 1; }

	UInt32 GetScoreWarLastBattleTime() { return m_ScoreWarLastBattleTime; }
	void SetScoreWarLastBattleTime(UInt32 time) { m_ScoreWarLastBattleTime = time; }

	bool CheckScoreWarReward(UInt32 rewardId) { return m_ScoreWarRewardMask.CheckMask(rewardId); }
	void SetScoreWarReward(UInt32 rewardId) { m_ScoreWarRewardMask.SetMask(rewardId); }
	void ClearScoreWarReward() { m_ScoreWarRewardMask.ClearMask(); }

	void UpdateScoreWarStraightCount(PkRaceResult result);
	UInt32 GetScoreWarWinStraight();
	UInt32 GetScoreWarLoseStraight();
	void InitScoreWarStraightCount();

	UInt32 GiveScoreWarReward(UInt32 rewardId);

	void SendScoreWarSortListInfo();

public: // �齱
	/**
	*@brief ��ԨƱ
	*/
	UInt32 DrawPrizeForHellTicket(UInt32 drawPrizeId, UInt32& rewardId);

	/**
	*@brief �齱ȯ
	*/
	UInt32 DrawPrizeForRaffleTicket(UInt32 drawPrizeId, UInt32& rewardId);

public: // �������ӻ

	/**
	*@brief ���������ۿ���Ϣͬ��
	*/
	void SyncArtifactJarDiscountInfo(UInt32 opActId, UInt8 extractDiscountStatus = 0, UInt32 discountRate = 0, UInt32 discountEffectTimes = 0);

public: //����
	inline UInt32 GetCount(const char* name)	{ return GetCounterMgr().GetCounter(name); }

	void OnSysRecordDataRes(const CLProtocol::SceneSysRecordDataRes& protocol);

	// ͬ��sys record
	void SyncSysRecordData(UInt32 rareType, UInt32 parm1, int num);

	// ����sys record����
	void RequestSysRecordData(UInt8 sysRecordEvent, UInt8 rareControlType, const std::vector<UInt32>& params, const std::vector<UInt32>& extraParam);

	/**
	*@brief ͬ��������Ϣ��world
	*/
	void SyncBaseInfoToWorld();
	void SyncBaseInfoToWorld(UInt16 oldLv);
	void SyncBaseInfoToWorld(const char* incExpReason, UInt64 incExp);

	inline void SetRefTime(UInt64 time)	{ m_RefTime = time; }
	inline UInt64 GetRefTime()	{ return m_RefTime; }

	inline void SetTotalOnlineTime(UInt32 time)	{ m_totOnlineTime = time; }
	inline UInt32 GetTotelOnlineTime()	{ return m_totOnlineTime; }

public:
	void OnGetMallItem(IEventParam* param);
	inline ObjID_t GetMallGetItemID()	{ return m_MallItemId; }
	inline void ClearMallGetItem()	{ m_MallItemId = 0; }

public:
	bool IsReadedNotify(UInt32 type, UInt32 param);
	void ReadNotify(UInt32 type, UInt64 param);
	void ClearReadNotify(UInt32 type, UInt32 param);

	void SendNotifyTimeItemState(std::vector<Item*>& items);

	// ͷ���
public:
	HeadFrameMgr& GetHeadFrameMgr() { return m_HeadFrameMgr; }
	void SetHeadFrameId(UInt32 id) { m_HeadFrameId = id; }
	UInt32 GetHeadFrameId() { return m_HeadFrameId; }

#ifdef _DEBUG
public:
	void SyncDebugChat(std::string word);
#endif

private:
	bool CheckPublishTask(UInt32 npcID, UInt32 taskID);
	bool CheckSubmitTask(UInt32 npcID, UInt32 taskID);

public: //��������
	
	CL_SYNC_PROPERTY_BEGIN(Creature)
	CL_SYNC_PROPERTY(m_ZoneId, SOA_ZONEID, SST_SCENE | SST_AROUND | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_Exp, SOA_EXP, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_BlessExp,	SOA_BLESSEXP,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_Occu, SOA_OCCU, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_ScenePos, SOA_SCENE_POS, SST_SCENE_ENTER)
	CL_SYNC_PROPERTY(m_SceneDir, SOA_SCENE_DIR, SST_SCENE_ENTER)
	CL_SYNC_PROPERTY(m_VipLvl, SOA_VIPLVL, SST_SELF | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_AROUND | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_qqVipInfo,	SOA_QQVIPINFO,		SST_SELF|SST_WATCH|SST_SCENE|SST_CHALLENGE)
	CL_SYNC_PROPERTY(m_Gold,		SOA_GOLD,		SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_BindGold,	SOA_BINDGOLD,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_Point,		SOA_POINT,		SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_BindPoint,	SOA_BINDPOINT, SST_SELF | SST_SCENE|SST_TEST)
	//CL_SYNC_PROPERTY(m_SpiritStone, SOA_SPIRITSTONE,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_Honour,		SOA_HONOUR,		SST_SELF|SST_WATCH|SST_SCENE)
	CL_SYNC_PROPERTY(m_PackSize,	SOA_PACKSIZE,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_StorageSize,	SOA_STORAGESIZE,	SST_SELF|SST_SCENE| SST_TEST)
	CL_SYNC_PROPERTY(m_SuitEffect, SOA_SUITEFFECT, SST_SELF | SST_WATCH | SST_CHALLENGE)
	CL_SYNC_PROPERTY(m_SkillBar,	SOA_SKILLBAR,	SST_SELF|SST_SCENE|SST_TEST)
	//CL_SYNC_PROPERTY(m_ItemBar,		SOA_ITEMBAR,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_Options,		SOA_GAMEOPTIONS,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_ItemCD,		SOA_ITEMCD,		SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_isTeamMaster,	SOA_TEAMMASTER,	SST_SELF)
	CL_SYNC_PROPERTY(m_TeamSize,	SOA_TEAMSIZE,	SST_SELF)
	CL_SYNC_PROPERTY(m_StoryId,		SOA_STORY,		SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_titleId, SOA_TITLE, SST_SELF | SST_AROUND | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_UpdateBulletin, SOA_UPDATEBULLETIN,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_FuncNotify, SOA_FUNCNOTIFY, SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_ActionFlag,	SOA_ACTIONFLAG, SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_FuncFlag,	SOA_FUNCFLAG,	SST_SELF|SST_SCENE|SST_TEST)
	CL_SYNC_PROPERTY(m_SrcZoneId,	SOA_SRCZONEID,	SST_SELF|SST_SCENE)
	CL_SYNC_PROPERTY(m_DayChargeNum, SOA_DAYCHARGENUM, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_NetSp, SOA_SP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_Awaken, SOA_AWAKEN, SST_SELF | SST_SCENE | SST_TEST)
    CL_SYNC_PROPERTY(m_pkValue, SOA_PKVALUE, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_Fatigue, SOA_FATIGUE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_WarriorSoul, SOA_WARRIOR_SOUL, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MatchScore, SOA_MATCH_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_CounterMgr, SOA_COUNTER, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PkCoin, SOA_PK_COIN, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_RetinueMgr.GetSceneRetinueInfo(), SOA_RETINUE, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_avatar, SOA_AVATAR, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_reviveCoin, SOA_REVIVE_COIN, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_Vip, SOA_VIP, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND | SST_BATTLE | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_CreateTime, SOA_CREATE_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_NewBoot, SOA_NEW_BOOT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_BootFlag, SOA_BOOT_FLAG, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_DeathTowerWipeoutEndTime, SOA_TOWER_WIPEOUT_END_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildName, SOA_GUILD_NAME, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_GuildPost, SOA_GUILD_POST, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_GuildContri, SOA_GUILD_CONTRI, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildShopId, SOA_GUILD_SHOP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_RedPointFlag, SOA_RED_POINT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleNumber, SOA_GUILD_BATTLE_NUMBER, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleScore, SOA_GUILD_BATTLE_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleRewardMask, SOA_GUILD_BATTLE_REWARD, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_DailyTaskScore, SOA_DAILY_TASK_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_DailyTaskRewardMask, SOA_DAILY_TASK_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_WudaoStatus, SOA_WUDAO_STATUS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MonthCardExpireTime, SOA_MONTH_CARD_EXPIRE_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_CustomField, SOA_CUSTOM_FIELD, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonLevel(), SOA_SEASON_LEVEL, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonStar(), SOA_SEASON_STAR, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonExp(), SOA_SEASON_EXP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonUplevelBattle(), SOA_SEASON_UPLEVEL_RECORD, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonAttr(), SOA_SEASON_ATTR, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_SeasonLevel.GetSeasonKingMarkCount(), SOA_SEASON_KING_MARK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AuctionRefreshTime, SOA_AUCTION_REFRESH_TIME, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AuctionAdditionBooth, SOA_AUCTION_ADDITION_BOOTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GoldJarPoint, SOA_GOLDJAR_POINT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MagJarPoint, SOA_MAGJAR_POINT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PetMgr.GetFollowPetDataId(), SOA_PET_FOLLOW, SST_SELF | SST_TEST | SST_SCENE | SST_AROUND | SST_LOSTDUNGEON)
	CL_SYNC_PROPERTY(m_potionSet, SOA_POTION_SET, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PreOccu, SOA_PRE_OCCU, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GuildBattleLotteryStatus, SOA_GUILD_BATTLE_LOTTERY_STATUS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PvpSkillMgr, SOA_PVP_SKILLS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_NetSp, SOA_PVP_SP, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PvpSkillBar, SOA_PVP_SKILLBAR, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AchievementScore, SOA_ACHIEVEMENT_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AchievementTaskRewardMask, SOA_ACHIEVEMENT_TASK_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_WeaponBar, SOA_WEAPON_BAR, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AppointmentOccu, SOA_APPOINTMENT_OCCU, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_PackSizeAddition, SOA_PACKAGE_SIZE_ADDITION, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MoneyManageStatus, SOA_MONEY_MANAGE_STATUS, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MoneyManageRewardMask, SOA_MONEY_MANAGE_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarScore, SOA_SCORE_WAR_SCORE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarBattleCount, SOA_SCORE_WAR_BATTLE_COUNT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarRewardMask, SOA_SCORE_WAR_REWARD_MASK, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_ScoreWarWinBattleCount, SOA_SCORE_WAR_WIN_BATTLE_COUNT, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_AcademicTotalGrowth, SOA_ACADEMIC_TOTAL_GROWTH_VALUE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterDailyTaskGrowth, SOA_MASTER_DAILYTASK_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterAcademicTaskGrowth, SOA_MASTER_ACADEMICTASK_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterUplevelGrowth, SOA_MASTER_UPLEVEL_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterGiveEquipGrowth, SOA_MASTER_GIVEEQUIP_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterGiveGiftGrowth, SOA_MASTER_GIVEGIFT_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_MasterTeamClearDungeonGrowth, SOA_MASTER_TEAMCLEARDUNGON_GROWTH, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_GoodTeacherValue, SOA_MASTER_GOODTEACHER_VALUE, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_RoomId, SOA_ROOM_ID, SST_CHALLENGE)
	CL_SYNC_PROPERTY(m_ShowFashionWeapon, SOA_SHOW_FASHION_WEAPON, SST_SELF | SST_TEST | SST_SCENE)
	CL_SYNC_PROPERTY(m_weaponLeaseTickets, SOA_WEAPON_LEASE_TICKETS, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_GameSet, SOA_GAME_SET, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_AdventureTeamName, SOA_ADVENTURE_TEAM_NAME, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_HeadFrameId, SOA_HEAD_FRAME, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_NewTitleName, SOA_NEW_TITLE_NAME, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_NewTitleGuid, SOA_NEW_TITLE_GUID, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_CHALLENGE | SST_TEST)
	CL_SYNC_PROPERTY(m_ChijiHp, SOA_CHIJI_HP, SST_SELF | SST_TEST)
	CL_SYNC_PROPERTY(m_ChijiMp, SOA_CHIJI_MP, SST_SELF | SST_TEST)
	CL_SYNC_PROPERTY(m_PackageTypeStr, SOA_PACKAGE_TYPE, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_GuildEmblem, SOA_GUILD_EMBLEM, SST_SELF | SST_AROUND | SST_WATCH | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_OppoVipLevel, SOA_OPPO_VIP_LEVEL, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_ChijiScore, SOA_CHIJI_SCORE, SST_SELF | SST_SCENE | SST_TEST)
	CL_SYNC_PROPERTY(m_LostDungeonHp, SOA_LOSTDUNGEON_HP, SST_SELF | SST_TEST | SST_LOSTDUNG_ENTER_SYNC | SST_AROUND)
	CL_SYNC_PROPERTY(m_LostDungeonMp, SOA_LOSTDUNGEON_MP, SST_SELF | SST_TEST | SST_LOSTDUNG_ENTER_SYNC | SST_AROUND)
	CL_SYNC_PROPERTY(m_LostDungeonScore, SOA_LOSTDUNGEON_SCORE, SST_SELF | SST_SCENE | SST_TEST | SST_AROUND)
	CL_SYNC_PROPERTY(m_lostDungBatSt, SOA_LOSTDUNGEON_BATTLE_ST, SST_SELF | SST_SCENE | SST_TEST | SST_AROUND)
		
	CL_SYNC_PROPERTY_END()

	CL_DBPROPERTY_BEGIN(Creature)
	CL_DBPROPERTY("zoneid",		m_ZoneId)
	CL_DBPROPERTY("exp",		m_Exp)
	CL_DBPROPERTY("blessexp",	m_BlessExp)
	CL_DBPROPERTY("occu",		m_Occu)
	CL_DBPROPERTY("pre_occu",	m_PreOccu)
	CL_DBPROPERTY("tourzone",	m_TourZoneId)
	//CL_DBPROPERTY("viplvl",		m_VipLvl)
	//CL_DBPROPERTY("viplvl", m_Vip.GetLevel())
	//CL_DBPROPERTY("vipexp", m_Vip.GetExp())
	CL_DBPROPERTY("vipduetime",	m_VipDueTime)
	CL_DBPROPERTY("createtime",	m_CreateTime)
	CL_DBPROPERTY("gold",		m_Gold)
	CL_DBPROPERTY("bindgold",	m_BindGold)
	CL_DBPROPERTY("bindpoint",	m_BindPoint)
	CL_DBPROPERTY("totlechargenum", m_TotleChargeNum)
	CL_DBPROPERTY("honour",		m_Honour) 
	CL_DBPROPERTY("packsize",	m_PackSize)
	//CL_DBPROPERTY("storagesize",	m_StorageSize)
	CL_DBPROPERTY("skillbar",	m_SkillBar)
	CL_DBPROPERTY("pvp_skillbar", m_PvpSkillBar)
	CL_DBPROPERTY("options",	m_Options)
	CL_DBPROPERTY("taskmask",	m_TaskMask)
	CL_DBPROPERTY("storymask",	m_StoryMask)
	CL_DBPROPERTY("story",		m_StoryId)
	CL_DBPROPERTY("itemcd",		m_ItemCD)
	CL_DBPROPERTY("routine",	m_RoutineMgr)
	CL_DBPROPERTY("actionflag",	m_ActionFlag)
	CL_DBPROPERTY("updatebulletin",	m_UpdateBulletin)
	CL_DBPROPERTY("revivecost",	m_ReviveCost)
	CL_DBPROPERTY("revivetime",	m_ReviveTime)
	CL_DBPROPERTY("continuousdays", m_ContinuousDays)
	CL_DBPROPERTY("funcflag",	m_FuncFlag)
	CL_DBPROPERTY("actioncount",	m_ActionCounterMgr)
	//CL_DBPROPERTY("totleonlinetime", m_totOnlineTime)	//�������ܵ�����ʱ��
	CL_DBPROPERTY("equipshop", m_EquipShopMgr)
	CL_DBPROPERTY("sp", m_SP)
	CL_DBPROPERTY("pvp_sp", m_PvpSP)
	CL_DBPROPERTY("awaken", m_Awaken)
    CL_DBPROPERTY("pkvalue", m_pkValue)
	CL_DBPROPERTY("fatigue", m_Fatigue)
    CL_DBPROPERTY("warrior_soul", m_WarriorSoul)
	CL_DBPROPERTY("pk_coin", m_PkCoin)
    CL_DBPROPERTY("match_score", m_MatchScore)
	CL_DBPROPERTY("retinuefighting", m_RetinueMgr.GetRetinueFighting())
	CL_DBPROPERTY("avatar", m_avatar);
	CL_DBPROPERTY("revive_coin", m_reviveCoin);
	CL_DBPROPERTY("offretinuelist", m_RetinueMgr.GetOffRetinueList());
	CL_DBPROPERTY("bootflag", m_BootFlag);
	CL_DBPROPERTY("newboot", m_NewBoot);
	CL_DBPROPERTY("tower_wipeout_end_time", m_DeathTowerWipeoutEndTime);
	CL_DBPROPERTY("guild_contri", m_GuildContri);
	CL_DBPROPERTY("redpoint", m_RedPointFlag);
	CL_DBPROPERTY("daily_task_score", m_DailyTaskScore);
	CL_DBPROPERTY("daily_task_reward_mask", m_DailyTaskRewardMask);
	CL_DBPROPERTY("wudao_status", m_WudaoStatus);
	CL_DBPROPERTY("forbid_talk_duetime", m_ForbitTalkDueTime);
	CL_DBPROPERTY("custom_field", m_CustomField);
	CL_DBPROPERTY("season_id", m_SeasonLevel.GetSeasonId());
	CL_DBPROPERTY("season_level", m_SeasonLevel.GetSeasonLevel());
	CL_DBPROPERTY("season_star", m_SeasonLevel.GetSeasonStar());
	CL_DBPROPERTY("season_exp", m_SeasonLevel.GetSeasonExp());
	CL_DBPROPERTY("season_change_time", m_SeasonLevel.GetSeasonChangeTime());
	CL_DBPROPERTY("season_play_status", m_SeasonLevel.GetSeasonPlayStatus());
	CL_DBPROPERTY("season_uplevel_records", m_SeasonLevel.GetSeasonUplevelBattle());
	CL_DBPROPERTY("season_attr", m_SeasonLevel.GetSeasonAttr());
	CL_DBPROPERTY("season_king_mark_count", m_SeasonLevel.GetSeasonKingMarkCount());
	CL_DBPROPERTY("season_attr_end_time", m_SeasonLevel.GetChangeSeasonAttrTime());

	CL_DBPROPERTY("auction_refresh_time", m_AuctionRefreshTime);
	CL_DBPROPERTY("auction_addition_booth", m_AuctionAdditionBooth);
	CL_DBPROPERTY("platform", m_Pf);
	CL_DBPROPERTY("openid", m_OpenId);
	CL_DBPROPERTY("last_reset_gjarpoint", m_LastResetGoldJarPointTime);
	CL_DBPROPERTY("last_reset_mjarpoint", m_LastResetMagJarPointTime);
	CL_DBPROPERTY("goldjar_point", m_GoldJarPoint);
	CL_DBPROPERTY("magjar_point", m_MagJarPoint);

	CL_DBPROPERTY("pet_battle", m_PetMgr.GetBattlePetMap());
	CL_DBPROPERTY("pet_follow", m_PetMgr.GetFollowPetId());
	//CL_DBPROPERTY("read_notify", m_readNotify);
	CL_DBPROPERTY("potion_set", m_potionSet);
	CL_DBPROPERTY("month_card_expire_time", m_MonthCardExpireTime);
	CL_DBPROPERTY("is_check_sp", m_IsCheckSp);
	CL_DBPROPERTY("pvp_skills", m_PvpSkillMgr);
	CL_DBPROPERTY("sky_fashion_get", m_FashionSkyGetBit);
	CL_DBPROPERTY("achievement_score", m_AchievementScore);
	CL_DBPROPERTY("daychargenum", m_DayChargeNum)
	CL_DBPROPERTY("achievement_task_reward_mask", m_AchievementTaskRewardMask);
	CL_DBPROPERTY("weapon_bar", m_WeaponBar);
	CL_DBPROPERTY("is_appointment_occu", m_AppointmentOccu);
	CL_DBPROPERTY("money_manage_reward_mask", m_MoneyManageRewardMask)
	CL_DBPROPERTY("money_manage_status", m_MoneyManageStatus)
	
	CL_DBPROPERTY("score_war_score", m_ScoreWarScore)
	CL_DBPROPERTY("score_war_battle_count", m_ScoreWarBattleCount)
	CL_DBPROPERTY("score_war_win_battle_count", m_ScoreWarWinBattleCount)
	CL_DBPROPERTY("score_war_reward_mask", m_ScoreWarRewardMask)
	CL_DBPROPERTY("score_war_last_battle_time", m_ScoreWarLastBattleTime)

	CL_DBPROPERTY("academic_total_growth", m_AcademicTotalGrowth)
	CL_DBPROPERTY("master_dailytask_growth", m_MasterDailyTaskGrowth)
	CL_DBPROPERTY("master_academictask_growth", m_MasterAcademicTaskGrowth)
	CL_DBPROPERTY("master_giveequip_growth", m_MasterGiveEquipGrowth)
	CL_DBPROPERTY("master_uplevel_growth", m_MasterUplevelGrowth)
	CL_DBPROPERTY("master_givegift_growth", m_MasterGiveGiftGrowth)
	CL_DBPROPERTY("master_teamcleardungeon_growth", m_MasterTeamClearDungeonGrowth)
	CL_DBPROPERTY("good_teacher_value", m_GoodTeacherValue)
	CL_DBPROPERTY("fin_sch_disciple_sum", m_FinSchDiscipleSum)

	CL_DBPROPERTY("score_war_score", m_ScoreWarScore)
	CL_DBPROPERTY("score_war_battle_count", m_ScoreWarBattleCount)
	CL_DBPROPERTY("score_war_reward_mask", m_ScoreWarRewardMask)
	CL_DBPROPERTY("score_war_last_battle_time", m_ScoreWarLastBattleTime)

	CL_DBPROPERTY("show_fashion_weapon", m_ShowFashionWeapon)
	CL_DBPROPERTY("game_set", m_GameSet)
	CL_DBPROPERTY("role_value_score", m_RoleValueScore)
	CL_DBPROPERTY("head_frame_id", m_HeadFrameId)
	CL_DBPROPERTY("new_title_guid", m_NewTitleGuid)
	CL_DBPROPERTY("chiji_score", m_ChijiScore)
	CL_DBPROPERTY("chiji_score_reset_time", m_ChijiScoreResetTime)

	CL_DBPROPERTY_END()
	

	DEFINE_CHANGESCENE_SERIALIZATION(Creature)
	{
		stream.SeriaArray(m_TickTime, LOGICTICK_MAX);

		stream & m_TourZoneId;
		stream & m_OpenId;
		stream & m_Source1 & m_Source2;
		stream & m_Inviter;
		stream & m_Pf;
		stream & m_DeviceId;
		stream & m_Model;
		stream & m_DeviceVersion;
		stream & m_LocalSavebackTime;
		stream & m_VipDueTime;
		stream & m_CreateTime;
		stream & m_OnlineTime;
		stream & m_AssetsLockTime & m_RecoverTime & m_BusyTime;

		stream & m_EnterScene & m_EnterPos;
		stream & m_ReviveTime & m_ReviveCost & m_ReviveTimes;

		stream & m_GmAuthority & m_ClientOption & m_ChatTimer;
		stream & m_TaskMask & m_TaskMgr;
		stream & m_StoryMask;
		stream & m_ItemMgr;

		stream & m_CounterMgr & m_ActionCounterMgr;
		stream & m_RoutineMgr;

		stream & m_WallowFactor;

		stream & m_ContinuousDays;

		stream & m_EquipShopMgr;
		stream & m_SP;
		stream & m_Awaken;
        stream & m_dungeonMgr;
        stream & m_pkValue;
        stream & m_MatchScore;
        stream & m_titleId;
        stream & m_WarriorSoul;
		stream & m_PkCoin;
		stream & m_avatar;

		//stream & m_SkillBar;
		//stream & m_ItemBar;
	}

	void SyncToRecord(bool bPos = false);

	//�����Ч����
	void ClearInvalidData();

private:	//��Ϸϵͳ�¼�ע��
	void _RegistOpActEvent();
	void _UnRegistOpActEvent();

private:
	/**
	 *@brief ��ȡɾ��װ�������еĲ���
	 */
	bool GetMaterials(const std::vector<ObjID_t>& ids, std::map<Item*,UInt32>& items, UInt8 type);
	void RemoveMaterials(const std::map<Item*,UInt32>& items);
	UInt32 RemoveMaterials(const char* reason, std::vector<Item*>& items, UInt32 num);

	/**
	 *@brief ͬ��������Ϣ��gate
	 */
	void SyncBaseInfoToGate();

	//��ֵͳ��
	void _OnCharge(const char* reason, UInt32 vipExp, UInt32 chargeNum);

public:
	void OnChargeMakeup(const char* reason, UInt32 vipExp, UInt32 chargeNum);

private:
	//����Ƿ��߼�����
	bool CheckLogicDayChange(const Avalon::Time& now, bool isOnline);
	//�µĿ����ж�
	bool _NewCheckLogicDayChange(const Avalon::Time& now);
	//�ϵĿ����ж�
	bool _OldCheckLogicDayChange();

	//�̳������ݿ����ж�
	bool CheckLogicDayChangeOld();

	//����Ƿ���Ȼ�տ���
	bool CheckDayChange(const Avalon::Time& now);
	//�̳������ݿ����ж�
	bool CheckDayChangeOld();
	//�µ���Ȼ�����ж�
	bool _NewCheckDayChange(const Avalon::Time& now);
	//�ϵ���Ȼ�����ж�
	bool _OldCheckDayChange();

	//�߼������ж�
	bool _CheckLogicWeekChange(const Avalon::Time& now);
	
public: //ʦͽ
	void	SetActiveTimeType(UInt8 type) { m_activeTimeType = type; }
	UInt8	GetActiveTimeType() { return m_activeTimeType; }

	void	SetMasterType(UInt8 type) { m_masterType = type; }
	UInt8   GetMasterType()	{ return m_masterType; }

	void	SetRegionId(UInt8 regionId) { m_regionId = regionId; }
	UInt8   GetRegionId() { return m_regionId; }

	void	SetDeclaration(std::string str) { m_declaration = str; }
	std::string  GetDeclaration() { return m_declaration; }

	/**
	*@brief ʦͽ�ܳɳ�ֵ
	*/
	UInt32 GetMasterSysAcademicGrowth() { return m_AcademicTotalGrowth; }
	void AddMasterSysAcademicGrowth(const char* reason, UInt32 num);
	
	/**
	*@brief ��ʦֵ
	*/
	UInt32 GetMasterSysGoodTeachValue() { return m_GoodTeacherValue; }
	void AddMasterSysGoodTeachValue(const char* reason, UInt32 num);
	void RemoveSysGoodTeachValue(const char* reason, UInt32 num);

	/**
	*@brief ʦ���ճ������ܳɳ�ֵ
	*/
	UInt32 GetMasterDailyTaskGrowth() { return m_MasterDailyTaskGrowth; }
	void AddMasterDailyTaskGrowth(const char* reason, UInt32 num);

	/**
	*@brief ʦ��ѧҵ�ɳ������ܳɳ�ֵ
	*/
	UInt32 GetMasterAcademicTaskGrowth() { return m_MasterAcademicTaskGrowth; }
	void AddMasterAcademicTaskGrowth(const char* reason, UInt32 num);

	/**
	*@brief ʦ��ʦͽ���ͨ�ص��³ǳɳ�ֵ
	*/
	UInt32 GetMasterTeamClearDungeonGrowth() { return m_MasterTeamClearDungeonGrowth; }
	void AddMasterTeamClearDungeonGrowth(const char* reason, UInt32 num, UInt32 dungeonId);

	/**
	*@brief ʦ��ͽ�������ɳ�ֵ
	*/
	UInt32 GetMasterUplevelGrowth() { return m_MasterUplevelGrowth; }
	void AddMasterUplevelGrowth(const char* reason, UInt32 num);

	/**
	*@brief ʦ������װ���ɳ�ֵ
	*/
	UInt32 GetMasterGiveEquipGrowth() { return m_MasterGiveEquipGrowth; }
	void AddMasterGiveEquipGrowth(const char* reason, UInt32 num);

	/**
	*@brief ��ʦͽ������
	*/
	void SetFinSchDiscipleSum(UInt32 value);
	UInt32 GetFinSchDiscipleSum() { return m_FinSchDiscipleSum; }

	/**
	*@brief ��ȡͽ������
	*/
	UInt32 GetDiscipleNum();

	/**
	*@brief ��ȡ���ʦ����������
	*/
	void GetMasterTaskShareData(MasterTaskShareData& masterTaskShareData);

	/**
	*@brief ����ʦͽ��ϵ���߽��
	*/
	static void HandleOfflineRelieveMaster(ObjID_t discipleId);

	/**
	*@brief ����ʦ��ϵͳ���
	*/
	UInt32 OnMasterSysGiveGift(UInt8 type, UInt32 giftDataId);

	/**
	*@brief ��������ʦ�����ֵ
	*/
	void OnLevelUpMasterSys();

	/**
	*@brief ����ʦ��װ������
	*/
	void OnGiveEquipMasterSys(UInt32 num);

public://���˻
	PlayerActivityInfo<Player>& GetPlayerActivityInfo() { return m_PlayerActivityInfo; }

public://�Լ�
	/**
	*@brief �Լ�Ѫ��
	*/
	Int32 GetChijiHp() const;
	void SetChijiHp(Int32 hp);

	/**
	*@brief ���ӡ�����Ѫ��
	*/
	void DecChijiHp(Int32 value);
	void IncChijiHp(Int32 value);

	/**
	*@brief �Լ�����
	*/
	Int32 GetChijiMp() const;
	void SetChijiMp(Int32 mp);

	/**
	*@brief ���ӡ���������
	*/
	void DecChijiMp(Int32 value);
	void IncChijiMp(Int32 value);

	/**
	*@brief �Լ�����
	*/
	void SetChijiScore(UInt32 val) { m_ChijiScore = val; }
	UInt32 GetChijiScore() { return m_ChijiScore; }

	/**
	*@brief ��ʼ���Լ�����
	*/
	void InitChijiPackage();

	/**
	*@brief ѧϰ�Լ�����
	*/
	void StudyChiJiSkill(UInt32 skillId, UInt32 skillLvl);

	/**
	*@brief �۳��Լ��̵����
	*/
	void RemoveChiJiShopCoin(const char* reason, UInt32 num);

	/**
	*@brief �Լ���ս
	*/
	void SetChijiNoWarInfo(UInt64 enemyRoleId, UInt32 battleId, UInt32 dungeonId, UInt32 choiceTime);
	UInt64 GetChijiNoWarEnemyId() { return m_ChijiNoWarEnemyId; }
	UInt32 GetChijiNoWarBattleId() { return m_ChijiNoWarBattleId; }
	UInt32 GetChijiNoWarChoiceTime() { return m_ChijiNoWarChoiceTime; }
	UInt32 GetChijiPKDungeonId() { return m_ChijiPKDungeonId; }
	void SendNoWarToBattle();

	/**
	*@brief �Լ�ְҵ�б�
	*/
	void CreateChijiOccu();
	bool IsHasChijiOccu(UInt32 occu);

private:
	/**
	*@brief ��ȡ��������
	*/
	void GetShareData(PlayerSenceShareData& sharedata);

	/**
	*@brief ͬ���������ݵ�world
	*/
	void SyncShareDataToWorld();

	/**
	*@brief ͬ��ʦ��ϵͳ����
	*/
	void SyncMasterSysDataToWorld();

public: //��ĥֵ
	/**
	*@brief ��ħֵ
	*/
	void SetResistMagic(UInt32 value) { m_ResistMagic = value; }
	UInt32 GetResistMagic() { return m_ResistMagic; }

	/**
	*@brief ͬ����ħֵ��world
	*/
	void SyncResistMagicToWorld();

	/**
	*@brief ������Ҷ���Ʒ��������
	*/
	UInt32 HandlerItemOpLock(UInt32 opType, ObjID_t itemId);

	/**
	*@brief ʱװ������ʾ����
	*/
	UInt32 SetShowFashionWeapon(UInt8 isShow);
	UInt8 GetShowFashionWeapon() { return m_ShowFashionWeapon; }

public: //�ٱ�ʱװ���װ����
	void AddChangeFashionScore(const char* reason, UInt32 num);
	void RemoveChangeFashionScore(const char* reason, UInt32 num);
	UInt32 GetChangeFashionScore();

	//�������޺��˷�
	void InitWeaponLeaseTickets(UInt32 num) { m_weaponLeaseTickets = num; }
	void AddWeaponLeaseTickets(const char* reason, UInt32 num);
	void RemoveWeaponLeaseTickets(const char* reason, UInt32 num);
	UInt32 GetWeaponLeaseTickets();


	// �Ƿ��ϱ��ع�
	bool IsVeteranReturn();
	// �ϱ��ع���
	void OnCheckVeteranReturn();

	// ��ɫ�ع�ʱ��
	void SetRoleReturnTime(UInt32 time) { m_RoleReturnTime = time; }
	UInt32 GetRoleReturnTime() const { return m_RoleReturnTime; }
	UInt32 GetRoleReturnExpireTime();

	// ��ɫ�ع�ȼ�
	void SetRoleReturnLevel(UInt32 level) { m_RoleReturnLevel = level; }
	UInt32 GetRoleReturnLevel() const { return m_RoleReturnLevel; }

	// �Ƿ��ٴλع�
	void SetAgainReturn(UInt8 isAgain) { m_AgainReturn = isAgain; }
	UInt8 GetAgainReturn() const { return m_AgainReturn; }

	// ������
	void SetEnergyCoin(UInt32 num) { m_EnergyCoin = num; }
	UInt32 GetEnergyCoin() const { return m_EnergyCoin; }
	void RefreshEnergyCoin(UInt32 num);

	// ͬ���˺ż���
	void SyncAccountCounterToWorld(UInt32 type);

	// �������ʹ�С
	void UpdatePackageTypeSize();

public: // ��Ϸ����

	void SetGameSet(UInt32 setType, const std::string& setValue);
	
	/**
	*@brief ������Ϸ�����ַ�
	*/
	void ParseGameSet();
	
	/**
	*@brief ������Ϸ�����ַ�
	*/
	void UpdateGameSet();

	/**
	*@brief ��Ϸ����ͬ����world
	*/
	void SyncGameSetWorld();

public:// ��ɫ����
	/**
	*@brief ���û�ȡ��ɫ����
	*/
	void SetRoleValueScore(UInt32 score);
	UInt32 GetRoleValueScore() const { return m_RoleValueScore; }

	/**
	*@brief ͬ����ɫ����
	*/
	void SyncRoleValueScore();

	/**
	*@brief ��ȡ�ȼ�����
	*/
	UInt32 GetPlayerLevelScore();

	/**
	*@brief ��������ڵ�������
	*/
	UInt32 CalcItemsScoreInPackage(PackType packType, bool reCalc);

	/**
	*@brief �����������
	*/
	UInt32 CalcPetsSocre(bool reCalc);

	/**
	*@brief �����ɫ����
	*/
	void CalcRoleValueScore(bool reCalc = false);

public: // ð�ն�

	/**
	*@brief ���û�ȡð�նӵȼ�
	*/
	void SetAdventureTeamLevel(UInt16 level) { m_AdventureTeamLevel = level; }
	UInt16 GetAdventureTeamLevel() const { return m_AdventureTeamLevel; }

	/**
	*@brief ���û�ȡð�ն�����
	*/
	void SetAdventureTeamGrade(UInt8 grade) { m_AdventureTeamGrade = grade; }
	UInt8 GetAdventureTeamGrade() const { return m_AdventureTeamGrade; }

public: //�Լ�
	UInt32 GetBattleID(void) { return m_BattleID; }
	void SetBabttleID(UInt32 battleID) { m_BattleID = battleID; }
	
public:	//ͷ��
	/**
	*@breif ���ͷ���޸�
	*/
	void OnNewTitleModify(UInt8 opType, UInt8 ownerType, UInt64 titleGuid, std::string titleName);

	void AddNewTitle(UInt32 titleId, UInt32 dueTime);

public: //��ʧ����
	RacePlayerInfo GetLostBattleRacePlayerInfo();
	void SetLostBattleRacePlayerInfo(const RacePlayerInfo& raceInfo) { m_lostBattleRacePlayerInfo = raceInfo; }
	inline void SetDungChageMode(UInt8 mode) { m_dungChageMode = mode; }
	inline UInt8 GetDungChageMode() { return m_dungChageMode; }
	inline void SetDungChageTeamId(UInt32 teamId) { m_dungChageTeamId = teamId; }
	inline UInt32 GetDungChageTeamId() { return m_dungChageTeamId;}
	
	LostDungeonTaskMgr& GetLostDungeonTaskMgr() { return m_LostDungeonTaskMgr; }

	inline void	SetLastLostBattleSceneId(UInt32 sceneId) { m_lastLostBattleSceneId = sceneId; }
	inline UInt32 GetLastLostBattleSceneId() { return m_lastLostBattleSceneId; }

	Int32 GetLostDungeonHp() const;
	void SetLostDungeonHp(Int32 hp);
	void DecLostDungeonHp(Int32 value);
	void IncLostDungeonHp(Int32 value);
	Int32 GetLostDungeonMp() const;
	void SetLostDungeonMp(Int32 mp);
	void DecLostDungenMp(Int32 value);
	void IncLostDungeonMp(Int32 value);

	void SetLostDungenScore(UInt32 val) { m_LostDungeonScore = val; }
	UInt32 GetLostDungenScore() { return m_LostDungeonScore; }

	void OnEnterLostDungeon(bool reconnect);

	bool IsCompLostDungeonTask();

	Int32 GetLostDungeonHpBack() const;
	void SetLostDungeonHpBack(Int32 hp);
	Int32 GetLostDungeonMpBack() const;
	void SetLostDungeonMpBack(Int32 mp);

	void SetLostDungeonSpeedBack(UInt32 speed);
	UInt32 GetLostDungeonSpeedBack();

	inline void SetLostDungBatSt(UInt8 st) { m_lostDungBatSt = st; }
	inline UInt8 GetLostDungBatSt() { return m_lostDungBatSt; }

	inline void SetLostDungProtTime(UInt32 time) { m_lostProtTime = time; }
	inline UInt32 GetLostDungProtTime() { return m_lostProtTime; }

	//��Ҳ鿴�鱨
	UInt32 OnPlayerSeeIntell(UInt64 targetId, std::vector<ItemReward>& artifacts);

	inline bool GetLostDungLeaveTag() { return m_lostDungLeaving; }
	inline void SetLostDungLeaveTag(bool tag) { m_lostDungLeaving = tag; }

	void GetLostDunegonTaskKillTargetPos(std::vector<UInt64>& players, std::vector<LostDungeonPlayerPos>& poss);
	//����ڵ�ս������id(��̬��)
	UInt32	m_lastLostBattleSceneId;

	// ����ʣ��Ѫ��(ǧ�ֱ�)
	ObjInt32					m_LostDungeonHp;
	// ����ʣ������(ǧ�ֱ�)
	ObjInt32					m_LostDungeonMp;
	// ���λ���
	ObjInt32					m_LostDungeonScore;

	// ����ս��ʱ�ĵ���ʣ��Ѫ��(ǧ�ֱ�)
	Int32					m_LostDungHpBack;
	// ����ս��ʱ�ĵ���ʣ������(ǧ�ֱ�)
	Int32					m_LostDungMpBack;
	// ����ս��ʱ����ƶ��ٶ�
	UInt32					m_LostDungSpeedBack;

	//ս������
	RacePlayerInfo	m_lostBattleRacePlayerInfo;
	//���³ǹؿ���սģʽ
	UInt8 m_dungChageMode;
	//���³ǹؿ�����id
	UInt32 m_dungChageTeamId;
	
	//��������
	LostDungeonTaskMgr m_LostDungeonTaskMgr;
	//���ս��״̬
	ObjInt8  m_lostDungBatSt;
	//��ұ���ʱ���
	UInt32   m_lostProtTime;
	//�����뿪ս��
	bool     m_lostDungLeaving;
private:
	//��������  noted by aprilliu  �յ�SceneEnterGameReqʱ����������
	Avalon::NetConnectionPtr m_Conn;
	//�˺�id
	UInt32		m_AccId;
	// �˺ű��GUID
	ObjID_t		m_AccountGuid;
	//��������id�������Ƿ��䣩
	UInt32		m_MajorCityId;

	//״̬
	Int32		m_GameStatus;
	//�޸�״̬��ʱ��
	UInt32		m_StatusTime;

	//gmȨ��
	UInt8		m_GmAuthority;
	//�ͻ���ѡ��
	UInt8		m_ClientOption;

	//openid
	ObjString	m_OpenId;

	//�߼���ʱ�¼�
	UInt64		m_TickTime[LOGICTICK_MAX];

	//oppo vip�ȼ�
	ObjUInt8	m_OppoVipLevel;

	//����
	std::string	m_City;
	//ip��ַ
	std::string	m_Ip;

	//ְҵ
	ObjUInt8	m_Occu;

	//Ԥתְְҵ
	ObjUInt8	m_PreOccu;

	//vip�ȼ�
	ObjUInt8	m_VipLvl;

	//vip����
	ObjUInt32	m_VipExp;

	//�ܳ�ֵ���
	UInt32		m_TotalChargeNum;

	//vip����ʱ��
	ObjUInt32	m_VipDueTime;
	//vip����buff
	Buff*		m_pVipBuff;

	//qq��Ȩ��Ϣ;
	QQVipInfo	m_qqVipInfo;

	//��Դ1
	std::string	m_Source1;
	//��Դ2
	std::string m_Source2;
	//������
	UInt32		m_Inviter;
	//ƽ̨
	ObjString	m_Pf;
	//�豸id
	std::string m_DeviceId;
	//�豸�ͺ�
	std::string m_Model;
	//�豸�汾
	std::string m_DeviceVersion;

	//��id
	ObjUInt32	m_ZoneId;

	//����
	ObjUInt64	m_Exp;
	//ף������
	ObjUInt32	m_BlessExp;

	//�ϴ�ԭ�ظ���ʱ��
	ObjUInt32	m_ReviveTime;
	//��������
	ObjUInt32	m_ReviveCost;
	//�����������
	UInt32		m_ReviveTimes;

	//���������������
	PlayerDataEntry*	m_pLevelDataEntry;

	//�����ʱ��
	ChatTimer	m_ChatTimer;

	//���������
	UInt8		m_LoadDataCount;
	//���ؽ���
	UInt64		m_LoadDataSerial;

	//�ͷ���־
	UInt32		m_PunishFlag;
	//���Ե���ʱ��
	ObjUInt32	m_ForbitTalkDueTime;

	//�����浵ʱ��
	UInt64		m_LocalSavebackTime;
	//��ɫ����ʱ��
	ObjInt32	m_CreateTime;
	//����ʱ��
	UInt64		m_OnlineTime;
	//������ʱ��(s)
	ObjUInt32		m_DayOnlineTime;
	//������ʱ��(s)
	UInt32		m_totOnlineTime;
	//�ָ�ʱ��
	UInt64		m_RecoverTime;
	//�ϴη�æʱ��
	UInt64		m_BusyTime;

	//������ڳ���
	UInt32		m_EnterScene;
	//�������λ��
	CLPosition	m_EnterPos;
	UInt64      m_EnterSceneDyncId;

	//����
	ItemMgr		m_ItemMgr;
	//�̵�
	ShopMgr		m_ShopMgr;
	//������cd
	ItemCD		m_ItemCD;
	//������
	RewardGroup	m_RewardGroup;
	//��װЧ��
	ObjUInt32	m_SuitEffect;

	//���
	ObjUInt32	m_Gold;
	//�󶨽��
	ObjUInt32	m_BindGold;
	//��ȯ
	ObjUInt32	m_Point;
	//�󶨵�ȯ
	ObjUInt32	m_BindPoint;
	//�۳��¼
	ObjUInt32	m_TotleChargeNum;
	//�ճ�ֵ��¼
	ObjUInt32   m_DayChargeNum;
	//����
	ObjUInt32	m_Honour;
    //����֮��
    ObjUInt32	m_WarriorSoul;
	//������	
	ObjUInt32	m_PkCoin;
	//��޻���
	ObjUInt32	m_GoldJarPoint;
	//ħ�޻���
	ObjUInt32	m_MagJarPoint;

	//������С
	ObjUInt32	m_PackSize;

	//���������ӳ�
	PackageSize<PACK_MAX> m_PackSizeAddition;

	//�ֿ��С
	ObjUInt8	m_StorageSize;
	//�Ʋ�����ʱ��
	UInt8		m_AssetsLockTime;

	//����������
	UInt32		m_WallowFactor;

	//�����������, noted by aprilliu, ����TaskMgr���и�����
	//huchenhui 2016.06.28 ��ӳɾ�
	CLMaskProperty<3 * MAX_TYPE_TASKNUM> m_TaskMask;
	//����
	TaskMgr			m_TaskMgr;
	//�ճ�����
	DailyTaskMgr	m_DailyTaskMgr;
	//�ɾ�
	AchievementTaskMgr	m_AchievementTaskMgr;
	//�����
	ActivityAttributeMgr m_ActivityAttributeMgr;
	//ѭ������
	CycleTaskMgr	m_CycleTaskMgr;
	//�������
	RedPacketTaskMgr	m_RedPacketTaskMgr;
	//��������
	LegendTaskMgr		m_LegendTaskMgr;
	//ʦ���ճ�����
	MasterDailyTaskMgr		m_MasterDailyTaskMgr;
	//ʦ��ѧҵ�ɳ�����	
	MasterAcademicGrowthTaskMgr m_MasterAcademicGrowthTaskMgr;
	//��ǰ���������������
	UInt32		m_ZoneTask;

	//��������
	CLMaskProperty<MAX_ONCE_STORY_NUM>	m_StoryMask;
	//��ǰ����
	ObjUInt16	m_StoryId;

	//������	by huchenhui
	SkillBar	m_SkillBar;
	//PVP������	by huchenhui
	PvpSkillBar	m_PvpSkillBar;
	//��Ʒ��	by huchenhui
	ItemBar		m_ItemBar;
	//��Ϸѡ��
	ObjInt32	m_Options;

	//����ӳ�BUFF
	Buff*		m_pExpAddBuff;

	//���������
	RequestMgr	m_Requests;

	//����
	Trade*		m_pTrade;

	//����
	Team*		m_pTeam;
	//�Ƿ�ӳ�
	ObjUInt8	m_isTeamMaster;
	//��������
	ObjUInt8	m_TeamSize;
	//���ʱ�����ܶȼ���
	UInt16		m_TeamIntimacyBuff;

	//�����б�
	FriendMap	m_Friends;

	//��ϵ�б�
	std::map<ObjID_t, UInt16>	m_Relations[RELATION_MAX];

	//����
	CounterMgr		m_CounterMgr;
	//��Ϊ����
	ActionCounterMgr	m_ActionCounterMgr;

	//�ճ�
	RoutineMgr	m_RoutineMgr;

	//��Ϊ��ʶ
	ObjUInt32	m_ActionFlag;

	//��������
	InviteMgr		m_InviteMgr;

	//�¿�����ʱ��(s)
	ObjUInt32		m_MonthCardExpireTime;

	//���°汾��
	ObjUInt32		m_UpdateBulletin;

	//Ԥ�湦��  added by aprilliu, ������������Ǽ���Ƿ���Ҫ�������������Զ�ͬ�������̸�ǰ��
	ObjUInt8		m_FuncNotify;
	//���ܿ���λ  �������������Ҳ������������ɺ���м�� (Player::UpdateFuncNotify())
	CLMaskProperty<MAX_FUNCTION_OPEN_NUM>	m_FuncFlag;

	//���Ǻ�������½����
	ObjUInt32		m_ContinuousDays;
	
	//ԭ����
	ObjUInt16		m_SrcZoneId;
	//��������
	ObjUInt16		m_TourZoneId;
	//������
	ObjUInt32		m_BornZoneId;

	// ���װ���̵�
	EquipShopMgr	m_EquipShopMgr;

	//�Ƿ�����
    bool            m_Runing;

    //�������ڳ����е�����
    ObjScenePos     m_ScenePos;

    //�˶�����360�ȣ�
    ObjSceneDir     m_SceneDir;

    //��һ���߶�ʱ��
    Avalon::Time    m_LastMoveTime;

	//SP noted by huchenhui
	ObjUInt32	    m_SP;

	//pvpSP noted by huchenhui
	ObjUInt32		m_PvpSP;

	//����
	ObjUInt8	    m_Awaken;

    // ���³ǹ�����
    DungeonMgr      m_dungeonMgr;

    // ����������
    ObjUInt32       m_pkValue;

    // ƥ�����
    ObjUInt32       m_MatchScore;

	// ������λ
	SeasonLevel		m_SeasonLevel;

	//ƣ��ֵ
	Fatigue			m_Fatigue;

    // ս����¼
    PkStatisticMgr  m_pkStatisticMgr;

    ObjUInt32       m_titleId;

	//��ӹ�����
	RetinueMgr		m_RetinueMgr;

	//���������
	PetMgr			m_PetMgr;

	// ���
	ObjPlayerAvatar	m_avatar;

	// ���ٹ���
	QuickBuyTransaction*	m_quickBuyTrans;

	// �����
	ObjUInt32		m_reviveCoin;

	//VIP
	Vip				m_Vip;

	//����
	CLMaskProperty<MAX_BOOT_FLAG_NUM>	m_BootFlag;

	// �����й�ע������б������ȼ�����
	std::set<PlayerSightPriority>	m_aroundPlayerList;
	std::vector<ObjID_t>			m_watchRoleIdList;
	std::vector<ObjID_t>			m_watchRoleIdSet;

	// �����й�ע������б������ȼ�����
	std::set<ObjID_t>				m_watchRoleList;

	// ��һ��ͬ��λ�õ�ʱ��(ms)
	UInt64							m_LastUpdatePosTime;
	// ��һ�μ���۲��б��ʱ��(ms)
	UInt64							m_LastUpdateWatchListTime;

	//���Խ������޴���
	UInt8	m_chatHiFrqCount;

	// ÿ�ն�ʱ��
	Avalon::DailyTimer	m_DailyTimer;

	// ����֮��ɨ������ʱ��
	ObjUInt32		m_DeathTowerWipeoutEndTime;

	//�����ӹ�����
	OpenJarMgr		m_JarMgr;

	// �������
	// ����ID
	ObjUInt64		m_GuildId;
	// ������
	ObjString		m_GuildName;
	// ְλ
	ObjUInt8		m_GuildPost;
	//���빫��ʱ��
	ObjUInt32		m_GuildEnterTime;
	// ����
	ObjUInt32		m_GuildContri;

	// �����̵�
	ObjUInt8		m_GuildShopId;

	// ���Ὠ����Ϣ
	UInt8			m_GuildBuildingLevel[GUILD_BUILDING_NUM];

	// ����ռ��Ŀ�����
	UInt8			m_GuildOccupyCrossTerrId;

	// ���
	ObjUInt32		m_RedPointFlag;

	//����ս����
	ObjUInt32		m_GuildBattleNumber;

	//����ս����
	ObjUInt32		m_GuildBattleScore;

	//����ս�齱״̬
	ObjUInt8		m_GuildBattleLotteryStatus;

	// ����ռ�
	ObjUInt32			m_GuildEmblem;

	//������ȡ���
	CLMaskProperty<GUILD_BATTLE_REWARD_SIZE> m_GuildBattleRewardMask;

	//ÿ���������
	ObjUInt32		m_DailyTaskScore;

	//�ɾͻ���
	ObjUInt32		m_AchievementScore;

	//ÿ����������ȡ���
	CLMaskProperty<DAILY_TASK_REWARD_SIZE> m_DailyTaskRewardMask;

	//�ɾͽ�����ȡ���
	CLMaskProperty<ACHIEVEMENT_TASK_REWARD_SIZE> m_AchievementTaskRewardMask;

	//������״̬
	ObjUInt8		m_WudaoStatus;

	// ��������һ������
	UInt32			m_WudaoPrevSceneId;
	// ��������һ����������
	CLPosition		m_WudaoPrevScenePos;
	// ��һ������
	Scene*			m_lastScene;

	ObjUInt32		m_NewBoot;

    // ������ɻص�
    std::vector<PlayerLoadingCallback*>    m_LoadingCallback;

	// �û��Զ����ֶ�
	ObjUInt32		m_CustomField;

	//������ˢ��ʱ��
	ObjUInt32		m_AuctionRefreshTime;

	//�����ж��⹺�����λ
	ObjUInt32		m_AuctionAdditionBooth;

	// �쳣���׹�����
	AbnormalTransactionMgr m_AbnormalTransactionMgr;

	bool m_bLogicDayChanged;

	UInt64 m_RefTime;		// �Ը���ҵ�����ʱ��,�����ѯ�������,���߳�ֵ����,�������ӳ���һ�������

	//��һ�����ý���ӻ���ʱ��
	ObjUInt32 m_LastResetGoldJarPointTime;
	//��һ������ħ�޻���ʱ��
	ObjUInt32 m_LastResetMagJarPointTime;

	//��һ�η������ȵ�ʱ��
	UInt32		m_LastSendHornTime;
	//��ǰ������������
	UInt16		m_HornCombo;

	//�̳ǹ����õĵ���
	ObjID_t		m_MallItemId;

	// �Ƿ�����ֻ�
	bool		m_IsPhoneBind;

	// ����֪ͨ
	//ReadNotifyProperty	m_readNotify;

	// ҩˮ����
	DungeonPotionSet<3>	m_potionSet;

	// �ɵ���ҵȼ������ڴ��������жϣ�
	UInt16				m_OldLevel;

	// �Ƿ������������������ݿ�
	UInt8 m_ConditionActivateDBLoadState;

	//װ����������͵�װ������Ұ󶨵ĵȼ���
	std::vector<PlayerLevel> m_EquipBindPlayerLevel;

	
	bool m_bFirstTimeItemTick;

	//�Ƿ��Ѿ����SP
	ObjUInt8 m_IsCheckSp;

	//PVP���ܹ���
	PvpSkillMgr m_PvpSkillMgr;

	// ʱװ�ϳ�������������Ϣ
	ObjUInt64 m_FashionSkyGetBit;

	//����ID
	ObjUInt32 m_RoomId;

	//ͬ���˺�����ʱ��Ķ�ʱ��
	Avalon::SimpleTimer		m_SyncAddictTimer;

	//������
	WeaponBar		m_WeaponBar;

	//ԤԼ��ɫ
	ObjUInt8		m_AppointmentOccu;

	//�������������ID
	UInt32			m_CrossSceneNodeId;
	//ԭ����������ID
	UInt32			m_SrcSceneNodeId;

	//ս������������ID
	UInt32			m_BattleSceneNodeId;

	//�Ƿ�֪ͨ�г���(client,gate)
	bool			m_NotifyChangeScene;

	//��ɫ���״̬
	ObjUInt8		m_MoneyManageStatus;

	//�˺����״̬
	UInt8			m_MoneyManageAccountStatus;

	//��ƹ�����
	CLMaskProperty<MONEY_MANAGE_REWARD_SIZE> m_MoneyManageRewardMask;

	//����������
	ObjUInt32		m_ScoreWarScore;
	//����������������
	CLMaskProperty<SCORE_WAR_REWARD_SIZE> m_ScoreWarRewardMask;
	//������ս������
	ObjUInt8		m_ScoreWarBattleCount;
	//������ս��ʤ������
	ObjUInt8		m_ScoreWarWinBattleCount;
	//���������ս������(����ˢ�»�������Ϣ)
	ObjUInt32		m_ScoreWarLastBattleTime;

	//�Ƿ���loading��(�ͻ��˼��س���)
	bool			m_IsClientLoading;

	//ʦͽ�ʾ�
	//����ʱ������
	UInt8			m_activeTimeType;
	//ʦ������
	UInt8			m_masterType;
	//����id
	UInt8			m_regionId;
	//����
	std::string		m_declaration;
	
	//ʦͽ�ɳ�ֵ��ֵ
	ObjUInt32		m_AcademicTotalGrowth;
	//ʦ���ճ�����ɳ���ֵ
	ObjUInt32		m_MasterDailyTaskGrowth;
	//ʦ��ѧҵ�ɳ���ֵ
	ObjUInt32		m_MasterAcademicTaskGrowth;
	//ʦ������װ���ɳ���ֵ
	ObjUInt32		m_MasterGiveEquipGrowth;
	//ʦ�������ɳ���ֵ
	ObjUInt32		m_MasterUplevelGrowth;
	//ʦ����������ɳ���ֵ
	ObjUInt32		m_MasterGiveGiftGrowth;
	//ʦͽ���ͨ�����³ǳɳ�ֵ
	ObjUInt32		m_MasterTeamClearDungeonGrowth;
	//��ʦֵ
	ObjUInt32		m_GoodTeacherValue;
	//��ʦͽ������
	ObjUInt32		m_FinSchDiscipleSum;

	//��ħֵ
	UInt32			m_ResistMagic;

	//���˻
	PlayerActivityInfo<Player> m_PlayerActivityInfo;

	// ��ȫ��״̬
	UInt32		m_SecurityStatus;

	// ����ʱװ��ʾѡ��
	ObjUInt8    m_ShowFashionWeapon;

	// �������޺��˷�
	ObjUInt32		m_weaponLeaseTickets;
	// ð�նӶ���
	ObjString			m_AdventureTeamName;
	// ��ɫ����
	ObjUInt32			m_RoleValueScore;
	// ��Ϸ����
	ObjString	m_GameSet;
	// ��Ϸ����map
	std::map<UInt32, std::string> m_GameSetMap;

	// ��ɫ�ع�ʱ��
	UInt32				m_RoleReturnTime;
	// ��ɫ�ع�ȼ�
	UInt32				m_RoleReturnLevel;
	// �Ƿ��ٴλع�
	UInt8				m_AgainReturn;
	// ������
	UInt32				m_EnergyCoin;

	// ð�նӵȼ�
	UInt16				m_AdventureTeamLevel;
	// ð�ն�����
	UInt8				m_AdventureTeamGrade;


	// �������ʹ�С
	ObjString			m_PackageTypeStr;
	// ��������map
	std::map<UInt32, UInt32> m_PackageTypeSizeMap;

	// ͷ���
	HeadFrameMgr		m_HeadFrameMgr;
	// ��ǰͷ���
	ObjUInt32				m_HeadFrameId;

	// ͷ������
	ObjString			m_NewTitleName;

	// ͷ��GUID
	ObjUInt64				m_NewTitleGuid;
	// �Լ�ר������
	// �Լ�ʣ��Ѫ��(ǧ�ֱ�)
	ObjInt32					m_ChijiHp;
	// �Լ�ʣ������(ǧ�ֱ�)
	ObjInt32					m_ChijiMp;
	// �Լ�����
	ObjInt32					m_ChijiScore;
	// �Լ���������ʱ��
	ObjInt64					m_ChijiScoreResetTime;
	// �Լ���ս����id
	UInt64						m_ChijiNoWarEnemyId;
	// �Լ���սս��id
	UInt32						m_ChijiNoWarBattleId;
	// �Լ���սѡ��ʱ��
	UInt32						m_ChijiNoWarChoiceTime;
	// �Լ�pk���³�
	UInt32						m_ChijiPKDungeonId;
	// �Լ������б�
	std::vector<UInt32>			m_ChijiOccuVec;

	SpMgr m_NetSp;

public:
	// �Լ�ս��ID
	UInt32					m_BattleID;
	Avalon::SimpleTimer		m_CheckBattleTimer;

#ifdef _DEBUG
	//ǿ�����ʲ���
	UInt8	m_testEquipStrengthFlag;
	//�ϳ�����׸��ʲ���
	UInt8	m_testHappySky;
#endif
};

/**
 *@brief ��ҷ�����
 */
typedef CLVisitor<Player> PlayerVisitor;

#endif

