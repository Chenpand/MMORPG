#ifndef _WEEK_SIGN_MGR_H_
#define _WEEK_SIGN_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLItemDefine.h>

struct ActRefresh
{
	ActRefresh() : actId(0), refreshTime(0) {}
	ActRefresh(UInt32 id, UInt64 time) : actId(id), refreshTime(time) {}

	// �ID
	UInt32 actId;
	// ˢ��ʱ��
	UInt64 refreshTime;
};

class CLRecordCallback;
class GASPlayer;
struct WeekSignItem;
struct ActItem;
namespace CLProtocol{
	struct WeekSignRecord;
	class GASWeekSignLotteryReq;
}

class WeekSignMgr : public Avalon::Singleton<WeekSignMgr>
{

public:
	WeekSignMgr();
	~WeekSignMgr();

	void OnInit();
	void OnTick(const Avalon::Time& now);

	// ���û����
	void SetActivityStatus(UInt32 actType, UInt32 actId, UInt32 actStatus);

	// ���ػid
	bool LoadActData(CLRecordCallback* callback);
	// ���ػ��ȡ����
	bool LoadServerActItemData(CLRecordCallback* callback);
	// ���ظ��˳�ȡ����
	bool LoadPlayerActItemData(CLRecordCallback* callback);
	// ������ǩ���齱��¼
	bool LoadWeekSignRecord(CLRecordCallback* callback);

	// �齱
	void HandleWeekSignReward(GASPlayer* player, UInt32 actType, UInt32 actId, UInt32 vipLvl);
	// ��ȡ������¼
	void HandleWeekSignRecord(GASPlayer* player, UInt32 actType);

private:

	// ����ȫ����ȡ����
	void _AddServerItemData(ActItem& actItem, UInt32 num, bool _isSave = false);
	// ������ҳ�ȡ����
	void _AddPlayerItemData(const std::string& playerId, ActItem& actItem, UInt32 num, bool _isSave = false);
	// ���ӳ齱��¼
	void _AddWeekSignRecord(UInt32 actType, CLProtocol::WeekSignRecord& record);

	// ���������Ʒ
	void _LotteryTrashItem(GASPlayer* player, UInt32 actType, ItemRewardVec& rewardVec);
	// ��ȡȫ���Ѿ���ȡ����
	UInt32 _GetServerItemCnt(UInt32 actType, UInt32 itemUid);
	// ��ȡ����ѳ�ȡ����
	UInt32 _GetPlayerItemCnt(const std::string& playerId, UInt32 actType, UInt32 itemUid);
	// ����
	void _PublicWeekSign(UInt32 actType, UInt32 notifyId, UInt32 superLink, const std::string& serverName, const std::string& playerName, const std::string& itemName);
	// ���¼
	void _RecordWeekSign(GASPlayer* player, UInt32 actType, const WeekSignItem& signItem);
	// ˢ�³�ȡ����
	void _RefreshItemCnt(UInt32 actType, UInt32 refreshType);

	// ���
	void _InsertWeekSignRecord(UInt32 actType, CLProtocol::WeekSignRecord& record);
	void _InsertServerItem(ActItem& actItem, UInt32 actNum);
	void _InsertPlayerItem(const std::string& playerId, ActItem& actItem, UInt32 actNum);

	// ����
	void _UpdateServerItem(const ActItem& actItem, UInt32 actNum);
	void _UpdatePlayerItem(const std::string& playerId, const ActItem& actItem, UInt32 actNum);

	// �������
	void _DelItemData(UInt32 actType, const std::string& tableName);
	void _DelDBData(UInt32 actType, const std::string& tableName);
	void _DelWeekSignRecord(UInt64 guid);
	void _SaveActId(UInt32 actType, ActRefresh& actFr, bool _isInsert = false);

private:

	typedef std::map<ActItem, UInt32> ItemNumMap;
	typedef std::map<std::string, ItemNumMap> PlayerItemNumMap;

	typedef std::vector<CLProtocol::WeekSignRecord> WeekSignRecordVec;
	typedef std::map<UInt32, WeekSignRecordVec> WeekSignRecordMap;

	// ��ǰ(key->�����)
	std::map<UInt32, ActRefresh> m_ActMap;
	// ȫ������߳�ȡ����(key->�����)
	ItemNumMap m_ServerItemMap;
	// ���˵��߳�ȡ����(key->playerId)
	PlayerItemNumMap m_PlayerItemMap;
	// ��ǩ���齱��¼(key->�����)
	WeekSignRecordMap m_WeekSignRecordMap;
};

#endif
