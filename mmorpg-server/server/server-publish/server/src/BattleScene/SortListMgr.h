#ifndef _SORTLIST_MGR_H_
#define _SORTLIST_MGR_H_

#include <AvalonSingleton.h>
#include <CLSortListDefine.h>
#include <CLRecordClient.h>
#include <set>

//���Ͷ�λ
const static UInt32 SORTLIST_ACTIVITY_MIN_SEASON_LEVEL = 54501;

class Item;

/**
 *@brief ���а������
 */
class SortListMgr : public Avalon::Singleton<SortListMgr>
{
	typedef std::set<ObjID_t> IdSet;

public:
	SortListMgr();
	~SortListMgr();

public:
	void SetEntries(UInt8 type, const std::vector<ObjID_t>& ids);
	void SetMinValue(UInt8 type, UInt64 value);

	/**
	 *@brief ���ɾ����
	 */
	void AddEntry(UInt8 type, ObjID_t id);
	void RemoveEntry(UInt8 type, ObjID_t id);

	/**
	 *@brief �۲�����
	 */
	void WatchData(ObjID_t watcher, UInt8 type, ObjID_t id, UInt32 dataId, ObjID_t owner, const std::string& ownerName, UInt32 accid);

	/**
	*@brief �����������ù���
	*/
	UInt32 GetResetMatchScore(UInt32 oldScore);

public://�¼�
	/**
	 *@brief ��ض��������ֵ�仯
	 */
	void OnSortValueChanged(UInt8 sortType, ObjID_t id, const std::string& name, UInt8 quality, UInt8 occu, 
		ObjID_t ownerId, UInt16 level, UInt32 value1, UInt32 value2 = 0, UInt32 value3 = 0); //modified by aprilliu, 2017.04.22 ����level�ֶ����ڱ�ʾװ�������а���װ���ĵȼ�
	void OnWorldSortValueChanged(UInt8 sortType, ObjID_t id, const std::string& name, UInt8 vip, UInt32 value1, UInt32 value2 = 0, UInt32 value3 = 0);

	void OnOwnerChanged(UInt8 sortType, ObjID_t entryId, ObjID_t ownerId, std::string ownerName);


	/**
	 *@brief �������ɾ��
	 */
	void OnSortEntryDeleted(UInt8 sortType, ObjID_t id);

	/**
	 *@brief ��ѯװ������
	 */
	void OnSelectEquipRet(ObjID_t watcher, const std::string& owner, CLRecordCallback* callback);

	void OnActivitySortValueChanged(UInt8 sortType, const std::string& platform, const std::string& openId, UInt32 zoneId, const std::string& zoneName, ObjID_t id, UInt32 accId, const std::string& name, UInt8 occu, UInt16 level, UInt32 value1, UInt32 value2, UInt32 value3);

private:
	void SendEquipDataToWorld(ObjID_t watcher, const std::string& owner, Item* item);
	//void SendPetDataToWorld(ObjID_t watcher, const std::string& owner, SleepingPet* pet);

	bool IsInActivitySort(const std::string& platform);

public:
	//���а���Сֵ , �����˸������а����Сֵ���������Թ��˵���Щ�ȵ�ǰ������Сֵ��С�ı仯 �����������а��Ը���
	UInt64	m_MinValues[SORTLIST_MAX];  //these array was reset when scene server has connected to world server, and fetching from world server
	//���а�id�б�
	IdSet	m_Ids[SORTLIST_MAX]; //these array was reset when scene server has connected to world server, and fetching from world server
};

#endif
