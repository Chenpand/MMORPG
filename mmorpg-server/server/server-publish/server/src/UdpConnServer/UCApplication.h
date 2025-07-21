#ifndef _UC_APPLICATION_H_
#define _UC_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>


class CCodeQueue;
class UCPlayer;

/************************************************************************/
/* 改进： UdpConnServer应该首先向后端service注册，得到后端service 应答后   */
/* 才能设置自己的状态为Normal, 表示可以对外提供网络服务，否则即使收到client */
/* 连接请求 也要予以拒绝													*/
/* TODO： 新增register协议； 增加status变量； 在收到client connect事件后要 */
/* 检查当前状态status，若register过程还没有完成，也即没有处于NORMAL状体，则 */
/* 要强制断开链接，直到状态改为NORMAL										*/		
/************************************************************************/

class UCApplication : public CLApplication, public  Avalon::Singleton<UCApplication>
{
public:
	enum 
	{
		
	};
public:
	UCApplication();
	~UCApplication();

	bool OnInit();

	void OnTick(const Avalon::Time& now);

	virtual void OnSignal(int signal);

	void OnQuit();

	void NotifyPing(UCPlayer* player);

	// 加载各种动态配置
	void LoadDynamicConfig();

	// udp超时等待时间
	UInt32 GetUdpRecvTimeout() const { return m_UdpRecvTimeout; }

	// 连接检测超时时间
	UInt32 GetTimeoutLimit() const { return m_timeoutLimit; }
	UInt32 GetTimeoutMinimum() const { return m_timeoutMinimum; }
	UInt32 GetTimeoutMaximum() const { return m_timeoutMaximum; }


protected:
	//bool make_channels();

// 	//检查service的数据并处理(转发)
// 	void check_service(const Avalon::Time& now);

private:

	// 是否要重新加载配置文件
	bool						m_ReloadConfig;

	// udp接收超时等待时间
	UInt32						m_UdpRecvTimeout;

	// 连接检测超时时间
	UInt32						m_timeoutLimit;
	UInt32						m_timeoutMinimum;
	UInt32						m_timeoutMaximum;

};



#endif
