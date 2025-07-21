#include "CLSysNotifyDataEntry.h"
#include <regex>

SysNotifyDataEntry::SysNotifyDataEntry()
{
}

SysNotifyDataEntry::~SysNotifyDataEntry()
{
}

bool SysNotifyDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	std::string strTmp = row.ReadString();

	//by huchenhui
	//������{%s:%d} �滻�� {$d}
	std::regex regexStr("\\{[\\w]*\\:([\\d]*)\\}");
	std::string fmt("{$1}");
	format = std::regex_replace(strTmp, regexStr, fmt);

	type = row.ReadUInt16();
	return true;
}
