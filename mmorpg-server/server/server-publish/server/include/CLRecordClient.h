#ifndef _CL_RECORD_CLIENT_H_
#define _CL_RECORD_CLIENT_H_

#include <set>

#include <AvalonThread.h>
#include <AvalonDoubleQueue.h>
#include <AvalonNetConnection.h>
#include <AvalonSingleton.h>

#include "CLRecordCommand.h"
#include "CLRecordCallback.h"

class CLApplication;

/**
 *@brief 
 */
class CLRecordClient : public Avalon::Thread, public Avalon::Singleton<CLRecordClient>
{
	typedef std::map<UInt32, CLRecordCallback*> CallbackMap;

	/**
	 *@brief ����
	 */
	struct RecordRequest
	{
		RecordRequest():cmd(NULL),callback(NULL){}

		CLRecordCommand*	cmd;		//	����
		CLRecordCallback*	callback;	//	�ص�
	};
public:
	CLRecordClient();
	~CLRecordClient();

public:
	/**
	 *@brief ��ʼ��
	 */
	bool Init(CLApplication* app, Avalon::NetConnectionPtr conn);

	/**
	*@brief ������Ҫת���ı�
	*/
	void SetNeedTransTables(const std::vector<std::string>& tables);
	UInt8 IsTableNeedTrans(const std::string& tablename);

	/**
	 *@brief ��ֹ
	 */
	void Final();

	/**
	 *@brief ���յ���Ϣ
	 */
	void OnPacketRecved(Avalon::Packet* packet);

	/**
	 *@brief ����ص�
	 */
	void ProcessCallbacks();

public:
	/**
	 *@brief ����id
	 */
	ObjID_t GenGuid();

	/**
	 *@brief ׼����������
	 */
	CLInsertCommand* PrepareInsertCommand(const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLInsertCommand* PrepareInsertCommand(UInt32 zoneId, const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief ׼����������
	 */
	CLUpdateCommand* PrepareUpdateCommand(const std::string& table, DBKey_t key, bool bflush, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLUpdateCommand* PrepareUpdateCommand(UInt32 zoneId, const std::string& table, DBKey_t key, bool bflush, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLUpdateCommand* PrepareUpdateCommand(const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLUpdateCommand* PrepareUpdateCommand(UInt32 zoneId, const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief ׼��ɾ������
	 */
	CLDeleteCommand* PrepareDeleteCommand(const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLDeleteCommand* PrepareDeleteCommand(UInt32 zoneId, const std::string& table, DBKey_t key, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLDeleteCommand* PrepareDeleteCommand(const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLDeleteCommand* PrepareDeleteCommand(UInt32 zoneId, const std::string& table, const std::string& cond, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief ׼����ѯ����
	 */
	CLSelectCommand* PrepareSelectCommand(const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLSelectCommand* PrepareSelectCommand(UInt32 zoneId, const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief ׼����д����
	 */
	CLFlushCommand* PrepareFlushCommand(const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);
	CLFlushCommand* PrepareFlushCommand(UInt32 zoneId, const std::string& table, UInt64 roleId = 0, UInt32 tableCount = 1);

	/**
	 *@brief ׼����������
	 */
	CLChangeNameCommand* PrepareChangeNameCommand(const std::string& table, DBKey_t key, const std::string& name, UInt8 isInsert);
	CLChangeNameCommand* PrepareChangeNameCommand(UInt32 zoneId, const std::string& table, DBKey_t key, const std::string& name, UInt8 isInsert);

	/**
	*@brief ׼������������������
	*/
	CLAutoGuidInsertCommand* PrepareAutoGuidInsertCommand(const std::string& table);

	/**
	 *@brief ����һ������
	 */
	void SendCommand(CLRecordCommand* cmd,CLRecordCallback* callback = NULL, UInt64 roleId = 0);

public:

private:
	/**
	 *@brief �߳��д�����Ϣ��
	 */
	void ProcessPackets();
	
	/**
	 *@brief �߳��з�����������
	 */
	void SendCommands();

	void Run();  //noted by aprilliu, �̺߳����б�����

	void HandleInsertRet(Avalon::Packet* packet);
	void HandleUpdateRet(Avalon::Packet* packet);
	void HandleDeleteRet(Avalon::Packet* packet);
	void HandleSelectRet(Avalon::Packet* packet);
	void HandleFlushRet(Avalon::Packet* packet);
	void HandleChangeNameRet(Avalon::Packet* packet);

	/**
	 *@brief ���ý��
	 */
	void SetResult(UInt32 requestid, UInt32 result);

	/**
	 *@brief ����һ���ص�
	 */
	CLRecordCallback* FindCallback(UInt32 requestid);

private:
	//Ӧ�ó����쳣ʱ��ֹ����
	CLApplication*	m_pApp;

	//�������   ����Avalon::DoubleQueue�������̰߳�ȫ�ģ�����m_Requests  m_Callbacks  m_Packets ��û�н��мӽ����Ĳ���
	Avalon::DoubleQueue<RecordRequest,256>	m_Requests;  //noted by aprilliu , ����record server���첽������У���record client�߳���ѭ���б�pop���� ���д���
	//�ص�����
	Avalon::DoubleQueue<CLRecordCallback*>	m_Callbacks; //noted by aprilliu,  ���յ���Ӧ������Ļص��б�Ҳ�����߼��̺߳�record client�߳�ͨѶ�Ķ��У�
	//���������б�
	Avalon::NetConnectionPtr	m_Connection;
	//���յ�����Ϣ����   noted by aprilliu record client�߳� �� ����IO�߳�ͨѶ����Ϣ���У���
	Avalon::PacketQueue	m_Packets;

	//�ص��б�
	CallbackMap		m_WaitCallbacks;  //noted by aprilliu, �ȴ�record server������Ӧ����лص��Ķ��У� �뷢��ȥ��db�����Ӧ�Ļص������������к�Ϊkey��
	//��ǰʱ��
	UInt32			m_Now;
	//guid����
	UInt64			m_Seed;
	//����id����
	UInt32			m_RequestIdSeed;

	//�Ƿ�����
	bool	m_bRun;

	// ��Ҫת��������Record�ı�
	std::set<std::string>	m_Trans2SlaverTableNames;

	// �����
	UInt32 m_Sequence;

public:

	/**
	 *@brief ��ʼ��(֧�ֶ��RecordServer����)
	 */
	bool Init(CLApplication* app, const std::vector<Avalon::NetConnectionPtr>& connVec);

private:

	/**
	 *@brief �߳��з�����������
	 */
	void SendCommandNews();

private:

	//RecordServer����
	UInt32 m_ConnectionNum;
	//���������б�
	std::vector<Avalon::NetConnectionPtr>	m_ConnectionVec;
	//�������
	std::vector<Avalon::DoubleQueue<RecordRequest, 256>>	m_RequestVec;
};


/**
 *@brief ��д�����ر�����
 */
 void CLFlushPlayerTables();


#endif
