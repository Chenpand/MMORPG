#ifndef _UC_APPLICATION_H_
#define _UC_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>


class	CCodeQueue;

/************************************************************************/
/* �Ľ��� UdpConnServerӦ����������serviceע�ᣬ�õ����service Ӧ���   */
/* ���������Լ���״̬ΪNormal, ��ʾ���Զ����ṩ������񣬷���ʹ�յ�client */
/* �������� ҲҪ���Ծܾ�													*/
/* TODO�� ����registerЭ�飻 ����status������ ���յ�client connect�¼���Ҫ */
/* ��鵱ǰ״̬status����register���̻�û����ɣ�Ҳ��û�д���NORMAL״�壬�� */
/* Ҫǿ�ƶϿ����ӣ�ֱ��״̬��ΪNORMAL										*/
/* web��Ŀ�޸ĳ�TCP  huchenhui	2017.05.22									*/
/************************************************************************/

class TCApplication : public CLApplication, public  Avalon::Singleton<TCApplication>
{
public:
	enum
	{

	};
public:
	TCApplication();
	~TCApplication();

	bool OnInit();

	void OnTick(const Avalon::Time& now);

	virtual void OnSignal(int signal);

	void OnQuit();

protected:
	//bool make_channels();

	// 	//���service�����ݲ�����(ת��)
	// 	void check_service(const Avalon::Time& now);

private:

	// �Ƿ�Ҫ���¼��������ļ�
	bool						m_ReloadConfig;


};



#endif
