#include "CLAnnouncementDataEntry.h"
#include <regex>

AnnouncementDataEntry::AnnouncementDataEntry()
{
}

AnnouncementDataEntry::~AnnouncementDataEntry()
{
}

bool AnnouncementDataEntry::Read(Avalon::DataRow &row)
{
	id = row.ReadUInt32();
	//firstTime = row.ReadUInt32();
	//content = row.ReadString();

	std::string strTmp = row.ReadString();
	std::regex regexStr("\\{[\\w]*\\:([\\d]*)\\}");
	std::string fmt("{$1}");
	content = std::regex_replace(strTmp, regexStr, fmt);

	interval = row.ReadUInt32();
	times = row.ReadUInt32();
	param = row.ReadUInt32();

	std::string tmpstr = row.ReadString();
	std::vector<std::string> strs;
	Avalon::StringUtil::Split(tmpstr, strs, "|");
	for (auto str : strs)
	{
		UInt32 num = Avalon::StringUtil::ConvertToValue<UInt32>(str);

		if (num < 0 || num >= 32)return false;
		SyncEventBit |= (1 << num);
	}
	return true;
}
