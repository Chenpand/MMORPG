#ifndef _CL_SYS_RECORD_DEFINE_H_
#define _CL_SYS_RECORD_DEFINE_H_

#include "CLDefine.h"


// sysrecord�¼�
enum SysRecordEvent
{
	SYS_RECORD_EVENT_NULL,
	// ��������
	SYS_RECORD_EVENT_MYSTICAL_MERCHANT,
};

// ϡ�п�������
enum RareControlType
{
	RARE_CTRL_INVALID = 0,

	RARE_CTRL_ITEM = 1,
	RARE_CTRL_DROP_SOLUTION = 2,
	// ���³ǲ����̵�
	RARE_CTRL_DUNGEON_MATERIAL_SHOP = 3,
	// ���³�װ���̵�
	RARE_CTRL_DUNGEON_EQUIP_SHOP = 4,
	// ���³��鱦�̵�
	RARE_CTRL_DUNGEON_JEWELRY_SHOP = 5,
};

#endif
