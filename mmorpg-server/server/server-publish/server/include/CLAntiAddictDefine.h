#ifndef _CL_ANTI_ADDICT_DEFINE_H_
#define _CL_ANTI_ADDICT_DEFINE_H_

#include <CLDefine.h>

/**
*@brief �������������
*/
enum AuthIDType
{
	AUTH_NO_ID = 0,		// δʵ��
	AUTH_NO_ADULT = 1,	// δ����
	AUTH_ADULT = 2,		// ����
};

/**
*@brief ��������Ϣ
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

	//����ʱ��
	UInt32 addictSecTime;
	//����
	UInt32 age;
	//�Ƿ���֤
	UInt32 isIDAuth;
	// �ϴε�¼ʱ��
	UInt32 lastLoginTime;
};

#endif