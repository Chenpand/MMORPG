#ifndef _WEEK_SIGN_MGR_H_
#define _WEEK_SIGN_MGR_H_

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLItemDefine.h>

struct ActRefresh
{
	ActRefresh() : actId(0), refreshTime(0) {}
	ActRefresh(UInt32 id, UInt64 time) : actId(id), refreshTime(time) {}

	// 活动ID
	UInt32 actId;
	// 刷新时间
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

	// 设置活动开关
	void SetActivityStatus(UInt32 actType, UInt32 actId, UInt32 actStatus);

	// 加载活动id
	bool LoadActData(CLRecordCallback* callback);
	// 加载活动抽取次数
	bool LoadServerActItemData(CLRecordCallback* callback);
	// 加载个人抽取次数
	bool LoadPlayerActItemData(CLRecordCallback* callback);
	// 加载周签到抽奖记录
	bool LoadWeekSignRecord(CLRecordCallback* callback);

	// 抽奖
	void HandleWeekSignReward(GASPlayer* player, UInt32 actType, UInt32 actId, UInt32 vipLvl);
	// 获取奖励记录
	void HandleWeekSignRecord(GASPlayer* player, UInt32 actType);

private:

	// 增加全服抽取次数
	void _AddServerItemData(ActItem& actItem, UInt32 num, bool _isSave = false);
	// 增加玩家抽取次数
	void _AddPlayerItemData(const std::string& playerId, ActItem& actItem, UInt32 num, bool _isSave = false);
	// 增加抽奖记录
	void _AddWeekSignRecord(UInt32 actType, CLProtocol::WeekSignRecord& record);

	// 抽个垃圾物品
	void _LotteryTrashItem(GASPlayer* player, UInt32 actType, ItemRewardVec& rewardVec);
	// 获取全服已经抽取次数
	UInt32 _GetServerItemCnt(UInt32 actType, UInt32 itemUid);
	// 获取玩家已抽取次数
	UInt32 _GetPlayerItemCnt(const std::string& playerId, UInt32 actType, UInt32 itemUid);
	// 公告
	void _PublicWeekSign(UInt32 actType, UInt32 notifyId, UInt32 superLink, const std::string& serverName, const std::string& playerName, const std::string& itemName);
	// 存记录
	void _RecordWeekSign(GASPlayer* player, UInt32 actType, const WeekSignItem& signItem);
	// 刷新抽取次数
	void _RefreshItemCnt(UInt32 actType, UInt32 refreshType);

	// 存库
	void _InsertWeekSignRecord(UInt32 actType, CLProtocol::WeekSignRecord& record);
	void _InsertServerItem(ActItem& actItem, UInt32 actNum);
	void _InsertPlayerItem(const std::string& playerId, ActItem& actItem, UInt32 actNum);

	// 更新
	void _UpdateServerItem(const ActItem& actItem, UInt32 actNum);
	void _UpdatePlayerItem(const std::string& playerId, const ActItem& actItem, UInt32 actNum);

	// 清除数据
	void _DelItemData(UInt32 actType, const std::string& tableName);
	void _DelDBData(UInt32 actType, const std::string& tableName);
	void _DelWeekSignRecord(UInt64 guid);
	void _SaveActId(UInt32 actType, ActRefresh& actFr, bool _isInsert = false);

private:

	typedef std::map<ActItem, UInt32> ItemNumMap;
	typedef std::map<std::string, ItemNumMap> PlayerItemNumMap;

	typedef std::vector<CLProtocol::WeekSignRecord> WeekSignRecordVec;
	typedef std::map<UInt32, WeekSignRecordVec> WeekSignRecordMap;

	// 当前(key->活动类型)
	std::map<UInt32, ActRefresh> m_ActMap;
	// 全服活动道具抽取次数(key->活动类型)
	ItemNumMap m_ServerItemMap;
	// 个人道具抽取次数(key->playerId)
	PlayerItemNumMap m_PlayerItemMap;
	// 周签到抽奖记录(key->活动类型)
	WeekSignRecordMap m_WeekSignRecordMap;
};

#endif
