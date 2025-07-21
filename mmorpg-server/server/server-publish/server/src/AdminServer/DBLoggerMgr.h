#ifndef _DB_LOGGER_MGR_H_
#define _DB_LOGGER_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonPacket.h>
#include <CLDefine.h>

class DBLogThread;
class DBLogQueryThread;

/**
 *@brief 数据库日志管理器
 */
class DBLoggerMgr : public Avalon::Singleton<DBLoggerMgr>
{
	typedef std::map<std::string, UInt32>	LogNameMap;
	typedef std::vector<DBLogThread*>		LogThreadVec;

public:
	DBLoggerMgr();
	~DBLoggerMgr();

public:
	bool Init(const std::string& config);
	void Final();

public:
	/**
	 *@brief 处理一个日志请求
	 */
	void ProcessLogPacket(Avalon::Packet* packet);

	/**
	 *@brief 查询日志
	 */
	void QueryLog(UInt32 connid, const std::string& logname, ObjID_t roleid, UInt32 beginTime, UInt32 endTime, UInt16 page, UInt16 pagesize, UInt32 itemId);

public://事件
	/**
	 *@brief tick事件
	 */
	void OnTick(const Avalon::Time& now);

private:
	/**
	 *@brief 加载数据库配置
	 */
	bool LoadDBConfig();

	/**
	 *@brief  初始化日志数据库
	 */
	bool InitLogDB();

private:
	//db主机
	std::string m_Host;
	//端口
	UInt32		m_Port;
	//用户名
	std::string	m_User;
	//密码
	std::string	m_Passwd;
	//数据库
	std::string	m_dbName;

	//日志线程数
	UInt32		m_LogThreadNum;
	//数据库日志列表
	LogNameMap	m_Logs;

	//日志查询线程
	DBLogQueryThread*	m_pQueryThread;
	//日志线程列表
	LogThreadVec	m_LogThreads;
	//当前日志线程索引
	UInt32			m_ThreadIndex;
};

#endif
