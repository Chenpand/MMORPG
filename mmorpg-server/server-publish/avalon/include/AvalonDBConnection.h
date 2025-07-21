/**
 *@author	 
 *@email	 
 *@date		2011-03-22		
 *@brief	���ݿ�����
 */
#ifndef _AVALON_DBCONNECTION_H_
#define _AVALON_DBCONNECTION_H_

#include "AvalonDefine.h"
#include "AvalonResultSet.h"

namespace Avalon
{
	/**
	 *@brief ���ݿ�����
	 */
	class DBConnection
	{
	public:
		virtual ~DBConnection(){}

	public:
		/**
		 *@brief ����һ������
		 *@param dbtype ���ݿ����� [mysql]
		 */
		static DBConnection* Create(const std::string& dbtype);

		/**
		 *@brief ����һ������
		 */
		static void Destroy(DBConnection* conn);

		/**
		 *@brief ��һ������
		 *@param host ��������ip
		 *@param port �˿�
		 *@param user �û���
		 *@param passwd ����
		 *@param db ���ݿ���
		 */
		virtual bool Open(const char* host,UInt32 port,const char* user,const char* passwd,const char* db) = 0;


		/**
		 *@brief ��ȡ���ݿ������Ϣ
		 */
		const char* GetHost() const{ return m_Host.c_str(); }
		UInt32		GetPort() const{ return m_Port; }
		const char* GetUser() const{ return m_User.c_str(); }
		const char* GetPasswd() const{ return m_Passwd.c_str(); }
		const char* GetDB() const{ return m_Db.c_str(); }

		/**
		 *@brief �ر�����
		 */
		virtual void Close() = 0;

		/**
		 *@brief ���ñ���
		 */
		virtual bool SetCharSet(const char* csname) = 0;

		/**
		 *@brief ��ʼһ������
		 */
		virtual bool BeginTransaction() = 0;

		/**
		 *@brief �ύ����
		 */
		virtual void Commit() = 0;

		/**
		 *@brief ��������
		 */
		virtual void Rollback() = 0;

		/**
		 *@brief ��β�ѯ
		 */
		virtual ResultSet* QueryFmt(const char* sql,...);

		/**
		 *@brief ��ѯ
		 */
		virtual ResultSet* Query(const char* sql, size_t len) = 0;
		ResultSet* Query(const std::string& sql){ return Query(sql.c_str(), sql.length()); }

		/**
		 *@brief �ͷŽ����
		 */
		virtual void FreeResultSet(ResultSet* set) = 0;

		/**
		 *@brief ִ�и������
		 */
		virtual Int64 UpdateFmt(const char* sql,...);

		/**
		 *@brief ִ�и������
		 */
		virtual Int64 Update(const char* sql, size_t len) = 0;
		Int64 Update(const std::string& sql){ return Update(sql.c_str(), sql.length()); }

		/**
		 *@brief ��ȡ���ɵ�id
		 */
		virtual UInt64 GetInsertID() = 0;

		/**
		 *@brief ��ȡ������Ϣ
		 */
		virtual const char* GetError() = 0;

		/**
		 *@brief ���ַ�������ת��
		 *@param to Ŀ���ַ�������������������Ϊԭ���ȵ�2����1
		 */
		virtual size_t EscapeString(char* to, const char* from, size_t len) = 0;

		/**
		 *@brief ��ȡ����ID
		 */
		virtual UInt32 GetErrorNo() = 0;

	protected:
		//��ַ
		std::string	m_Host;
		//�˿�
		UInt32		m_Port;
		//�û���
		std::string	m_User;
		//����
		std::string	m_Passwd;
		//���ݿ�
		std::string	m_Db;
	};
}

#endif
