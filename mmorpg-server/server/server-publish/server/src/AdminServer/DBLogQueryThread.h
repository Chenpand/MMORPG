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

	//����id
	UInt32		connid;
	//����
	std::string	type;
	//��ɫid
	ObjID_t		roleid;
	//��ʼʱ��
	UInt32		beginTime;
	//����ʱ��
	UInt32		endTime;
	//��ʼҳ
	UInt16		page;
	//ҳ��С
	UInt16		pagesize;
	//����id
	UInt32		itemId;
};

/**
 *@brief db��־��ѯ�߳�
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
	 *@brief ����db��־�߳�
	 */
	bool Start(const std::string& host, UInt32 port, const std::string& user, const std::string& passwd, const std::string& dbname);

	void Stop();

	void PutRequest(DBLogQueryRequest* request);

protected:
	void Run();

private:
	/**
	 *@brief ����һ������
	 */
	void ProcessQueryRequest(DBLogQueryRequest* request);

private:
	//�Ƿ�������
	bool	m_bRun;
	//�������
	RequestQueue			m_RequestQueue;
	//���ݿ�����
	Avalon::RetryDBConnection*	m_pDBConn;

	//db��־�б�
	LogNameMap				m_DBLogs;

	//����־��ʱ��
	Avalon::SimpleTimer		m_Timer;
};


#endif
