#ifndef _AVALON_RETRY_DB_CONNECTION_H_
#define _AVALON_RETRY_DB_CONNECTION_H_

#include "AvalonDBConnection.h"

namespace Avalon
{
	/**
	 *@brief �Զ����������ݿ�����
	 */
	class RetryDBConnection
	{
		RetryDBConnection(const std::string& dbtype, UInt32 retryTimes, UInt32 retryInterval, Avalon::DBConnection* dbconn);

	public:
		~RetryDBConnection();

	public:
		/**
		 *@brief ����һ������
		 */
		static RetryDBConnection* Create(const std::string& dbtype, UInt32 retryTimes = 1, UInt32 retryInterval = 100);

		/**
		 *@brief ����һ������
		 */
		static void Destroy(RetryDBConnection* conn);

		/**
		 *@brief ��һ������
		 *@param host ��������ip
		 *@param port �˿�
		 *@param user �û���
		 *@param passwd ����
		 *@param db ���ݿ���
		 */
		bool Open(const char* host,UInt32 port,const char* user,const char* passwd,const char* db);

		/**
		 *@brief �ر�����
		 */
		void Close();

		/**
		 *@brief ���ñ���
		 */
		bool SetCharSet(const char* csname);

		/**
		 *@brief ��β�ѯ
		 */
		ResultSet* QueryFmt(const char* sql,...);

		/**
		 *@brief ��ѯ
		 */
		ResultSet* Query(const char* sql, size_t len);
		ResultSet* Query(const std::string& sql){ return Query(sql.c_str(), sql.length()); }

		/**
		 *@brief �ͷŽ����
		 */
		void FreeResultSet(ResultSet* set);

		
		/**
		 *@brief ִ�и������
		 */
		Int64 UpdateFmt(const char* sql,...);

		/**
		 *@brief ִ�и������
		 */
		Int64 Update(const char* sql, size_t len, bool checkErrNo = false);
		Int64 Update(const std::string& sql, bool checkErrNo=false){ return Update(sql.c_str(), sql.length(), checkErrNo); }

		/**
		 *@brief ��ȡ���ɵ�id
		 */
		UInt64 GetInsertID();

		/**
		 *@brief ��ȡ������Ϣ
		 */
		const char* GetError();

		/**
		 *@brief ���ַ�������ת��
		 *@param to Ŀ���ַ�������������������Ϊԭ���ȵ�2����1
		 */
		size_t EscapeString(char* to, const char* from, size_t len);


		UInt32 GetErrorNo() { return m_lastErrNo; }

	private:
		//���ݿ�����
		std::string	m_DbType;
		//��������
		UInt32		m_RetryTimes;
		//�������
		UInt32		m_RetryInterval;
		//����
		Avalon::DBConnection*	m_pConn;
		//�ϴδ���ID
		UInt32 m_lastErrNo;
	};
}

#endif
