/**********************************************************************************

   ע��:
           1. _CUSTOM_*_BEGIN��_CUSTOM_*_END֮��Ĵ����ǿ����ֶ��޸ĵġ�
           2. �����ط��Ĵ��붼��Ҫ�Ķ�!!!!

*********************************************************************************/
#ifndef __CL_OVERLONDDEVICEVALUE_DATA_ENTRY_H__
#define __CL_OVERLONDDEVICEVALUE_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

//�Զ���ͷ�ļ�	
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// �Զ���ṹ��ö��	
// __CUSTOM_STRUCT_BEGIN__
struct OverlondProbabilityData
{
	//�����ȼ�
	UInt32	sharpenLv;
	//Ȩ��
	UInt32	weight;
};
// __CUSTOM_STRUCT_END__

class OverlondDeviceValueDataEntry : public Avalon::DataEntry
{
public:
	OverlondDeviceValueDataEntry();
	virtual ~OverlondDeviceValueDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// �Զ���ӿ�	
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	//ID
	UInt32                                        id;
	//����ID
	UInt32                                        itemDataID;
	//�����ȼ�
	UInt32                                        sharpenLv;
	//Ȩ��
	UInt32                                        weight;

// �Զ����ֶ�
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class OverlondDeviceValueDataEntryMgr : public Avalon::DataEntryMgrBase<OverlondDeviceValueDataEntry>
	, public Avalon::Singleton<OverlondDeviceValueDataEntryMgr>
{
public:
	virtual bool AddEntry(OverlondDeviceValueDataEntry* entry);

// �Զ���ӿڡ��ֶ�
// __CUSTOM_MGR_BEGIN__
public:
	UInt32 RandSharpenLevel(UInt32 itemDataID, UInt8 lvl);

private:
	std::map<UInt32, std::vector<OverlondProbabilityData>> probabilityMap;
// __CUSTOM_MGR_END__
};

#endif
