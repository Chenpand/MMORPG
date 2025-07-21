#ifndef _WS_RELATION_MGR_H_
#define _WS_RELATION_MGR_H_

#include <AvalonSimpleTimer.h>
#include "WSRelation.h"
#include "WSApplication.h"
#include <functional>

class WSPlayer;
class CLRecordCallback;
namespace CLProtocol{
	class SceneSyncChat;
};

inline UInt64 GetUSecsNow()
{
#if defined(AVALON_OS_WINDOWS)
	return 0;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (UInt64)tv.tv_sec * 1000000 + (UInt64)tv.tv_usec;
#endif
}

class UpdateRelationDebugLog
{
public:
	UpdateRelationDebugLog(const char* info) : m_info(info)
	{
		if (WSApplication::Instance()->IsCheckUpDateRelation())
		{
			m_startTime = GetUSecsNow();
			//DebugLogStream << "begin invoke  WSRelationMgr::UpdateRelation form " << m_info.c_str() << LogStream::eos;
		}
		
	}

	~UpdateRelationDebugLog()
	{
		if (WSApplication::Instance()->IsCheckUpDateRelation())
		{
			UInt64 endTime = GetUSecsNow();
			DebugLogStream << "end invoke  WSRelationMgr::UpdateRelation in " << m_info.c_str() << " used time = " << endTime - m_startTime << LogStream::eos;
		}
	}
protected:
private:
	std::string m_info;
	UInt64	m_startTime;
};

/**
*@brief ȫ�ֹ�ϵ������
*/
class GlobalRelationMgr : public Avalon::Singleton<GlobalRelationMgr>
{
public:
	GlobalRelationMgr();
	~GlobalRelationMgr();

	void RemoveAllRelation(ObjID_t roleUid);
};

/**
*@brief ȫ�ִ�������������
*/
class AddonPayOrderMgr : public Avalon::Singleton<AddonPayOrderMgr>
{
public:
	AddonPayOrderMgr();
	~AddonPayOrderMgr();

	AddonPayOrderData* GetOrder(UInt64 id);
	void AddOrder(AddonPayOrderData* data);
	void GetReqOrders(UInt64 reqId, std::vector<AddonPayOrderData*>& outDatas);
	void GetRepOrders(UInt64 repId, std::vector<AddonPayOrderData*>& outDatas);
private:

	std::map<UInt64, AddonPayOrderData*> m_AddonPayDatas;
	std::map<UInt64, std::vector<AddonPayOrderData*>>	m_RequestDatas;
	std::map<UInt64, std::vector<AddonPayOrderData*>>	m_ReplyDatas;
};

/** 
 *@brief ��ϵ������
 */
class WSRelationMgr 
{
	typedef std::map<ObjID_t, WSRelation*>	RelationMap;
	typedef std::vector<ObjID_t> IdVec;

	//���ɱ������
	const static UInt32	MAX_KILL_TIMES = 999;
	
public:
	WSRelationMgr();
	~WSRelationMgr();

public:
	/**
	 *@breif ���û�ȡӵ����
	 */
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const{ return m_pOwner; }

	/**
	 *@brief �ж�ĳ�����͵Ĺ�ϵ�Ƿ�����
	 */
	bool IsFull(UInt8 type) const;

	/**
	 *@brief ���ҡ��Ƴ������ӹ�ϵ
	 */
	WSRelation* FindRelation(UInt8 type, ObjID_t id);
	WSRelation* FindRelation(UInt8 type, const std::string& name);
	// �˷��������relation��
	void RemoveRelation(WSRelation* relation);
	bool RemoveRelation(UInt8 type, ObjID_t id);
	bool AddRelation(WSRelation* relation);
	bool HasRelation(ObjID_t id);
	bool HasRelation(UInt8 type);
	WSRelation* GetRelation(ObjID_t id);

	WSRelation* GetRelation(UInt8 type);
	bool ChangeToStranger(UInt8 type, ObjID_t id);
	bool AddFriend(UInt8 type, ObjID_t id);
	void UpdateRelation(ObjID_t uid, WSRelation* oldRelation, UInt8 newType);

	void RemoveAllRelations();
	void OnDeletePlayer(WSRelation* relation);

	/**
	 *@brief ����һ�����͵Ĺ�ϵ
	 */
	void VisitRelations(UInt8 type, RelationVisitor& visitor);
	void VisitRelations(RelationVisitor& visitor);

	void VisitRelationsNew(UInt8 type, const std::function<bool(WSRelation*)>& ff);
	/**
	 *@brief �������й�ϵ
	 */
	void LoadRelations();

