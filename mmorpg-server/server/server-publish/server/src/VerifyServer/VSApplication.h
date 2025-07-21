#ifndef _VS_APPLICATION_H_
#define _VS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <set>

/**
*@brief ��֤������
*/
class VSApplication : public CLApplication, public Avalon::Singleton<VSApplication>
{
public:
	VSApplication();
	~VSApplication();

	UInt32 GetAbility();
	UInt8 GetVerifyLogLevel() const { return m_VerifyLogLevel; }

	bool IsInDungeonWhiteList(UInt32 dungeonId) const { return m_DungeonWhiteList.find(dungeonId) != m_DungeonWhiteList.end(); }

	bool IsReportCheatWhenRaceEndUnsync() const { return m_IsReportCheatWhenRaceEndUnsync; }

	UInt32 MaxFramePerUpdate() const { return m_MaxFramePerUpdate; }

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

private:
	/**
	*@brief �����������ݱ�
	*/
	bool LoadDataTables();

	/**
	*@brief ���ظ��ֶ�̬����
	*/
	void LoadDynamicConfig();

private:
	// ���¼�������
	bool	m_ReloadConfig;

	// ���ͬʱ���е���֤����
	UInt32		m_MaxVerifyTask;

	// ��֤��������־����
	UInt8		m_VerifyLogLevel;

	// ����ľ�
	std::set<UInt32> m_DungeonWhiteList;

	// dump�ڴ�
	bool	m_DumpMemory;

	// �Ƿ���㲻ͬ����T��
	bool		m_IsReportCheatWhenRaceEndUnsync;

	// һ��updateִ�е�֡��
	UInt32		m_MaxFramePerUpdate;
};

#endif
