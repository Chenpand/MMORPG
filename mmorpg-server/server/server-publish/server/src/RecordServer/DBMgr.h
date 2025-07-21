#ifndef __DB_MGR_H__
#define __DB_MGR_H__

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <AvalonObjectPool.h>
#include <AvalonResultSet.h>
#include <AvalonThreadPool.h>
#include "DBTask.h"
#include "DBWorkerThread.h"

struct DBResult
{
	AVALON_DEFINE_OBJECTPOOL(DBResult, Avalon::Mutex)

	DBResult()
	{
		conn = NULL;
		resultSet = NULL;
		affectRow = 0;
		callback = NULL;
	}

	Avalon::RetryDBConnection*	conn;
	Avalon::ResultSet*			resultSet;
	UInt32						affectRow; 
	DBRequestCallback*			callback;
};

class WorkerThread;
class DBMgr : public Avalon::ThreadPool, public Avalon::Singleton<DBMgr>
{
public:
	DBMgr();
	~DBMgr();

	/**
	*@brief 初始化
	*/
	bool Init(int threadNum);

	/**
	*@brief 更新
	*/
	bool OnTick(const Avalon::Time& now);

	/**
	*@brief 终止
	*/
	void Final();

public:
	/**
	*@brief 添加sql请求
	*/
	void ExcuteSql(const std::string& sql, SqlType type, DBRequestCallback* cb);

	/**
	*@brief 添加sql执行结果
	*/
	void PushResult(DBResult* result);

protected:
	/**
	*@brief 创建工作线程
	*/
	virtual Avalon::WorkerThread* CreateWorkerThread();

	/**
	*@brief 获取数据库执行结果
	*/
	DBResult* PopDBResult();

private:
	// 数据库执行结果
	std::list<DBResult*>		m_ResultList;
	// 执行结果互斥锁
	Avalon::Mutex				m_ResultListMutex;
};

#endif