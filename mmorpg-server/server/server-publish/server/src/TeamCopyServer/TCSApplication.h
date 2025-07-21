#ifndef _TCS_APPLICATION_H_
#define _TCS_APPLICATION_H_

#include <set>
#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief �ű�������Ӧ�ó�����
 */
class TCSApplication : public CLApplication, public Avalon::Singleton<TCSApplication>
{
public:
	TCSApplication();
	~TCSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	/**
	*@brief ��ս��Ա�Ƿ��㹻����
	*/
	bool GetMemberIsEnough() { return m_IsMemberEnough; }

	/**
	*@brief �Ƿ񿪽���
	*/
	bool GetIsOpenGoldTeam() { return m_IsOpenGoldTeam; }

	/**
	*@brief �Ƿ��¼ת����־
	*/
	bool IsLogTransProtocol() { return m_LogTransProtocol; }

	/**
	*@brief ͣ��ά���Ƿ񷵻�����
	*/
	bool IsReturnNumStopServer() { return m_StopServerIsReturnNum; }

	/**
	*@brief ʧ�ܷ���Ӷ��
	*/
	bool IsTC2FailedReturnCommission() { return m_TC2FailedReturnCommission; }
	
	/**
	*@brief �Ƿ�ر��ű�
	*/
	bool IsCloseTeamType(UInt32 teamType) { return m_CloseTeamType.count(teamType) != 0; }

private:
	/**
	*@brief ���ظ��ֶ�̬����
	*/
	void LoadDynamicConfig();

	/**
	*@brief �����������ݱ�
	*/
	bool LoadDataTables();

	/**
	*@brief ���¼������ݱ�
	*/
	bool ReloadDataTables();

private:
	
	//tickʱ��
	UInt64	m_TickTime;
	// ����
	bool m_IsMemberEnough;
	// �Ƿ񿪽���
	bool m_IsOpenGoldTeam;
	// ��������
	bool m_ReloadConfig;
	// ���ر��
	bool m_ReloadTable;
	// �Ƿ��¼ת����־
	bool m_LogTransProtocol;
	// ͣ��ά���Ƿ񷵻�����
	bool m_StopServerIsReturnNum;
	// �ű�2ʧ�ܷ���Ӷ��
	bool m_TC2FailedReturnCommission;
	// �رյ��ű�����
	std::set<UInt32> m_CloseTeamType;
};

#endif