	/**
	 *@brief ͬ����ϵ�б�����
	 */
	void SyncRelationListToScene();

	/**
	*@brief ��ѯ���ѵĺ�Ժ��������
	*/
	//void QueryBKOpRecord(UInt16 uStartNo, UInt16 uNum, Avalon::NetOutputStream &stream);

	//���ú��ѹ�ϵÿ����������
	void ResetFriendDayGiftNum();

	//���¹�ϵ
	void UpdateRelation();

	void SetMasterNote(std::string note, bool syncDB = false);
	inline std::string GetMasterNote()	{ return m_MasterNote; }

	void SetIsRecvDisciple(bool isRecv, bool syncDB = false);
	inline bool GetIsRecvDisciple()	{ return m_IsRecvDisciple; }

public: //�¼�
	/**
	 *@brief �������ݷ���
	 */
	void OnLoadDataRet(CLRecordCallback* callback);

	/**
	 *@brief �������
	 */
	void OnOnline(bool dayChanged, bool isReconnect = false);

	//�߼��ո�
	void OnLogicDayChange();

	/**
	 *@brief �������
	 */
	void OnOffline();

	/**
	 *@brief ��ɱ
	 */
	void OnKilled(WSPlayer* killer);

	/**
	 *@brief �ȼ��ı�
	 */
	void OnLevelChanged();

	/**
	*@brief vip�ȼ��ı�
	*/
	void OnVipLvlChanged();

	/**
	*@brief ְҵ�ı�
	*/
	void OnOccuChanged();

	/**
	*@brief ��λ�ı�
	*/
	void OnSeasonLvChanged();

	/**
	 *@brief �л�����������
	 */
	void OnServerChanged();

	/**
	*@brief ս�����ı�
	*/
	void OnPowerChanged();

	/**
	*@brief ״̬�ı�
	*/
	void OnStatusChanged(UInt8 status);

	/**
	*@brief ͷ���ı�
	*/
	void OnHeadFrameChanged();

	/**
	*@brief ����ı�
	*/
	void OnGuildChanged();

	/**
	*@brief �ع�����ƺ�
	*/
	void OnReturnYearTitle();
	
	/**
	 *@brief ����
	 */
	void OnHeartbeat(const Avalon::Time& now);
	void FiveMinHb();

	/**
	 *@brief ���ָı�
	 */
	static void OnNameChanged(ObjID_t playerId, const std::string& name);

	/**
	 *@brief �Ա�ı�                                                                     
	 */
	static void OnSexChanged(ObjID_t playerId, UInt8 sex);

	/**
	*@brief ��ϵ�ı�
	*/
	void OnRelationTypeChanged();

	/**
	*@brief ͬ����ϵ�����ݿ�
	*/
	static void SyncRelationTypeToDB(ObjID_t owner, ObjID_t tar, UInt8 type);

	/**
	*@brief ����ϵ���ȼ�
	*/
	bool CheckRelationTypePriority(RelationType typeNew, RelationType typeOld);

	/**
	*@brief ����ϵ���ŵȼ�
	*/
	bool CheckRelationOpenLv(RelationType type);

	//�����ϵ���
	void OnRequestPlayer(RelationType type, ObjID_t tarId);

	//����������
	void ClearRequestPlayer(RelationType type);

	//�����������
	bool FindRequestPlayer(RelationType type, ObjID_t tarId);

	/**
	*@������Ӻ���
	*/
	void OnAddFriend(WSRelation* relation);

	/**
	*@brief �����ɻع���� bRegress [false]:���ǻع���ң�[true]�ǻع����
	*/
	void OnChangeRegression(bool bRegress);

public:	// ��Ϣ��
	/**
	*@brief ͬ�����
	*/
	void SendNotifyDelBlackList(ObjID_t myId, ObjID_t tarId);
	
	void SendPlayerDelRelation(RelationType type, ObjID_t tarId);
	/**
	*@brief ͬ������
	*/
	void SendNotifyNewRelation(RelationType type, ObjID_t tarId);
	void SendNotifySceneDel(ObjID_t myId, RelationType type, ObjID_t tarId);

public:	// ���ܸ�
	void RequestAddonPayData(UInt32 goodId, UInt64 tarId);

	// ͬ��������Ϣ�����
	void SyncAddonPayData(AddonPayOrderData* order);

	// ͬ�����д�����Ϣ�����
	void SyncAllAddonPayDatas(std::vector<AddonPayOrderData*>& orders);

private: // ���ܸ�
	bool TransOrderToNetData(AddonPayOrderData* order, AddonPayData& netData);

private:
	
