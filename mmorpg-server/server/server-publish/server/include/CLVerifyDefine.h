#ifndef __CL_VERIFY_DEFINE_H__
#define __CL_VERIFY_DEFINE_H__

#include <CLDefine.h>
#include <AvalonSocket.h>

// 校验信息
struct ChecksumInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & frame & checksum;
	}

	UInt32 frame;
	UInt32 checksum;
};

// 玩家的校验数据
struct PlayerChecksumInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & roleId & checksums;
	}

	ObjID_t						roleId;
	std::vector<ChecksumInfo>	checksums;
};

#endif