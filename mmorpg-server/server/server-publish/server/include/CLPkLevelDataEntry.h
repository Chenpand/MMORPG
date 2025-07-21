#ifndef _CL_PKLEVEL_DATAENTRY_H_
#define _CL_PKLEVEL_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum PkLevelType
{
    PKLV_BRONZE,    // 青铜
	PKLV_SILVER,	// 白银
    PKLV_GOLD,      // 黄金
	PKLV_DIAMOND,	// 钻石
    PKLV_EXTREME,   // 至尊
    PKLV_NUM,
};

/**
*@brief 决斗场等级数据项
*/
class PkLevelDataEntry : public Avalon::DataEntry
{
public:
    PkLevelDataEntry();
    ~PkLevelDataEntry();

public:
    UInt32 GetKey() const { return id; }

    bool Read(Avalon::DataRow& row);

public:
	bool GetDataByPkValue(UInt32 pkValue, PkLevelType& type, UInt8& level);

public:
    // id
    UInt32          id;
    //等级类型
    PkLevelType	    type;
    //等级
    UInt8	        level;
    //最低经验要求
    UInt32	        minPkValue;
};

struct PkLevelInfo
{
	PkLevelInfo()
	{
		type = PKLV_NUM;
		level = 0;
	}

	PkLevelType     type;
	UInt8           level;
};

class PkLevelDataEntryMgr : public Avalon::DataEntryMgrBase<PkLevelDataEntry>
							, public Avalon::Singleton<PkLevelDataEntryMgr>
{
public:
	/**
	*@brief 添加数据项
	*/
	virtual bool AddEntry(PkLevelDataEntry* pDataEntry);

public:
	bool GetLevelByPkValue(UInt32 pkValue, PkLevelType& type, UInt8& level);
	UInt32 GenRobotPkValue(UInt32 pkValue);

private:
	std::map<UInt32, PkLevelInfo>   m_pkLevelInfoes;
};

#endif
