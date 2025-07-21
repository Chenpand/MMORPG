#ifndef _CL_APPLICATION_H_
#define _CL_APPLICATION_H_

#include <AvalonServerApplication.h>
#include "CLDefine.h"
#include "CLNetAddress.h"

/**
 *@brief 通用的应用程序框架
 */
class CLApplication : public Avalon::ServerApplication
{
public:
	CLApplication();
	~CLApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	 *@brief 判断是否调试模式
	 */
	bool IsDebugMode() const{ return m_bDebugMode; }

	/**
	 *@brief 获取网络地址
	 */
	CLNetAddress* GetNetAddress(){ return &m_Address; }

private:
	//网络地址  Admin Server是从NetAddress.xml加载得到， 其他服务器要从admin server拉取
	CLNetAddress m_Address;
	//是否调试模式
	bool	m_bDebugMode;
};

#endif
