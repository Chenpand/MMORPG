#ifndef __CL_DAILAY_TODO_DATA_ENTRY_H__
#define __CL_DAILAY_TODO_DATA_ENTRY_H__
#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

class DungeonDataEntry;
/**
*@brief ÿ�ձ���������
*/
class DailyTodoItemDataEntry : public Avalon::DataEntry
{
public:
	DailyTodoItemDataEntry() {}
	~DailyTodoItemDataEntry() {}

public:
	UInt32 GetKey() const { return iD; }

	bool Read(Avalon::DataRow& row);

	static UInt32 GetColNum() { return 7; }

public:
	//ΨһID
	UInt32		iD;
	//����
	UInt8		type;
	//������
	UInt8		subType;
	//���³�������
	std::vector<UInt8>	dungeonSubTypes;
	//���Ƽ�����
	UInt32		dayRecommendNum;
	//���Ƽ�����
	UInt32		weekRecommendDayNum;
	//�ܼ�ˢ��
	UInt32		refreshWeek;
};


class DailyTodoItemDataEntryMgr : public Avalon::DataEntryMgrBase<DailyTodoItemDataEntry>
	, public Avalon::Singleton<DailyTodoItemDataEntryMgr>
{
public:
	typedef std::map<UInt8, DailyTodoItemDataEntry*> DataEntryMap;
	typedef std::map<UInt8, UInt8> DungTpMapDailyTp;
	bool AddEntry(DailyTodoItemDataEntry* dataEntry);

	UInt8 DungeonTypeToDailyToDoSubType(const DungeonDataEntry* dungeonData);
	DailyTodoItemDataEntry*	GetEntryBySubType(UInt8 subType);
	DailyTodoItemDataEntry* GetEntryByDungeonSubType(const DungeonDataEntry* dungeonData);
private:
	DataEntryMap	 m_DataEntrys;
	DungTpMapDailyTp m_DungTpMapDailyTps;
};

#endif