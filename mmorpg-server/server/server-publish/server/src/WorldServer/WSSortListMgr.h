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
	//ʱ����
	UInt32 m_Interval;
	//����ִ������
	UInt32 m_SingleSize;
	//���ִ��ʱ��
	UInt32 m_LastGiveTime;
	
	std::string m_SendName;
	std::string m_Title;
	std::string m_Content;
	std::string m_Reason;
};

/**
 *@brief ���а������
 */
class WSSortListVisitor
{
public:
	virtual ~WSSortListVisitor(){}

	virtual bool operator()(SortListEntry& entry, UInt32 ranking) = 0;
};

/**
 *@brief ���а�
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
	 *@brief ��������
	 */
	void UpdateSortList(SortListEntry& entry);

	/**
	 *@brief ���³�����
	 */
	void UpdatePetName(ObjID_t id, const std::string& name);

	/**
	 *@brief �г����а�
	 */
	void ListSortEntries(WSPlayer* player, UInt8 sortType, UInt8 occu, UInt16 start, UInt16 count);

	/**
	 *@brief �г����������������                                                                     
	 */
	void ListSortEntriesNear(WSPlayer* player, UInt8 sortType, UInt32 count);

	/**
	*@brief ͬ���������
	*/
	void SyncPlayerEntry(WSPlayer* player, UInt8 sortType);

	/**
	*@brief ͬ���������
	*/
	void SyncPlayerEntry(WSPlayer* player, SortListEntry entry, UInt16 ranking);

	/**
	 *@brief ɾ��������
	 */
	void DeleteEntry(UInt8 sortType, ObjID_t id);

	/**
	*@brief ����IDɾ��������
	*/
	void DeleteEntryById(UInt8 sortType, ObjID_t itemId);

	/**
	*@brief ɾ������������
	*/
	void DeleteAllEntry(ObjID_t id);

	/**
	 *@brief �۲�����
	 */
	void WatchData(WSPlayer* player, UInt8 sortType, ObjID_t id);

	/**
	 *@brief �������а�
	 */
	void VisitSortList(UInt8 sortType, WSSortListVisitor& visitor);

	/** 
	 *@brief ��������
	 */
	UInt32 GetRanking(UInt8 sortType, ObjID_t id) const;

	/** 
	 *@brief ������а�
	 */
	void ClearSortList(UInt8 sortType);

public://�¼�
	/**
	 *@brief ������������������
	 */
	void OnSceneConnected(UInt32 id);

	/**
	 *@brief tick
	 */
	void OnTick(const Avalon::Time& now);

	/**
	 *@brief ����
	 */
	void OnDayChanged();

	/**
	 *@brief ����
	 */
	void OnWeekChanged();

	/**
	 *@brief �������
	 */
	void OnPlayerOnline(WSPlayer* player);

	/**
	 *@brief vip�ȼ��ı�
	 */
	void OnVipLvlChanged(WSPlayer* player);

	/**
	 *@brief ����ȼ��ı�
	 */
	void OnPlayerLevelChanged(WSPlayer* player);

	/**
	 *@brief ���а����仯����ֵ�仯Ҳ��
	 */
	void OnSortListChanged(UInt8 type);

	/**
	 *@brief ����
	 */
	void OnNameChanged(ObjID_t playerId, const std::string& name);

	/**
	*@brief ӵ���߸ı�
	*/
	void OnOwnerChanged(SortListType sortType, ObjID_t entryId, ObjID_t playerId, const std::string& name);

	/**
	*@brief תְ
	*/
	void OnOccuChanged(WSPlayer* player);

	/**
	*@brief �޸Ĺ���leader
	*/
	void OnGuildLeaderChanged(ObjID_t guildId, ObjID_t leaderId, std::string leader);

	/**
	*@brief �����Ա�䶯
	*/
	void OnGuildMemeberChanged(ObjID_t guildId, UInt32 memberCount);

	/**
	*@brief ����ȼ��䶯
	*/
	void OnGuildLevelChanged(ObjID_t guildId, UInt32 level);

	/**
	*@brief ���������
	*/
	void OnGuildNameChanged(ObjID_t guildId, std::string name);

	/**
	*@brief �޸�value3�����ǲ���������
	*/
	void OnUpdateValue3(SortListType sortType, ObjID_t roleId, UInt32 value3);

	/**
	*@brief ð�նӵȼ��ı�
	*/
	void OnAdventureTeamLevelChanged(SortListType sortType, ObjID_t uid, UInt16 level);

	/**
	*@brief ð�ն����ı�
	*/
	void OnAdventureTeamNameChanged(SortListType sortType, ObjID_t uid, const std::string& name);

	/**
	*@brief ð�ն������ı�
	*/
	void OnAdventureTeamGradeChanged(SortListType sortType, ObjID_t uid, UInt8 grade);

	//���Է���ÿ�ս���
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
	// ��ͬ���а����⴦����

	// ��������
	void OnResetSeasonMatch();

	//����ÿ�ս���
	void GiveRewardMatchDaily(const Avalon::Time& now);

public:
	// �����������ù���
	static UInt32 GetResetMatchScore(UInt32 oldScore);

	static void GrantSeasonMatchAward(ObjID_t roleId, UInt32 rank, UInt32 totalRank);

	std::vector<UInt64> GetSeasonPlayerIds(UInt32 size);

public:
	//����������
	void OnNewYearRedPacketEnd();

	bool CheckGiveAdventureSortReward();

	void GiveAdventureSortRewards();
private:
	//���а�����
	SortEntryList	m_SortEntrys[SORTLIST_MAX];

	//��ʱ����
	Avalon::SimpleTimer	m_SaveTimer;

	UInt64				m_reopen1v1SeasonTime;

	//1v1ÿ�ս��� ��������
	WSSortListGiveReward m_1v1DailyReward;

	//������
	std::set<ObjID_t> m_BlackRoleList;

};

#endif
