#ifndef __DB_TASK_H__
#define __DB_TASK_H__

#include <CLDefine.h>
#include <AvalonObjectPool.h>
#include <AvalonTask.h>
#include "DBWorkerThread.h"

enum SqlType
{
	SQL_SELECT,
	SQL_INSERT,
	SQL_UPDATE,
	SQL_DELETE,
	SQL_NUM
};

class DBRequestCallback
{
public:
	DBRequestCallback() : m_EffectRow(0) {}
	virtual ~DBRequestCallback() {}

	virtual void OnSuccess(Avalon::ResultSet* resultSet) {}
	virtual void OnFailure() {}

	UInt32 GetAffectRow() { return m_EffectRow; }
	void SetAffectRow(UInt32 row) { m_EffectRow = row; }

	const std::string& GetSql() { return m_Sql; }
	void SetSql(const std::string& sql) { m_Sql = sql; }

protected:
	UInt32 m_EffectRow;
	std::string m_Sql;
};

struct SqlTask
{
	AVALON_DEFINE_OBJECTPOOL(SqlTask, Avalon::Mutex)

	//sql����
	SqlType				type;
	// sql���
	std::string			sql;
	// �ص�
	DBRequestCallback*	callback;
};

class DBTask : public Avalon::Task
{
	AVALON_DEFINE_OBJECTPOOL(DBTask, Avalon::Mutex)
public:
	DBTask(SqlType type = SQL_SELECT, const std::string& sql = "", DBRequestCallback* cb = NULL);
	virtual ~DBTask();

	/**
	*@brief ����ִ�нӿڣ�ִ��sql���
	*/
	virtual void Run(Avalon::WorkerThread* thread);

	/**
	*@brief �ͷ��������
	*/
	virtual void Release();

private:
	//sql����
	SqlType				m_Type;
	// sql���
	std::string			m_Sql;
	// �ص�
	DBRequestCallback*	m_CallBack;
	
};

#endif