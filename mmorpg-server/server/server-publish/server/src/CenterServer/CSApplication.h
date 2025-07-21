#ifndef _CS_APPLICATION_H_
#define _CS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>
#include <CLOperateDefine.h>

/**
 *@brief ���ķ�����Ӧ�ó�����
 */
class CSApplication : public CLApplication, public Avalon::Singleton<CSApplication>
{
public:
	CSApplication();
	~CSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief �źŴ���
	*/
	virtual void OnSignal(int signal);

public:
	const std::string& GetPushUrl() const { return m_PushUrl; }
	const std::string & GetPushKey() const { return m_PushKey; }
	bool CanPush() { return m_CanPush; }
	const std::string& GetCustomServiceAgentAddr() const { return m_CustomServerAgentAddr; }
	const std::string& GetRoleListServerAddr() const { return m_RoleListServerAddr; }
	const std::string& GetCheatReportAddr() const { return m_CheatReportAddr; }
	const std::string& GetPlayerInfoQueryUrl() const { return m_PlayerVipInfoUrl; }
	const std::string& GetPlayerAntiAddictInfoUrl() const { return m_PlayerAntiAddictInfoUrl; }
	const std::string& GetActivityYearSortListUrl() const{ return m_ActivityYearSortListUrl; }
	const std::string& GetAccountConvertQueryUrl() const { return m_AccountConvertQueryUrl; }
	const std::string& GetCheatQueryUrl() const { return m_CheatQueryUrl; }
	const std::string& GetGmOpActivityLibUrl() const { return m_GmOpActivityLibUrl; }
	const std::string& GetGmOpActivityKey() const { return m_GmOpActivityKey; }
	const std::string& GetRedPacketRankUrl() const { return m_RedPacketRankUrl; }
	const std::string& GetCheckRechargePushUrl() const { return m_RechargePushCheckUrl; }
	const std::string& GetAccLoginTimeUrl() const { return m_AccLoginTimeUrl; }

public:
	Avalon::RetryDBConnection* GetDBConnection() const{ return m_DBConn; }
	inline const OperateInfo& GetOperateInfo() { return m_operateInfo; }

private:
	/**
	*@brief ���ض�̬������
	*/
	void LoadDynamicConfig();

private:
	
	//tickʱ��
	UInt64	m_TickTime;
	//���ݿ�����
	Avalon::RetryDBConnection* m_DBConn;
	//��Ӫ������Ϣ
	OperateInfo m_operateInfo;
	//������Ϣǩ��key
	std::string		m_PushKey;
	//������ϢĿ��url
	std::string		m_PushUrl;
	//���¼�������
	bool			m_ReloadConfig;
	//�Ƿ��������
	bool			m_CanPush;
	//�ͷ�ϵͳǩ��������
	std::string		m_CustomServerAgentAddr;
	//�����ɫ��Ϣ������
	std::string		m_RoleListServerAddr;
	//���׼�¼�ռ�������
	std::string		m_CheatReportAddr;
	//�û�vip�ȼ�
	std::string		m_PlayerVipInfoUrl;
	//�û���������Ϣ
	std::string		m_PlayerAntiAddictInfoUrl;
	//����а�URL
	std::string		m_ActivityYearSortListUrl;
	//�˺�ת����ѯ������
	std::string		m_AccountConvertQueryUrl;
	//�ͷ���Ϣurl
	std::string		m_CheatQueryUrl;
	//gm��Ӫ����ַ
	std::string		m_GmOpActivityLibUrl;
	//gm��Ӫ�key
	std::string		m_GmOpActivityKey;
	//������а�url
	std::string		m_RedPacketRankUrl;
	//��ֵ���ͼ��url
	std::string		m_RechargePushCheckUrl;
	//�ϱ��˺ŵ�¼ʱ��url
	std::string		m_AccLoginTimeUrl;
};

#endif
