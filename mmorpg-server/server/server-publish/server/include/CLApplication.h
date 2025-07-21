#ifndef _CL_APPLICATION_H_
#define _CL_APPLICATION_H_

#include <AvalonServerApplication.h>
#include "CLDefine.h"
#include "CLNetAddress.h"

/**
 *@brief ͨ�õ�Ӧ�ó�����
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
	 *@brief �ж��Ƿ����ģʽ
	 */
	bool IsDebugMode() const{ return m_bDebugMode; }

	/**
	 *@brief ��ȡ�����ַ
	 */
	CLNetAddress* GetNetAddress(){ return &m_Address; }

private:
	//�����ַ  Admin Server�Ǵ�NetAddress.xml���صõ��� ����������Ҫ��admin server��ȡ
	CLNetAddress m_Address;
	//�Ƿ����ģʽ
	bool	m_bDebugMode;
};

#endif
