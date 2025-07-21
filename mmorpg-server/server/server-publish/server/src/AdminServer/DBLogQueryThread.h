#ifndef _DB_LOG_QUERY_THREAD_H_
#define _DB_LOG_QUERY_THREAD_H_

#include <AvalonThread.h>
#include <AvalonObjectPool.h>
#include <AvalonDoubleQueue.h>
#include <AvalonRetryDBConnection.h>
#include <AvalonSimpleTimer.h>
#include <CLDefine.h>

struct DBLogQueryRequest
{
	AVALON_DEFINE_OBJECTPOOL(DBLogQueryRequest, Avalon::Mutex)

	DBLogQueryRequest():connid(0),roleid(0),beginTime(0),endTime(0),page(0),pagesize(0),itemId(0){}

	//连接id
	UInt32		connid;
	//类型
	std::string	type;
	//角色id
	ObjID_t		roleid;
	//开始时间
	UInt32		beginTime;
	//结束时间
	UInt32		endTime;
	//开始页
	UInt16		page;
	//页大小
	UInt16		pagesize;
	//道具id
	UInt32		itemId;
};

/**
 *@brief db日志查询线程
 */
class DBLogQueryThread : public Avalon::Thread
{
	typedef std::map<std::string, UInt32>	LogNameMap;

public:
	typedef Avalon::DoubleQueue<DBLogQueryRequest*>	RequestQueue;

public:
	DBLogQueryThread(const LogNameMap& lognames);
	~DBLogQueryThread();

public:
	/**
	 *@brief 开启db日志线程
	 */
	bool Start(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname);

	void Stop();

	void PutRequest(DBLogQueryRequest* request);

protected:
	void Run();

private:
	/**
	 *@brief 处理一个请求
	 */
	void ProcessQueryRequest(DBLogQueryRequest* request);

private:
	//是否在运行
	bool	m_bRun;
	//请求队列
	RequestQueue			m_RequestQueue;
	//数据库连接
	Avalon::RetryDBConnection*	m_pDBConn;

	//db日志列表
	LogNameMap				m_DBLogs;

	//清日志定时器
	Avalon::SimpleTimer		m_Timer;
};


#endif
