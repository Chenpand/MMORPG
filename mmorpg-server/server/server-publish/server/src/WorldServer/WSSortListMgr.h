#ifndef _WS_SORTLIST_MGR_H_
#define _WS_SORTLIST_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLRecordCallback.h>
#include <CLSortListDefine.h>
#include <set>

class WSPlayer;

struct WSSortListRewardEntry
{
	WSSortListRewardEntry() :playerId(0), ranking(0), value(0){}

	UInt64 playerId;
	UInt32 ranking;
	UInt32 value;
};

class WSSortListGiveReward
{
public:
	WSSortListGiveReward(){}
	~WSSortListGiveReward(){}

	void Init(SortListType type, Avalon::Time now, UInt32 intervalTime);

	void Init(SortListType type, Avalon::Time now, UInt32 intervalTime, std::string sendName, std::string title, std::string content, std::string reason);

public:
	inline std::string& GetSendName() { return m_SendName; }
	inline std::string& GetTitle() { return m_Title; }
	inline std::string& GetContent() { return m_Content; }
	inline std::string& GetReason() { return m_Reason; }

	inline bool IsLoadMailInfo(){ return m_SendName.empty() || m_Title.empty() || m_Content.empty(); }

	inline bool IsContinue() { return m_Index < m_Entry.size(); }
	inline void IncIndex() { m_Index += 1; }
	inline UInt32 GetIndex() { return m_Index; }

	inline UInt32 GetLastGiveTime() { return m_LastGiveTime; }
	inline void SetLastGiveTime(UInt32 lastGiveTime) { m_LastGiveTime = lastGiveTime; }

	inline UInt32 GetSingleSize() { return m_SingleSize; }

	inline UInt32 GetInterval() { return m_Interval; }

	bool GetEntry(WSSortListRewardEntry& entry) { return GetEntry(m_Index, entry); }

private:
	bool GetEntry(UInt32 index, WSSortListRewardEntry& entry);

private:
	UInt32 m_Index;

	std::vector<WSSortListRewardEntry> m_Entry;
	//时间间隔
	UInt32 m_Interval;
	//单次执行数量
	UInt32 m_SingleSize;
	//最后执行时间
	UInt32 m_LastGiveTime;
	
	std::string m_SendName;
	std::string m_Title;
	std::string m_Content;
	std::string m_Reason;
};

/**
 *@brief 排行榜访问器
 */
class WSSortListVisitor
{
public:
	virtual ~WSSortListVisitor(){}

	virtual bool operator()(SortListEntry& entry, UInt32 ranking) = 0;
};

/**
 *@brief 排行榜
 */
class WSSortListMgr : public Avalon::Singleton<WSSortListMgr>
{
	friend class SelectSortDataCallback;

	typedef std::list<SortListEntry>	SortEntryList;

public:
	WSSortListMgr();
	~WSSortListMgr();

public:
	void Init();
	void Final();

	bool ReloadBlackRoleList(std::string strs);

public:
	/**
	 *@brief 更新排名
	 */
	void UpdateSortList(SortListEntry& entry);

	/**
	 *@brief 更新宠物名
	 */
	void UpdatePetName(ObjID_t id, const std::string& name);

	/**
	 *@brief 列出排行榜
	 */
	void ListSortEntries(WSPlayer* player, UInt8 sortType, UInt8 occu, UInt16 start, UInt16 count);

	/**
	 *@brief 列出自身排名相近的项                                                                     
	 */
	void ListSortEntriesNear(WSPlayer* player, UInt8 sortType, UInt32 count);

	/**
	*@brief 同步玩家排名
	*/
	void SyncPlayerEntry(WSPlayer* player, UInt8 sortType);

	/**
	*@brief 同步玩家排名
	*/
	void SyncPlayerEntry(WSPlayer* player, SortListEntry entry, UInt16 ranking);

	/**
	 *@brief 删除排序项
	 */
	void DeleteEntry(UInt8 sortType, ObjID_t id);

	/**
	*@brief 根据ID删除排序项
	*/
	void DeleteEntryById(UInt8 sortType, ObjID_t itemId);

	/**
	*@brief 删除所有排序项
	*/
	void DeleteAllEntry(ObjID_t id);

	/**
	 *@brief 观察数据
	 */
	void WatchData(WSPlayer* player, UInt8 sortType, ObjID_t id);

