#ifndef _US_APPLICATION_H_
#define _US_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <set>

/**
 *@brief ���������Ӧ�ó�����
 */
class USApplication : public CLApplication, public Avalon::Singleton<USApplication>
{
public:
	USApplication();
	~USApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	/**
	*@brief �Ƿ��¼ת����־
	*/
	bool IsLogTransProtocol() { return m_LogTransProtocol; }

	/**
	*@brief �Ƿ����ù���
	*/
	bool IsOpenFunction(UnionServerFunctionType funType) { return m_OpenFunction.find(funType) != m_OpenFunction.end(); }

	/**
	*@brief �Ƿ�رչھ����ƶ�ͬ���͹�սͬ��
	*/
	bool IsCloseMove() { return m_CloseMove; }

	/**
	*@brief ��ȡ�����Ĺ���
	*/
	std::set<UnionServerFunctionType>& GeOpenFunction() { return m_OpenFunction; }
	
	/**
	*@brief �¼ܶ���������
	*/
	std::set<UInt32>& GetCloseOrderZone() { return m_CloseOrderZone; }

	/**
	*@brief �¼ܶ����Ľ�ɫ
	*/
	std::set<UInt64>& GetCloseOrderRole() { return m_CloseOrderRole; }

	/**
	*@brief �¼ܶ���
	*/
	std::set<UInt64>& GetCloseOrder() { return m_CloseOrder; }
	
	/**
	*@brief ������
	*/
	std::set<UInt32>& GetReturnProfitZone() { return m_ReturnProfitZone; }

	UInt32 GetMoveCnt() { return m_MoveCnt; }

	UInt32 GetPlayerLimit() { return m_PlayerLimit; }

private:
	/**
	*@brief ���ظ��ֶ�̬����
	*/
	void LoadDynamicConfig();

	/**
	*@brief �����������ݱ�
	*/
	bool LoadDataTables();

	/**
	*@brief ���¼������ݱ�
	*/
	bool ReloadDataTables();

private:
	UInt32 m_MoveCnt = 1;
	//tickʱ��
	UInt64	m_TickTime;
	// ��������
	bool m_ReloadConfig;
	// ���ر��
	bool m_ReloadTable;
	// �Ƿ��¼ת����־
	bool m_LogTransProtocol;
	// �ھ����ر��ƶ�ͬ���͹�սͬ��
	bool m_CloseMove = false;
	// �ѿ��Ź���
	std::set<UnionServerFunctionType> m_OpenFunction;

	// ��Ҽ�����Ҫ�¼ܵ�����
	std::set<UInt32> m_CloseOrderZone;
	// ��Ҽ�����Ҫ�¼ܵĽ�ɫ
	std::set<UInt64> m_CloseOrderRole;
	// ��Ҽ�����Ҫ�¼ܵĶ���
	std::set<UInt64> m_CloseOrder;
	// ��Ҽ������淵��������
	std::set<UInt32> m_ReturnProfitZone;

	//�ھ�����������
	UInt32 m_PlayerLimit = 0;
};

#endif
