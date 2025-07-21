#ifndef __CL_VERIFY_DEFINE_H__
#define __CL_VERIFY_DEFINE_H__

#include <CLDefine.h>
#include <AvalonSocket.h>

// У����Ϣ
struct ChecksumInfo
{
	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & frame & checksum;
	}

	UInt32 frame;
	UInt32 checksum;
};

// ��ҵ�У������
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