	/**
	 *@brief ͬ�������ݿ�
	 */
	void SyncToDB();

	/**
	 *@brief ͬ���ȼ������ݿ�
	 */
	static void SyncLevelToDB(ObjID_t playerId, UInt16 level);

	/**
	*@brief ͬ��ͷ������ݿ�
	*/
	static void SyncHeadFrameToDB(ObjID_t playerId, UInt32 id);

	/**
	*@brief ͬ������ID�����ݿ�
	*/
	static void SyncGuildIdToDB(ObjID_t playerId, UInt64 id);

	/**
	*@brief ͬ������ID�����ݿ�
	*/
	static void SyncReturnYearTitleToDB(ObjID_t playerId, UInt32 title);

	/**
	*@brief ͬ��vip�ȼ������ݿ�
	*/
	static void SyncVipLevelToDB(ObjID_t playerId, UInt16 vipLvl);

	/**
	*@brief ͬ��ְҵ�����ݿ�
	*/
	static void SyncOccuToDB(ObjID_t playerId, UInt8 occu);

	/**
	*@brief ͬ����λ�����ݿ�
	*/
	static void SyncSeasonLvToDB(ObjID_t playerId, UInt32	seasonLv);

	/**
	 *@brief ͬ�����ֵ����ݿ�
	 */
	static void SyncNameToDB(ObjID_t playerId, const std::string& name);

	/**
	 *@brief ͬ���Ա����ݿ�                                                                     
	 */
	static void SyncSexToDB(ObjID_t playerId, UInt8 sex);


	/**
	*@brief ͬ��qq vip�ȼ������ݿ�
	*/
	static void SyncQQvipToDB(ObjID_t playerId, UInt8 qqVipLvl, UInt8 bSuperBlue);

	/**
	*@brief ����ף���ȼ�����
	*@param ��ҵȼ�
	*@return �Ƿ����� true:�� false:��
	*/
	bool IsBlessLevelLimit(UInt16 level);

	/**
	*@brief  ͬ�����ѻع��ֶ�
	*/
	static void SyncRegressionMarkToDB(ObjID_t playerId, UInt8 IsRegress);

	/**
	*@brief  ͬ������ʱ��
	*/
	static void SyncOfflineTimeToDB(ObjID_t playerId, UInt32 offlineTime);
public:
	/**
	*@brief �ж�������Ƿ�Ϊ����
	*/
	static bool IsMutualFriend(WSPlayer* player1, WSPlayer* player2);

	/**
	*@brief �ж�������Ƿ���ʦͽ
	*/
	static bool IsMasterDisciple(WSPlayer* player1, WSPlayer* player2);

	/**
	*@brief �޸�ʦ��ÿ��״̬����
	*/
	static void SyncMasterDailyTaskState(ObjID_t owner, ObjID_t tar, UInt8 type, UInt8 state);
	
	/**
	*@brief ����ʦ��ÿ������״̬
	*/
	void UpdateMasterDailyTaskStateOnLogicDayChange();

	/**
	*#brief �޸Ĺ�ϵ
	*/
	static void UpdateRelationTypeToDb(ObjID_t owner, UInt8 type, ObjID_t id, UInt8 newType);

	/**
	*@brief  ���ñ�ע
	*/
	UInt32 SetPlayerRemark(ObjID_t roleId, std::string& remark);

	/**
	*@brief ֪ͨ���ѣ�ʦͽ �һع���
	*/
	void NotifyRelationReturn();
	void NotifyRelationReturnMsg(UInt32 type, CLProtocol::SceneSyncChat& chat);

	/**
	*@brief ��ȡʦ������
	*/
	std::string GetMasterName();
private:
	//ӵ����
	WSPlayer*	m_pOwner;
	//��ϵ�б�
	RelationMap	m_Relations[RELATION_MAX];
	//�򵥶�ʱ��
	Avalon::SimpleTimer	m_SyncDBTimer;
	bool		m_bDayGiftChxsanged;
	//ʦ������
	std::string	m_MasterNote;
	//�Ƿ���ͽ
	bool		m_IsRecvDisciple;
	//������˴����б�
	std::vector<AddonPayData>	m_RequestAddonPayData;
	//���������Լ������б�
	std::vector<AddonPayData>	m_ReplyAddonPayData;
	//���������� 5������һ��
	std::set<ObjID_t>	m_RequstedPlayerID[RelationType::RELATION_MAX];
};

#endif
