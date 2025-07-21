#ifndef _CL_ANTI_ADDICT_DEFINE_H_
#define _CL_ANTI_ADDICT_DEFINE_H_

#include <CLDefine.h>

/**
*@brief 防沉迷身份类型
*/
enum AuthIDType
{
	AUTH_NO_ID = 0,		// 未实名
	AUTH_NO_ADULT = 1,	// 未成年
	AUTH_ADULT = 2,		// 成年
};

/**
*@brief 防沉迷信息
*/
struct AntiAddictInfo
{
	AntiAddictInfo() : addictSecTime(0), age(0), isIDAuth(0), lastLoginTime(0) {}

	void Init(UInt32 _time, UInt32 _age, UInt32 _isIDAuth, UInt32 _lastLoginTime)
	{
		addictSecTime = _time;
		age = _age;
		isIDAuth = _isIDAuth;
		lastLoginTime = _lastLoginTime;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & addictSecTime & age & isIDAuth & lastLoginTime;
	}

	//沉迷时间
	UInt32 addictSecTime;
	//年龄
	UInt32 age;
	//是否认证
	UInt32 isIDAuth;
	// 上次登录时间
	UInt32 lastLoginTime;
};

#endif