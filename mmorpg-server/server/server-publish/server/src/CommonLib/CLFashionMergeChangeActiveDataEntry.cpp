#include"CLFashionMergeChangeActiveDataEntry.h"

bool FashionMergeChangeActiveDataEntry::Read(Avalon::DataRow& row)
{
	if (row.GetColNum() != GetColNum())
	{
		ErrorLogStream << "row.GetColNum() != GetColNum()" << LogStream::eos;
		return false;
	}

	id = row.ReadUInt32();
	advanceId = row.ReadUInt32();
	pro = row.ReadUInt32();

	std::string goldConsumeStr = row.ReadString();
	std::vector<std::string> goldConsumeStrVec;
	Avalon::StringUtil::Split(goldConsumeStr, goldConsumeStrVec, "_");
	if (goldConsumeStrVec.size() == 2)
	{
		goldConsume.id = Avalon::StringUtil::ConvertToValue<UInt32>(goldConsumeStrVec[0]);
		goldConsume.num = Avalon::StringUtil::ConvertToValue<UInt32>(goldConsumeStrVec[1]);
	}

	std::string ticketConsumeStr = row.ReadString();
	std::vector<std::string> ticketConsumeStrVec;
	Avalon::StringUtil::Split(ticketConsumeStr, ticketConsumeStrVec, "_");
	if (ticketConsumeStrVec.size() == 2)
	{
		ticketConsume.id = Avalon::StringUtil::ConvertToValue<UInt32>(ticketConsumeStrVec[0]);
		ticketConsume.num = Avalon::StringUtil::ConvertToValue<UInt32>(ticketConsumeStrVec[1]);
	}

	return true;
}