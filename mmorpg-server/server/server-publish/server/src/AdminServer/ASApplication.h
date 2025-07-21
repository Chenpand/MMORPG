#ifndef _AS_APPLICATION_H_
#define _AS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <set>

// ���¼�������
#define  SIG_RELOAD_CONFIG 10

struct ClientMD5Info
{
	std::string tableMd5;
	std::string skillMd5;
};

/**
 *@brief �����Ӧ�ó�����
 */
class ASApplication : public CLApplication, public Avalon::Singleton<ASApplication>
{
public:
	ASApplication();
	~ASApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	virtual void OnSignal(int signal);

	void OnQuit();

    bool CheckVersion(UInt32 version);

public:
	bool IsForbidLogin(UInt32 accid);

public:
	const std::string& GetCryptKey1() const{ return m_CryptKey1; }
	const std::string& GetCryptKey2() const{ return m_CryptKey2; }
	const std::string& GetBillingKey() const{ return m_BillingKey; }
	const std::string& GetDirCryptKey() const { return m_DirCryptKey; }
	const std::string& GetReplayAgentAddress() const { return m_ReplayAgentAddress; }

	void SetWorldPlayerNum(UInt32 num) { m_WorldPlayerNum = num; }
	UInt32 GetWorldPlayerNum() const { return m_WorldPlayerNum; }

	void SetWorldMaxPlayerNum(UInt32 num) { m_WorldMaxPlayerNum = num; }
	UInt32 GetWorldMaxPlayerNum() const { return m_WorldMaxPlayerNum; }

	void SetInQueuePlayerNum(UInt32 num) { m_InQueuePlayerNum = num; }
	UInt32 GetInQueuePlayerNum() const { return m_InQueuePlayerNum; }

	const std::string GetTableMD5() const { return m_TableMd5; }
	const std::string GetSkillMD5() const { return m_SkillMd5; }
	const std::vector<ClientMD5Info>& GetClientMD5Infoes() const { return m_Md5Infoes; }
	const std::string GetSoMD5() const { return m_SoMd5; }
	const std::string GetSoKey() const { return m_SoKey; }
	bool CheckTableMD5() const { return m_CheckTableMd5; }
	bool IsOpenBugly(UInt32 accid);
	const UInt32 GetVoiceFlag() const { return m_VoiceFlag; }
	std::string GetActivityYearSortListUrl(const std::string& pf) const;
	std::string GetWebActivityUrl(const std::string& pf) const;

	bool IsOpenNewReconnectAlgo() const { return m_OpenNewReconnectAlgo; }
	bool IsOpenNewReportFrameAlgo() const { return m_OpenNewReportFrameAlgo; }

	const std::string& GetCommentServerUrl() const { return m_CommentServerUrl; }

	const std::string& GetRedPacketRankUrl() const { return m_RedPacketRankUrl; }

	/**
	*@brief �Ƿ���Ҫת���˺�
	*/
	bool IsNeedConvertAccount(const std::string& pf) const { return std::find(m_ConvertPfs.begin(), m_ConvertPfs.end(), pf) != m_ConvertPfs.end(); }

	/**
	*@brief ת����Ϣ���ӵ�ַ
	*/
	const std::string& GetConvertUrl() const { return m_ConvertUrl; }

	/**
	*@brief �ٱ����ӵ�ַ
	*/
	const std::string& GetReportUrl() const { return m_ReportWebUrl; }

	/**
	*@brief ��д�ʾ��ַ
	*/
	const std::string& GetWriteQuestionnaireUrl() const { return m_WriteQuestionnaireUrl; }

	/**
	*@brief �Ƿ��¼�ű���־
	*/
	bool IsLogTeamCopyProtocol() { return m_LogTeamCopyProtocol; }

	/**
	*@brief ս�����Ƿ�ʹ��tcp
	*/
	UInt8 BattleUseTcp() const { return m_BattleUseTcp; }

	/**
	*@brief �˺���֤��������
	*/
	UInt32 GetAccVerifyParamLength() { return m_AccVerifyParamLength; }

protected:
	// ���ظ��ֶ�̬����
	void LoadDynamicConfig();
	void LoadVersion();
    bool ReadVersionFromStr(std::string str);
    UInt32 MakeVersion(UInt32 major, UInt32 minor, UInt32 develop);
    void FetchVersion(UInt32 version, UInt32& major, UInt32& minor, UInt32& develop);
	void ReportServerStatus();

private:
    //key1             
	std::string	m_CryptKey1;
	//key2
	std::string	m_CryptKey2;
	//Ŀ¼����������key
	std::string	m_DirCryptKey;

	//billing key
	std::string	m_BillingKey;

    // �汾��
    UInt32      m_Version;

	// �Ƿ�Ҫ���¼��ذ汾��
	bool		m_ReloadConfig;

	// �Ƿ�ʼ���ܲ���
	bool		m_StartProfiler;

	// �Ƿ��Ѿ���ʼ���ܲ���
	bool		m_IsStartProfiler;

	// �㱨������״̬��ʱ��
	Avalon::SimpleTimer		m_ReportTimer;

	// Ŀ¼��������ַ
	SockAddrVec				m_DirServerAddrs;

	// Ŀ¼������socket
	Avalon::Socket			m_DirUdpSocket;

	// �Ƿ��ֹ��¼
	bool					m_ForbidLogin;

	// ��¼����������ʹ��ֹ��¼Ҳ���Ե�¼��
	std::set<UInt32>		m_LoginWhiteList;

	// ¼������������ַ
	std::string				m_ReplayAgentAddress;

	// worldserver��ǰ����
	UInt32					m_WorldPlayerNum;

	// worldserver�������
	UInt32					m_WorldMaxPlayerNum;

	// ���md5
	std::string				m_TableMd5;

	// ����md5
	std::string				m_SkillMd5;

	// ����MD5��Ϣ
	std::vector<ClientMD5Info>		m_Md5Infoes;

	// somd5+key
	std::string				m_SoMd5;

	// soKey
	std::string				m_SoKey;

	// �Ƿ���md5
	bool					m_CheckTableMd5;

	// �����Ŷ��е����
	UInt32					m_InQueuePlayerNum;

	// �Ƿ���bugly
	bool					m_OpenBugly;

	// ������־
	UInt32					m_VoiceFlag;

	//����а��ַ
	std::string				m_ActivityYearSortListUrl;

	// bugly������
	std::set<UInt32>		m_BuglyWhiteList;

	// �Ƿ����µ���������
	bool					m_OpenNewReconnectAlgo;

	// �Ƿ����µķ���֡�㷨
	bool					m_OpenNewReportFrameAlgo;

	//��ҳ���ַ
	std::string				m_WebActivityUrl;

	// װ��ͼ�����۷���ַ
	std::string				m_CommentServerUrl;

	//������е�ַ
	std::string				m_RedPacketRankUrl;

	// ��Ҫ�˺�ת�Ƶ�����
	std::vector<std::string>	m_ConvertPfs;
	// ת����Ϣ���ӵ�ַ
	std::string					m_ConvertUrl;

	// �ٱ����ӵ�ַ
	std::string					m_ReportWebUrl;

	// ��д�ʾ��ַ
	std::string					m_WriteQuestionnaireUrl;

	// �Ƿ��¼�ű���Ϣ
	bool						m_LogTeamCopyProtocol;

	// ս�����Ƿ�ʹ��tcp
	UInt8						m_BattleUseTcp;

	// �˺���֤��������
	UInt32						m_AccVerifyParamLength;
};

#endif
