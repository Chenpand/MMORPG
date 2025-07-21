#ifndef _UC_APPLICATION_H_
#define _UC_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>


class CCodeQueue;
class UCPlayer;

/************************************************************************/
/* �Ľ��� UdpConnServerӦ����������serviceע�ᣬ�õ����service Ӧ���   */
/* ���������Լ���״̬ΪNormal, ��ʾ���Զ����ṩ������񣬷���ʹ�յ�client */
/* �������� ҲҪ���Ծܾ�													*/
/* TODO�� ����registerЭ�飻 ����status������ ���յ�client connect�¼���Ҫ */
/* ��鵱ǰ״̬status����register���̻�û����ɣ�Ҳ��û�д���NORMAL״�壬�� */
/* Ҫǿ�ƶϿ����ӣ�ֱ��״̬��ΪNORMAL										*/		
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

	// ���ظ��ֶ�̬����
	void LoadDynamicConfig();

	// udp��ʱ�ȴ�ʱ��
	UInt32 GetUdpRecvTimeout() const { return m_UdpRecvTimeout; }

	// ���Ӽ�ⳬʱʱ��
	UInt32 GetTimeoutLimit() const { return m_timeoutLimit; }
	UInt32 GetTimeoutMinimum() const { return m_timeoutMinimum; }
	UInt32 GetTimeoutMaximum() const { return m_timeoutMaximum; }


protected:
	//bool make_channels();

// 	//���service�����ݲ�����(ת��)
// 	void check_service(const Avalon::Time& now);

private:

	// �Ƿ�Ҫ���¼��������ļ�
	bool						m_ReloadConfig;

	// udp���ճ�ʱ�ȴ�ʱ��
	UInt32						m_UdpRecvTimeout;

	// ���Ӽ�ⳬʱʱ��
	UInt32						m_timeoutLimit;
	UInt32						m_timeoutMinimum;
	UInt32						m_timeoutMaximum;

};



#endif
