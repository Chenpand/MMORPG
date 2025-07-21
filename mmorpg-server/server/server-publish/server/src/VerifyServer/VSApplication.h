#ifndef _VS_APPLICATION_H_
#define _VS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>
#include <set>

/**
*@brief 验证服务器
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
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

	/**
	*@brief 加载各种动态配置
	*/
	void LoadDynamicConfig();

private:
	// 重新加载配置
	bool	m_ReloadConfig;

	// 最多同时进行的验证任务
	UInt32		m_MaxVerifyTask;

	// 验证服务器日志级别
	UInt8		m_VerifyLogLevel;

	// 不验的局
	std::set<UInt32> m_DungeonWhiteList;

	// dump内存
	bool	m_DumpMemory;

	// 是否结算不同步后T人
	bool		m_IsReportCheatWhenRaceEndUnsync;

	// 一次update执行的帧数
	UInt32		m_MaxFramePerUpdate;
};

#endif
