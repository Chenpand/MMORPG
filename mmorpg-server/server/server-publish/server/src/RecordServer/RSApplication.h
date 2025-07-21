#ifndef _RS_APPLICATION_H_
#define _RS_APPLICATION_H_

#include <CLApplication.h>
#include <AvalonSingleton.h>

/**
 *@brief 档案服务器应用程序类
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
	*@brief 信号处理
	*/
	virtual void OnSignal(int signal);

	/**
	*@brief 加载动态的配置
	*/
	void LoadDynamicConfig();

	bool IsRecordUpdateLog() const { return m_IsRecordUpdateLog; }
	bool OpenTransRequest() const { return m_OpenTransRequest; }

	UInt32 GetMergeFlushSize() const { return m_MergeFlushSize; }

	/**
	*@brief 需要批量执行sql的表
	*/
	bool IsTableOpenBatchExecute(const std::string& tablename) const { return std::find(m_BatchTables.begin(), m_BatchTables.end(), tablename) != m_BatchTables.end(); }

	bool IsCheckMysqlErrNo() const { return m_checkMysqlLastErrNo; }

private:
	
	//tick时间
	UInt64	m_TickTime;

	//重新加载配置
	bool			m_ReloadConfig;

	//是否记录日志
	bool	m_IsRecordUpdateLog;
	// 是否开启转发请求功能
	bool			m_OpenTransRequest;

	//合并flush的SQL数量
	UInt32			m_MergeFlushSize;
	// 需要批量执行sql的表
	std::vector<std::string>	m_BatchTables;

	// 是否检查mysql错误ID
	bool m_checkMysqlLastErrNo;
};

#endif
