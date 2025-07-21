#ifndef _GAS_SELECT_INITDATA_CALLBACK_H_
#define _GAS_SELECT_INITDATA_CALLBACK_H_

#include <CLRecordCallback.h>

/**
*@brief 加载初始数据回调
*/
class SelectInitDataCallback : public CLRecordCallback
{
public:
	SelectInitDataCallback();
	virtual ~SelectInitDataCallback();

public:
	virtual bool OnLoadFinished() = 0;

private:
	void OnFinished();
	void OnFailed(UInt32 errorcode);
};

#endif