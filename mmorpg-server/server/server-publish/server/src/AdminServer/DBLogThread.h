#ifndef _DB_LOG_THREAD_H_
#define _DB_LOG_THREAD_H_

#include <AvalonThread.h>
#include <AvalonPacket.h>
#include <AvalonRetryDBConnection.h>
#include <CLDefine.h>

/**
 *@brief db日志线程
 */
class DBLogThread : public Avalon::Thread
{
	typedef std::map<std::string, UInt32>	LogNameMap;

public:
	DBLogThread(const LogNameMap& logs);
	~DBLogThread();

public:
	/**
	 *@brief 开启db日志线程
	 */
	bool Start(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname);

	void Stop();

	/**
	 *@brief 一个日志请求
	 */
	void PutLogPacket(Avalon::Packet* packet);

	/**
	 *@brief 判断连接是否还存在
	 */
	bool IsDBConnAlive() const{ return m_pDBConn != NULL; } 

protected:
	void Run();

private:
	/**
	 *@brief 处理一个请求
	 */
	void ProcessLogPacket(Avalon::Packet* packet);

private:
	//是否在运行
	bool	m_bRun;
	//数据库日志请求队列
	Avalon::PacketQueue m_LogQueue;
	//数据库连接
	Avalon::RetryDBConnection* m_pDBConn;

	//db日志列表
	LogNameMap				m_DBLogs;
};

#endif
