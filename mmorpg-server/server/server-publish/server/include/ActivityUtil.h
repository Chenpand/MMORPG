#ifndef _ACTIVITY_UTIL_H_
#define _ACTIVITY_UTIL_H_

#include "CLActivityDefine.h"
#include "CLActiveEventDataEntry.h"

namespace ActivityUtil
{

	// ��ʼ����Ӫ�
	bool InitOpActivityInfo(OpActivityRegInfo& info, const OpActivityDataEntry* data);

	// �Զ���������Ӫ���ɼ���
	bool CustomGenOpActCompleteCount(OpActivityRegInfo& opActInfo);

	// �Ƿ����ϱ��ع���
	bool IsInVeteranReturnPeriod(UInt32 returnTime);

	// ����ϱ��ع�
	bool CheckVeteranReturn(UInt32 accId, UInt16 maxLevelInRoles, UInt32 veteranReturnTime, UInt32 accountOfflineTime);
}

#endif