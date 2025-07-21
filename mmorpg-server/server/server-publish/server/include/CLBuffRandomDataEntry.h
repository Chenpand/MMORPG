/**********************************************************************************

   ע��:
           1. _CUSTOM_*_BEGIN��_CUSTOM_*_END֮��Ĵ����ǿ����ֶ��޸ĵġ�
           2. �����ط��Ĵ��붼��Ҫ�Ķ�!!!!

*********************************************************************************/
#ifndef __CL_BUFFRANDOM_DATA_ENTRY_H__
#define __CL_BUFFRANDOM_DATA_ENTRY_H__

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

// �Զ���ͷ�ļ�
// __CUSTOM_HEADER_BEGIN__
// __CUSTOM_HEADER_END__

// �Զ���ṹ��ö��
// __CUSTOM_STRUCT_BEGIN__
// __CUSTOM_STRUCT_END__

class BuffRandomDataEntry : public Avalon::DataEntry
{
public:
	BuffRandomDataEntry();
	virtual ~BuffRandomDataEntry();

	UInt32 GetKey() const;
	bool Read(Avalon::DataRow &row);

// �Զ���ӿ�
// __CUSTOM_ENTRYFUNCTION_BEGIN__
// __CUSTOM_ENTRYFUNCTION_END__

public:
	// ID
	UInt32                                          id;
	// Buff ID
	UInt32                                          buffId;
	// Ȩ��
	UInt32                                          weight;

// �Զ����ֶ�
// __CUSTOM_ENTRYFIELD_BEGIN__
// __CUSTOM_ENTRYFIELD_END__
};

class BuffRandomDataEntryMgr : public Avalon::DataEntryMgrBase<BuffRandomDataEntry>
	, public Avalon::Singleton<BuffRandomDataEntryMgr>
{
public:
	virtual bool AddEntry(BuffRandomDataEntry* entry);

// �Զ���ӿڡ��ֶ�
// __CUSTOM_MGR_BEGIN__
	Int32 RandBuff(const std::vector<UInt32>& uids);
// __CUSTOM_MGR_END__
};

#endif
