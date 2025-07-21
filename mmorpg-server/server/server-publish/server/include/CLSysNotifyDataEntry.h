#ifndef _CL_SYSNOTIFY_DATAENTRY_H_
#define _CL_SYSNOTIFY_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ϵͳ��ʾ
*/
class SysNotifyDataEntry : public Avalon::DataEntry
{
public:
	SysNotifyDataEntry();
	~SysNotifyDataEntry();
	
	UInt32 GetKey() const{ return id; }

	bool Read(Avalon::DataRow &row);

public:
	//ID
	UInt32		id;
	//���ͣ�����ʾ����
	UInt16		type;
	//��ʽ���ַ���
	std::string	format;
};

typedef Avalon::DataEntryVisitor<SysNotifyDataEntry> SysNotifyDataEntryVisitor;
typedef Avalon::DataEntryMgr<SysNotifyDataEntry> SysNotifyDataEntryMgr;

#endif
