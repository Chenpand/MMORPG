#ifndef _CL_SECURITY_LOCK_DEFINE_H_
#define _CL_SECURITY_LOCK_DEFINE_H_


enum LockOpType
{
	LT_LOCK = 1,			// ����
	LT_UNLOCK = 2,			// ����
	LT_FORCE_UNLOCK = 3,	// ǿ�ƽ���
	LT_CANCAL_APPLY = 4,	// ȡ������
};

enum SecurityLockState
{
	SECURITY_STATE_UNLOCK	= 0,	// û��
	SECURITY_STATE_LOCK		= 1,	// ��ס
	SECURITY_STATE_FORCE	= 2,	// ����ǿ�ƽ���
};

enum SecurityLockUdpLogOpType
{
	SL_LOG_SET_SECURITY_LOCK	= 1,	// ���ð�ȫ��
	SL_LOG_SET_COMMON_DEVICE	= 2,	// ���ó����豸
	SL_LOG_SET_ITEM_LOCK		= 3,	// ����װ����
};


#endif