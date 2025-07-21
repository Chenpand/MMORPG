#ifndef _SORTLIST_MGR_H_
#define _SORTLIST_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <CLSortListDefine.h>

class GameZone;

/**
 *@brief ���а������                                                                     
 */
class SortListMgr : public Avalon::Singleton<SortListMgr>
{
	typedef std::list<SortListEntry>	SortEntryList;

public:
	SortListMgr();
	~SortListMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief ��������
	 */
	void UpdateSortList(SortListEntry& entry);

	/**
	 *@brief �г�ָ��������θ�����                                                                     
	 */
	void ListSortEntriesNear(GameZone* zone,  ObjID_t playerId, UInt8 sortType, UInt32 num);

	/**
	 *@brief �г�ָ���������                                               
	 */
	void ListSortEntries(GameZone* zone, ObjID_t playerId, UInt8 sortType, UInt16 start, UInt16 count);

	/**
	 *@brief �г�һ�������������                                                                     
	 */
	void ListSortEntries(GameZone* zone, UInt8 sortType, const std::string& function, UInt32 num);

	UInt32 GetRanking(UInt8 sortType, ObjID_t id) const;

public:
	/**
	 *@brief tick
	 */
	void OnTick(const Avalon::Time& now);

private:
	void AddToList(SortListEntry& entry);
	UInt64 GetMinSortValue(UInt8 sortType) const;
	void DeleteEntry(UInt8 sortType, SortEntryList::iterator iter);
	void Save();
	void Save(SortListEntry& entry);

private:
	//���а�����
	SortEntryList		m_SortEntrys[SORTLIST_MAX];
	//���涨ʱ��
	Avalon::SimpleTimer	m_SaveTimer;
};

#endif
