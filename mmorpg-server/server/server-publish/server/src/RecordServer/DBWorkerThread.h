#ifndef __DB_WORKER_THREAD_H__
#define __DB_WORKER_THREAD_H__

#include <AvalonWorkerThread.h>
#include <AvalonRetryDBConnection.h>

class ThreadPool;
class DBWorkerThread : public Avalon::WorkerThread
{
public:
	explicit DBWorkerThread(Avalon::ThreadPool* poll);
	~DBWorkerThread();

	/**
	*@brief 初始化接口，子类实现
	*/
	virtual bool Init();

	/**
	*@brief 获取数据库连接
	*/
	Avalon::RetryDBConnection* GetDBConn() const;

protected:
	/**
	*@brief 创建数据库连接
	*/
	Avalon::RetryDBConnection* CreateDBConn();

private:
	// 数据库连接
	Avalon::RetryDBConnection*	m_DbConn;
};

#endif