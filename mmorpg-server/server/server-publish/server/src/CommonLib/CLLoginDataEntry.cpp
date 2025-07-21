#include <CLLoginDataEntry.h>
#include <CLItemTypeDefine.h>
#include <TimeUtil.h>

LoginPushDataEntry::LoginPushDataEntry()
{
}

LoginPushDataEntry::~LoginPushDataEntry()
{
}

bool LoginPushDataEntry::Read(Avalon::DataRow &row)
{
	if (GetColNum() != row.GetColNum())
	{
		return false;
	}

	id = row.ReadUInt8();
	name = row.ReadString();
	unlockLevel = row.ReadUInt16();
	iconPath = row.ReadString();
	linkInfo = row.ReadString();

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(startTime, str))
		{
			ErrorLogStream << "Error start time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	{
		std::string str = row.ReadString();
		if (!TimeUtil::TimeStrConvertToTimestamp(endTime, str))
		{
			ErrorLogStream << "Error end time format(" << str << ")!" << LogStream::eos;
			return false;
		}
	}

	daysAfterService = row.ReadUInt32();
	continueDays = row.ReadUInt32();
	openInterval = row.ReadString();
	closeInterval = row.ReadString();
	loadingIconPath = row.ReadString();
	sortNum = row.ReadUInt8();
	isShowTime = row.ReadUInt8();
	isSetNative = row.ReadInt8();

	return true;
}


LoginPushDataEntryMgr::LoginPushDataEntryMgr()
{
}

LoginPushDataEntryMgr::~LoginPushDataEntryMgr()
{
}