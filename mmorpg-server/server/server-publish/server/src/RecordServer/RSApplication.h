#ifndef _RS_APPLICATION_H_
#define _RS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief ����������Ӧ�ó�����
 */
class RSApplication : public CLApplication, public Avalon::Singleton<RSApplication>
{
public:
	RSApplication();
	~RSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief �źŴ���
	*/
	virtual void OnSignal(int signal);

	/**
	*@brief ���ض�̬������
	*/
	void LoadDynamicConfig();

	bool IsRecordUpdateLog() const { return m_IsRecordUpdateLog; }
	bool OpenTransRequest() const { return m_OpenTransRequest; }

	UInt32 GetMergeFlushSize() const { return m_MergeFlushSize; }

	/**
	*@brief ��Ҫ����ִ��sql�ı�
	*/
	bool IsTableOpenBatchExecute(const std::string& tablename) const { return std::find(m_BatchTables.begin(), m_BatchTables.end(), tablename) != m_BatchTables.end(); }

	bool IsCheckMysqlErrNo() const { return m_checkMysqlLastErrNo; }

private:
	
	//tickʱ��
	UInt64	m_TickTime;

	//���¼�������
	bool			m_ReloadConfig;

	//�Ƿ��¼��־
	bool	m_IsRecordUpdateLog;
	// �Ƿ���ת��������
	bool			m_OpenTransRequest;

	//�ϲ�flush��SQL����
	UInt32			m_MergeFlushSize;
	// ��Ҫ����ִ��sql�ı�
	std::vector<std::string>	m_BatchTables;

	// �Ƿ���mysql����ID
	bool m_checkMysqlLastErrNo;
};

#endif
