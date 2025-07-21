#ifndef _DS_APPLICATION_H_
#define _DS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonRetryDBConnection.h>

/**
 *@brief 挑战服务器应用程序类
 */
class DSApplication : public CLApplication, public Avalon::Singleton<DSApplication>
{
public:
	DSApplication();
	~DSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	void OnQuit();

	/**
	*@brief 信号处理
	*/
	virtual void OnSignal(int signal);

public:
	/**
	*@brief 是否检测重复进入地下城
	*/
	bool IsCheckRepeatStartDungeon() const { return m_CheckRepeatStartDungeon; }

	/**
	*@brief 是否验证md5
	*/
	bool IsCheckMD5() const { return m_CheckMd5; }

	/**
	*@brief 是否记录地下城日志
	*/
	bool IsRecordDungeonLog() const { return m_IsRecordDungeonLog; }

	/**
	*@brief 是否记录地下城录像
	*/
	bool IsRecordDungeonReplay() const { return m_IsRecordDungeonReplay; }

	/**
	*@brief 拾取掉落时检测上报的帧
	*/
	bool CheckPickDropFrame() const { return m_CheckPickDropFrame; }
private:
	/**
	*@brief 加载动态的配置
	*/
	void LoadDynamicConfig();

	/**
	*@brief 加载所有数据表
	*/
	bool LoadDataTables();

private:
	//tick时间
	UInt64			m_TickTime;
	
	//是否检测重复进入地下城
	bool			m_CheckRepeatStartDungeon;

	// 是否打开md5检测
	bool			m_CheckMd5;
	
	// 是否记录地下城日志
	bool			m_IsRecordDungeonLog;

	// 是否记录地下城录像
	bool			m_IsRecordDungeonReplay;

	//重新加载配置
	bool			m_ReloadConfig;

	// 拾取掉落时检测上报的帧
	bool			m_CheckPickDropFrame;
};

#endif
