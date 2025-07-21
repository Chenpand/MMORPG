#ifndef _DB_LOGGER_MGR_H_
#define _DB_LOGGER_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonPacket.h>
#include <CLDefine.h>

class DBLogThread;
class DBLogQueryThread;

/**
 *@brief ���ݿ���־������
 */
class DBLoggerMgr : public Avalon::Singleton<DBLoggerMgr>
{
	typedef std::map<std::string, UInt32>	LogNameMap;
	typedef std::vector<DBLogThread*>		LogThreadVec;

public:
	DBLoggerMgr();
	~DBLoggerMgr();

public:
	bool Init(const std::string& config);
	void Final();

public:
	/**
	 *@brief ����һ����־����
	 */
	void ProcessLogPacket(Avalon::Packet* packet);

	/**
	 *@brief ��ѯ��־
	 */
	void QueryLog(UInt32 connid, const std::string& logname, ObjID_t roleid, UInt32 beginTime, UInt32 endTime, UInt16 page, UInt16 pagesize, UInt32 itemId);

public://�¼�
	/**
	 *@brief tick�¼�
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief �������ݿ�����
	 */
	bool LoadDBConfig();

	/**
	 *@brief  ��ʼ����־���ݿ�
	 */
	bool InitLogDB();

private:
	//db����
	std::string m_Host;
	//�˿�
	UInt32		m_Port;
	//�û���
	std::string	m_User;
	//����
	std::string	m_Passwd;
	//���ݿ�
	std::string	m_dbName;

	//��־�߳���
	UInt32		m_LogThreadNum;
	//���ݿ���־�б�
	LogNameMap	m_Logs;

	//��־��ѯ�߳�
	DBLogQueryThread*	m_pQueryThread;
	//��־�߳��б�
	LogThreadVec	m_LogThreads;
	//��ǰ��־�߳�����
	UInt32			m_ThreadIndex;
};

#endif
