#ifndef __CL_CROSS_DUNGEON_DEFINE_H__
#define __CL_CROSS_DUNGEON_DEFINE_H__

#include <AvalonProtocol.h>

// ���³ǽ���ԭ��
enum CrossDungeonEndReason
{
    CDER_INVALID,
	CDER_TEAMCOPY_FILED_DESTORY, //�ű��ݵ㱻����
	CDER_TEAMCOPY_SETTLE,        //�ű�����
};

// ���³ǽ������
enum CrossDungeonEndResult
{
	CDERT_INVALID,
	CDERT_FAIL,		//ʧ��
	CDERT_SUCC,		//�ɹ�
};

#endif
