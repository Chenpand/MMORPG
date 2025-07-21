#ifndef _GS_APPLICATION_H_
#define _GS_APPLICATION_H_


#include <set>
#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
 *@brief ����Ӧ�ó�����
 */
class GSApplication : public CLApplication, public Avalon::Singleton<GSApplication>
{
public:
	GSApplication();
	~GSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	virtual void OnSignal(int signal);

	/**
	*@brief ���ض�̬������
	*/
	void LoadDynamicConfig();

	void OnQuit();

public:
	const std::string& GetCryptKey1() const{ return m_CryptKey1; }
	const std::string& GetCryptKey2() const{ return m_CryptKey2; }
	UInt32 GetMaxConnNum() const { return m_MaxConnNum; }

	void SetServerStartTime(UInt32 time)	{ m_serverStartTime = time; }
	const UInt32 GetServerStartTime() const	{ return m_serverStartTime; }

	/**
	*@brief ��ȡ�������ɳ����������
	*/
	UInt32 GetMaxPlayerNum() const { return m_MaxPlayerNum; }

	/**
	*@brief ��ȡ���������ͬʱ��������
	*/
	UInt32 GetMaxLoadingPlayerNum() const { return m_MaxLoadingPlayer; }

	bool IsEncryptProtocol() const { return m_EncryptProtocol; }
	bool IsLogTransProtocol() const { return m_LogTransProtocol; }
	bool IsLogTeamCopyProtocol() const { return m_LogTeamCopyProtocol; }

	/**
	*@brief �Ƿ񻺴��
	*/
	bool IsFilterPacketBuffer(UInt32 packetId);

	/**
	*@brief ���ߺ���ձ���İ�
	*/
	bool IsClearPacketWhenKick() const { return m_ClearPacketWhenKick; }

	/**
	*@brief �Ƿ�������ת���˺��ļ�
	*/
	bool IsSaveConvertFile() const { return m_SaveConvertFile; }

	/**
	*@brief �Ƿ�����ͼ
	*/
	bool IsConvertScreenShot() const { return m_ConvertScreenShot; }

	/**
	*@brief �Ƿ���������
	*/
	bool IsCheckAntiAddict() { return m_CheckAntiAddict; }

	/**
	*@brief �������ο͵�¼���
	*/
	UInt32 GetTouLoginInterval() { return m_TourLoginInterval; }

	/**
	*@brief �������ο�����ʱ��
	*/
	UInt32 GetTourOnlineTime() { return m_TourOnlineTime; }

	/**
	*@brief �Ƿ�ͳ����Ϣ
	*/
	bool IsCountPacket(UInt64 playerId);

	/**
	*@brief �Ƿ������ܷ���
	*/
	bool OpenPerf() const { return m_OpenPerf; }

private:
	/**
	 *@brief �����������ݱ�
	 */
	bool LoadDataTables();

	/**
	*@brief ���¼������ݱ�
	*/
	void ReloadDataTables();

private:
	//key1
	std::string	m_CryptKey1;
	//key2
	std::string	m_CryptKey2;
	UInt64		m_TickTime;

	// �Ƿ�Ҫ���¼��������ļ�
	bool		m_ReloadConfig;
	// ���������
	UInt32		m_MaxConnNum;
	// ����ʱ��
	UInt32		m_serverStartTime;

	//�������
	UInt32			m_MaxPlayerNum;
	//���ͬʱ��������
	UInt32			m_MaxLoadingPlayer;  
	// �Ƿ���Э�����
	bool			m_EncryptProtocol;
	// �Ƿ�ʼ��¼ת����Ϣ
	bool			m_LogTransProtocol;
	// �Ƿ�ʼ��¼�ű���Ϣ
	bool			m_LogTeamCopyProtocol;

	// ���˲��ֻ�����Ϣ
	std::set<UInt32> m_FilterPacketBufferSet;

	//���¼��ر��
	bool	m_ReloadTable;

	// ���ߺ���ձ���İ�
	bool	m_ClearPacketWhenKick;

	// Tick��ʱ����1����
	Avalon::SimpleTimer		m_TickTimerMin;

	// �Ƿ�������ת���˺��ļ�
	bool			m_SaveConvertFile;
	// �Ƿ�����ͼ
	bool			m_ConvertScreenShot;
	// �Ƿ��������
	bool			m_CheckAntiAddict;
	// �������ο͵�¼���
	UInt32			m_TourLoginInterval;
	// �������ο�����ʱ��
	UInt32			m_TourOnlineTime;

	// ͳ����Ϣ���������
	std::set<UInt64> m_PacketCountPlayerSet;

	// ���ܷ���
	bool			m_OpenPerf;
};



#endif
