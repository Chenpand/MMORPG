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
	*@brief ��ʼ��
	*/
	bool Init(int threadNum);

	/**
	*@brief ����
	*/
	bool OnTick(const Avalon::Time& now);

	/**
	*@brief ��ֹ
	*/
	void Final();

public:
	/**
	*@brief ���sql����
	*/
	void ExcuteSql(const std::string& sql, SqlType type, DBRequestCallback* cb);

	/**
	*@brief ���sqlִ�н��
	*/
	void PushResult(DBResult* result);

protected:
	/**
	*@brief ���������߳�
	*/
	virtual Avalon::WorkerThread* CreateWorkerThread();

	/**
	*@brief ��ȡ���ݿ�ִ�н��
	*/
	DBResult* PopDBResult();

private:
	// ���ݿ�ִ�н��
	std::list<DBResult*>		m_ResultList;
	// ִ�н��������
	Avalon::Mutex				m_ResultListMutex;
};

#endif