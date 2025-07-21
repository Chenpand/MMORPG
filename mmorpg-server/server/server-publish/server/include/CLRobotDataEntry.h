#ifndef _CL_ROBOT_DATAENTRY_H_
#define _CL_ROBOT_DATAENTRY_H_

#include "CLDefine.h"
#include <AvalonDataEntry.h>
#include <AvalonDataEntryMgr.h>

/**
*@brief ������������
*/
struct RobotDataEntry : public Avalon::DataEntry
{
public:
	UInt32 GetKey() const { return id; }

	bool Read(Avalon::DataRow& row)
	{
		id = row.ReadUInt32();
		name = row.ReadString();
		hard = row.ReadUInt8();
		level = row.ReadUInt16();
		occu = row.ReadUInt8();
		equips = row.ReadString();
		skills = row.ReadString();

		return true;
	}

public:
	UInt32	id;
	std::string name;
	UInt8 hard;
	UInt16 level;
	UInt8 occu;
	// װ��
	std::string equips;
	// ��������
	std::string skills;
};

typedef Avalon::DataEntryMgr<RobotDataEntry> RobotDataEntryMgr;

#endif
