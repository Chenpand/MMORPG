#ifndef _TCS_APPLICATION_H_
#define _TCS_APPLICATION_H_

#include <set>
#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief 团本服务器应用程序类
 */
class TCSApplication : public CLApplication, public Avalon::Singleton<TCSApplication>
{
public:
	TCSApplication();
	~TCSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	virtual void OnSignal(int signal);

	/**
	*@brief 开战成员是否足够开关
	*/
	bool GetMemberIsEnough() { return m_IsMemberEnough; }

	/**
	*@brief 是否开金团
	*/
	bool GetIsOpenGoldTeam() { return m_IsOpenGoldTeam; }

	/**
	*@brief 是否记录转发日志
	*/
	bool IsLogTransProtocol() { return m_LogTransProtocol; }

	/**
	*@brief 停服维护是否返还次数
	*/
	bool IsReturnNumStopServer() { return m_StopServerIsReturnNum; }

	/**
	*@brief 失败返还佣金
	*/
	bool IsTC2FailedReturnCommission() { return m_TC2FailedReturnCommission; }
	
	/**
	*@brief 是否关闭团本
	*/
	bool IsCloseTeamType(UInt32 teamType) { return m_CloseTeamType.count(teamType) != 0; }

private:
	/**
	*@brief 加载各种动态配置
	*/
	void LoadDynamicConfig();

	/**
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

	/**
	*@brief 重新加载数据表
	*/
	bool ReloadDataTables();

private:
	
	//tick时间
	UInt64	m_TickTime;
	// 开关
	bool m_IsMemberEnough;
	// 是否开金团
	bool m_IsOpenGoldTeam;
	// 重载配置
	bool m_ReloadConfig;
	// 重载表格
	bool m_ReloadTable;
	// 是否记录转发日志
	bool m_LogTransProtocol;
	// 停服维护是否返还次数
	bool m_StopServerIsReturnNum;
	// 团本2失败返还佣金
	bool m_TC2FailedReturnCommission;
	// 关闭的团本类型
	std::set<UInt32> m_CloseTeamType;
};

#endif
