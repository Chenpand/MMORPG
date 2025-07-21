#ifndef __WS_SHAREDATA_LOAD_CALLBACK_H__
#define __WS_SHAREDATA_LOAD_CALLBACK_H__

#include <CLDefine.h>
class WSPlayerShareData;

class WSShareDataLoadCallback
{
public:
	/**
	*@brief 默认超时时间
	*/
	static const UInt32 DEFAULT_TIMEOUT_TIME = 30;

	WSShareDataLoadCallback(UInt32 timeout = DEFAULT_TIMEOUT_TIME)
		: m_StartTime((UInt32)CURRENT_TIME.Sec()), m_Timeout(timeout) {}
	virtual ~WSShareDataLoadCallback() {}

public:
	/**
	*@brief 加载成功
	*/
	virtual void OnFinish(WSPlayerShareData* player) = 0;

	/**
	*@brief 加载失败
	*/
	virtual void OnFailure(WSPlayerShareData* player) = 0;

	/**
	*@brief 加载超时
	*/
	virtual void OnTimeout(WSPlayerShareData* player) = 0;
protected:
	// 开始时间
	UInt32      m_StartTime;
	// 超时时间(s)
	UInt32      m_Timeout;
};

#endif