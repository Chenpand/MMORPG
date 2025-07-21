#ifndef _CL_SECURITY_LOCK_DEFINE_H_
#define _CL_SECURITY_LOCK_DEFINE_H_


enum LockOpType
{
	LT_LOCK = 1,			// 上锁
	LT_UNLOCK = 2,			// 解锁
	LT_FORCE_UNLOCK = 3,	// 强制解锁
	LT_CANCAL_APPLY = 4,	// 取消申请
};

enum SecurityLockState
{
	SECURITY_STATE_UNLOCK	= 0,	// 没锁
	SECURITY_STATE_LOCK		= 1,	// 锁住
	SECURITY_STATE_FORCE	= 2,	// 申请强制解锁
};

enum SecurityLockUdpLogOpType
{
	SL_LOG_SET_SECURITY_LOCK	= 1,	// 设置安全锁
	SL_LOG_SET_COMMON_DEVICE	= 2,	// 设置常用设备
	SL_LOG_SET_ITEM_LOCK		= 3,	// 设置装备锁
};


#endif