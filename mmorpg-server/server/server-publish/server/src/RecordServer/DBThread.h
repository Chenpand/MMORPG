#ifndef __DB_THREAD_H__
#define __DB_THREAD_H__

#include <AvalonThread.h>
#include <AvalonRetryDBConnection.h>
#include <AvalonDoubleQueue.h>
#include "DBTask.h"

typedef Avalon::DoubleQueue<SqlTask*, 256> SqlQueue;
class DBThread : public Avalon::Thread
{
public:
	DBThread();
	~DBThread();

	/**
	*@brief 初始化接口，创建数据库连接
	*/
	bool Init(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname);

	/**
	*@brief 线程执行体
	*/
	virtual void Run();

public:
	/**
	*@brief 添加sql请求
	*/
	void PushSql(const std::string& sql, SqlType type, DBRequestCallback* cb);

protected:
	/**
	*@brief 执行sql语句
	*/
	void ExcuteSql(SqlTask* task);

private:
	// 数据库连接
	Avalon::RetryDBConnection*	m_DbConn;
	// 需要执行的sql语句任务
	SqlQueue					m_SqlQueue;
};

#endif