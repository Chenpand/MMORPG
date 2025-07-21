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
	*@brief ��ʼ���ӿڣ�����ʵ��
	*/
	virtual bool Init();

	/**
	*@brief ��ȡ���ݿ�����
	*/
	Avalon::RetryDBConnection* GetDBConn() const;

protected:
	/**
	*@brief �������ݿ�����
	*/
	Avalon::RetryDBConnection* CreateDBConn();

private:
	// ���ݿ�����
	Avalon::RetryDBConnection*	m_DbConn;
};

#endif