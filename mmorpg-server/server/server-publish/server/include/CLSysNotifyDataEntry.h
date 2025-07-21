#ifndef _CL_SYSNOTIFY_DATAENTRY_H_
#define _CL_SYSNOTIFY_DATAENTRY_H_

#include <CLDefine.h>
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief 系统提示
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
	//类型，即显示区域
	UInt16		type;
	//格式化字符串
	std::string	format;
};

typedef Avalon::DataEntryVisitor<SysNotifyDataEntry> SysNotifyDataEntryVisitor;
typedef Avalon::DataEntryMgr<SysNotifyDataEntry> SysNotifyDataEntryMgr;

#endif
