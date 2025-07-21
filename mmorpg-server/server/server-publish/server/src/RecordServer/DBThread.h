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
	*@brief ��ʼ���ӿڣ��������ݿ�����
	*/
	bool Init(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname);

	/**
	*@brief �߳�ִ����
	*/
	virtual void Run();

public:
	/**
	*@brief ���sql����
	*/
	void PushSql(const std::string& sql, SqlType type, DBRequestCallback* cb);

protected:
	/**
	*@brief ִ��sql���
	*/
	void ExcuteSql(SqlTask* task);

private:
	// ���ݿ�����
	Avalon::RetryDBConnection*	m_DbConn;
	// ��Ҫִ�е�sql�������
	SqlQueue					m_SqlQueue;
};

#endif