	/**
	 *@brief 遍历排行榜
	 */
	void VisitSortList(UInt8 sortType, WSSortListVisitor& visitor);

	/** 
	 *@brief 返回名次
	 */
	UInt32 GetRanking(UInt8 sortType, ObjID_t id) const;

	/** 
	 *@brief 清空排行榜
	 */
	void ClearSortList(UInt8 sortType);

public://事件
	/**
	 *@brief 场景服务器建立连接
	 */
	void OnSceneConnected(UInt32 id);

	/**
	 *@brief tick
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief 跨天
	 */
	void OnDayChanged();

	/**
	 *@brief 跨周
	 */
	void OnWeekChanged();

	/**
	 *@brief 玩家上线
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief vip等级改变
	 */
	void OnVipLvlChanged(WSPlayer* player);

	/**
	 *@brief 人物等级改变
	 */
	void OnPlayerLevelChanged(WSPlayer* player);

	/**
	 *@brief 排行榜发生变化，数值变化也算
	 */
	void OnSortListChanged(UInt8 type);

	/**
	 *@brief 改名
	 */
	void OnNameChanged(ObjID_t playerId, const std::string& name);

	/**
	*@brief 拥有者改变
	*/
	void OnOwnerChanged(SortListType sortType, ObjID_t entryId, ObjID_t playerId, const std::string& name);

	/**
	*@brief 转职
	*/
	void OnOccuChanged(WSPlayer* player);

	/**
	*@brief 修改公会leader
	*/
	void OnGuildLeaderChanged(ObjID_t guildId, ObjID_t leaderId, std::string leader);

	/**
	*@brief 公会成员变动
	*/
	void OnGuildMemeberChanged(ObjID_t guildId, UInt32 memberCount);

	/**
	*@brief 公会等级变动
	*/
	void OnGuildLevelChanged(ObjID_t guildId, UInt32 level);

	/**
	*@brief 公会名变更
	*/
	void OnGuildNameChanged(ObjID_t guildId, std::string name);

	/**
	*@brief 修改value3，但是不重新排序
	*/
	void OnUpdateValue3(SortListType sortType, ObjID_t roleId, UInt32 value3);

	/**
	*@brief 冒险队等级改变
	*/
	void OnAdventureTeamLevelChanged(SortListType sortType, ObjID_t uid, UInt16 level);

	/**
	*@brief 冒险队名改变
	*/
	void OnAdventureTeamNameChanged(SortListType sortType, ObjID_t uid, const std::string& name);

	/**
	*@brief 冒险队评级改变
	*/
	void OnAdventureTeamGradeChanged(SortListType sortType, ObjID_t uid, UInt8 grade);

	//测试发放每日奖励
	void TestGiveReward();

	SortListEntry* FindEntry(UInt8 sortType, ObjID_t id);

private:
	bool OnSelectDataRet(CLRecordCallback* callback);

	void AddToList(SortListEntry& entry);

	bool IsInBlackRoleList(ObjID_t id);

	UInt64 GetMinSortValue(UInt8 sortType) const;

	void DeleteEntry(UInt8 sortType, SortEntryList::iterator iter);

	void Save();

	void Save(SortListEntry& entry);

	//void GiveReward();

private:
	// 不同排行榜特殊处理部分

	// 赛季重置
	void OnResetSeasonMatch();

	//赛季每日奖励
	void GiveRewardMatchDaily(const Avalon::Time& now);

public:
	// 赛季积分重置规则
	static UInt32 GetResetMatchScore(UInt32 oldScore);

	static void GrantSeasonMatchAward(ObjID_t roleId, UInt32 rank, UInt32 totalRank);

	std::vector<UInt64> GetSeasonPlayerIds(UInt32 size);

public:
	//新年红包结算
	void OnNewYearRedPacketEnd();

	bool CheckGiveAdventureSortReward();

	void GiveAdventureSortRewards();
private:
	//排行榜数据
	SortEntryList	m_SortEntrys[SORTLIST_MAX];

	//定时保存
	Avalon::SimpleTimer	m_SaveTimer;

	UInt64				m_reopen1v1SeasonTime;

	//1v1每日奖励 分批发放
	WSSortListGiveReward m_1v1DailyReward;

	//黑名单
	std::set<ObjID_t> m_BlackRoleList;

};

#endif
