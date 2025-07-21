#ifndef _CL_MASTERSYSGIFT_DATAENTRY_H_
#define _CL_MASTERSYSGIFT_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>


/**
 *@brief MASTERSYSGIFT data item
 */
class MasterSysGiftDataEntryBase : public Avalon::DataEntry
{
public:
	MasterSysGiftDataEntryBase();
	~MasterSysGiftDataEntryBase();

public:
	virtual UInt32 GetKey() const { return iD; };

	bool Read(Avalon::DataRow& row);

	virtual bool InternalRead() { return true; };

	static UInt32 GetColNum() { return 6; };

	UInt32 GetParam() { return param; }
public:
	//唯一ID
	UInt32	iD;
	//礼包类型
	UInt32	type;
	//参数
	UInt32	param;
	//礼包物品id
	std::string	giftIdstr;
	//是否领取key
	std::string	counterKey;
	//成长值转良师值比例
	UInt32  rate;
};

class MasterSysGiftDataEntry : public MasterSysGiftDataEntryBase
{
public:
	MasterSysGiftDataEntry() {};
	~MasterSysGiftDataEntry() {};

	bool InternalRead();
public:
	//礼包物品id
	std::vector<UInt32>	giftIds;
};

typedef Avalon::DataEntryVisitor<MasterSysGiftDataEntry> MasterSysGiftDataEntryVisitor;

class MasterSysGiftMapDataEntryMgr : public Avalon::DataEntryMgrBase<MasterSysGiftDataEntry>
	, public Avalon::Singleton<MasterSysGiftMapDataEntryMgr>
{
public:
	MasterSysGiftMapDataEntryMgr() {
		m_MasterDailyTaskGift = NULL;
		m_DiscipleDailyTaskGift = NULL;
	}
	
	typedef std::vector<MasterSysGiftDataEntry*> DataEntryVec;

public:
	bool AddEntry(MasterSysGiftDataEntry* entry);

	//获取师傅日常任务礼包
	MasterSysGiftDataEntry*	GetMasterDailyTaskGift() { return m_MasterDailyTaskGift; }

	//获取徒弟日常任务礼包
	MasterSysGiftDataEntry* GetDiscipleDailyTaskGift() { return m_DiscipleDailyTaskGift; }

	//获取徒弟成长礼包
	const DataEntryVec& GetDiscipleAcademicTaskGiftVec() { return m_DiscipleAcademicTaskGiftVec; }

	//师傅出师礼包
	const DataEntryVec& GetMasterFinishSchoolGiftVec() { return m_MasterFinishSchoolGiftVec; }

	//徒弟出师礼包
	const DataEntryVec& GetDiscipleFinishSchoolGiftVec() { return m_DiscipleFinishSchoolGiftVec; }

	//获取某类型最低出师礼包
	const MasterSysGiftDataEntry* GetMinFinishSchoolDataEntrySpecType(UInt8 type);
	
	//获取指定成长值可获取的礼包
	const MasterSysGiftDataEntry* GetDataEntrySpecic(UInt8 type, UInt32 Acadegrowth);
private:
	void OnAddEntry(MasterSysGiftDataEntry* entry);

private:
	//师傅日常任务礼包
	MasterSysGiftDataEntry*		m_MasterDailyTaskGift;
	//徒弟日常任务礼包
	MasterSysGiftDataEntry*		m_DiscipleDailyTaskGift;
	//徒弟成长礼包
	DataEntryVec				m_DiscipleAcademicTaskGiftVec;
	//师傅出师礼包
	DataEntryVec				m_MasterFinishSchoolGiftVec;
	//徒弟出师礼包
	DataEntryVec				m_DiscipleFinishSchoolGiftVec;
};

#endif