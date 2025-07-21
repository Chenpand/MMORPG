#pragma once
#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief	 ������Դ������
*/

class LostDungeonResDataEntry : public Avalon::DataEntry
{
public:
	LostDungeonResDataEntry();
	~LostDungeonResDataEntry();

public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row);

	//Ψһid
	UInt32		id;
	//��Դ��id
	UInt32		resGroupId;
	//���ɵ���id
	std::vector<UInt32> items;
	//��ʼˢ��ʱ��
	UInt32		initRefreshTime;
	//ˢ�¼��
	UInt32		intervalRefreshTime;
	//��ͼid
	UInt32		mapId;
};

/**
*@brief ������Դ������
*/
class LostDungeonResDataEntryMgr : public Avalon::DataEntryMgrBase<LostDungeonResDataEntry>
	, public Avalon::Singleton<LostDungeonResDataEntryMgr>
{
public:
	LostDungeonResDataEntryMgr() {};
	~LostDungeonResDataEntryMgr() {};
public:
	bool AddEntry(LostDungeonResDataEntry* dataEntry);
	std::vector<LostDungeonResDataEntry*>& GetDatasByMapId(UInt32 mapId);

	std::map<UInt32, std::vector<LostDungeonResDataEntry*>> m_datas;

};