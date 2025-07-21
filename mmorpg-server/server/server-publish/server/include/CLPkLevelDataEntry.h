#ifndef _CL_PKLEVEL_DATAENTRY_H_
#define _CL_PKLEVEL_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

enum PkLevelType
{
    PKLV_BRONZE,    // ��ͭ
	PKLV_SILVER,	// ����
    PKLV_GOLD,      // �ƽ�
	PKLV_DIAMOND,	// ��ʯ
    PKLV_EXTREME,   // ����
    PKLV_NUM,
};

/**
*@brief �������ȼ�������
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
    //�ȼ�����
    PkLevelType	    type;
    //�ȼ�
    UInt8	        level;
    //��;���Ҫ��
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
	*@brief ���������
	*/
	virtual bool AddEntry(PkLevelDataEntry* pDataEntry);

public:
	bool GetLevelByPkValue(UInt32 pkValue, PkLevelType& type, UInt8& level);
	UInt32 GenRobotPkValue(UInt32 pkValue);

private:
	std::map<UInt32, PkLevelInfo>   m_pkLevelInfoes;
};

#endif
