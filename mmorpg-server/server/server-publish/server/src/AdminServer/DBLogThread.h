#ifndef _DB_LOG_THREAD_H_
#define _DB_LOG_THREAD_H_

#include <AvalonThread.h>
#include <AvalonPacket.h>
#include <AvalonRetryDBConnection.h>
#include <CLDefine.h>

/**
 *@brief db��־�߳�
 */
class DBLogThread : public Avalon::Thread
{
	typedef std::map<std::string, UInt32>	LogNameMap;

public:
	DBLogThread(const LogNameMap& logs);
	~DBLogThread();

public:
	/**
	 *@brief ����db��־�߳�
	 */
	bool Start(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname);

	void Stop();

	/**
	 *@brief һ����־����
	 */
	void PutLogPacket(Avalon::Packet* packet);

	/**
	 *@brief �ж������Ƿ񻹴���
	 */
	bool IsDBConnAlive() const{ return m_pDBConn != NULL; } 

protected:
	void Run();

private:
	/**
	 *@brief ����һ������
	 */
	void ProcessLogPacket(Avalon::Packet* packet);

private:
	//�Ƿ�������
	bool	m_bRun;
	//���ݿ���־�������
	Avalon::PacketQueue m_LogQueue;
	//���ݿ�����
	Avalon::RetryDBConnection* m_pDBConn;

	//db��־�б�
	LogNameMap				m_DBLogs;
};

#endif
