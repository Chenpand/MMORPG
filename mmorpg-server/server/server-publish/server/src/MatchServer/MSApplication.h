#ifndef _MS_APPLICATION_H_
#define _MS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
*@brief 管理服应用程序类
*/
class MSApplication : public CLApplication, public Avalon::Singleton<MSApplication>
{
public:
	MSApplication();
	~MSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief 信号处理
	*/
	virtual void OnSignal(int signal);

	/**
	*@brief 战斗标记
	*/
	UInt64 GetBattleFlag() const { return m_BattleFlag; }

private:
    /**
    *@brief 加载所有数据表
    */
    bool LoadDataTables();

	/**
	*@brief 加载动态的配置
	*/
	void LoadDynamicConfig();

private:
	//重新加载配置
	bool			m_ReloadConfig;

	// 战斗标记
	UInt64			m_BattleFlag;
};

#endif
