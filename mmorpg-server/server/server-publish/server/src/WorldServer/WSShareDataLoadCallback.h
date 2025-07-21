#ifndef __WS_SHAREDATA_LOAD_CALLBACK_H__
#define __WS_SHAREDATA_LOAD_CALLBACK_H__

#include <CLDefine.h>
class WSPlayerShareData;

class WSShareDataLoadCallback
{
public:
	/**
	*@brief Ĭ�ϳ�ʱʱ��
	*/
	static const UInt32 DEFAULT_TIMEOUT_TIME = 30;

	WSShareDataLoadCallback(UInt32 timeout = DEFAULT_TIMEOUT_TIME)
		: m_StartTime((UInt32)CURRENT_TIME.Sec()), m_Timeout(timeout) {}
	virtual ~WSShareDataLoadCallback() {}

public:
	/**
	*@brief ���سɹ�
	*/
	virtual void OnFinish(WSPlayerShareData* player) = 0;

	/**
	*@brief ����ʧ��
	*/
	virtual void OnFailure(WSPlayerShareData* player) = 0;

	/**
	*@brief ���س�ʱ
	*/
	virtual void OnTimeout(WSPlayerShareData* player) = 0;
protected:
	// ��ʼʱ��
	UInt32      m_StartTime;
	// ��ʱʱ��(s)
	UInt32      m_Timeout;
};

#endif