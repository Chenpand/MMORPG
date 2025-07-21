#ifndef _GS_APPLICATION_H_
#define _GS_APPLICATION_H_


#include <set>
#include <CLApplication.h>
#include <AvalonSingleton.h>
#include <AvalonSimpleTimer.h>

/**
 *@brief 网关应用程序类
 */
class GSApplication : public CLApplication, public Avalon::Singleton<GSApplication>
{
public:
	GSApplication();
	~GSApplication();

public:
	bool OnInit();

	void OnTick(const Avalon::Time& now);

	virtual void OnSignal(int signal);

	/**
	*@brief 加载动态的配置
	*/
	void LoadDynamicConfig();

	void OnQuit();

public:
	const std::string& GetCryptKey1() const{ return m_CryptKey1; }
	const std::string& GetCryptKey2() const{ return m_CryptKey2; }
	UInt32 GetMaxConnNum() const { return m_MaxConnNum; }

	void SetServerStartTime(UInt32 time)	{ m_serverStartTime = time; }
	const UInt32 GetServerStartTime() const	{ return m_serverStartTime; }

	/**
	*@brief 获取服务器可承载最大人数
	*/
	UInt32 GetMaxPlayerNum() const { return m_MaxPlayerNum; }

	/**
	*@brief 获取服务器最多同时加载人数
	*/
	UInt32 GetMaxLoadingPlayerNum() const { return m_MaxLoadingPlayer; }

	bool IsEncryptProtocol() const { return m_EncryptProtocol; }
	bool IsLogTransProtocol() const { return m_LogTransProtocol; }
	bool IsLogTeamCopyProtocol() const { return m_LogTeamCopyProtocol; }

	/**
	*@brief 是否缓存包
	*/
	bool IsFilterPacketBuffer(UInt32 packetId);

	/**
	*@brief 被踢后清空保存的包
	*/
	bool IsClearPacketWhenKick() const { return m_ClearPacketWhenKick; }

	/**
	*@brief 是否开启保存转移账号文件
	*/
	bool IsSaveConvertFile() const { return m_SaveConvertFile; }

	/**
	*@brief 是否开启截图
	*/
	bool IsConvertScreenShot() const { return m_ConvertScreenShot; }

	/**
	*@brief 是否开启防沉迷
	*/
	bool IsCheckAntiAddict() { return m_CheckAntiAddict; }

	/**
	*@brief 防沉迷游客登录间隔
	*/
	UInt32 GetTouLoginInterval() { return m_TourLoginInterval; }

	/**
	*@brief 防沉迷游客在线时间
	*/
	UInt32 GetTourOnlineTime() { return m_TourOnlineTime; }

	/**
	*@brief 是否统计消息
	*/
	bool IsCountPacket(UInt64 playerId);

	/**
	*@brief 是否开启性能分析
	*/
	bool OpenPerf() const { return m_OpenPerf; }

private:
	/**
	 *@brief 加载所有数据表
	 */
	bool LoadDataTables();

	/**
	*@brief 重新加载数据表
	*/
	void ReloadDataTables();

private:
	//key1
	std::string	m_CryptKey1;
	//key2
	std::string	m_CryptKey2;
	UInt64		m_TickTime;

	// 是否要重新加载配置文件
	bool		m_ReloadConfig;
	// 最大连接数
	UInt32		m_MaxConnNum;
	// 开服时间
	UInt32		m_serverStartTime;

	//最大人数
	UInt32			m_MaxPlayerNum;
	//最大同时加载人数
	UInt32			m_MaxLoadingPlayer;  
	// 是否开启协议加密
	bool			m_EncryptProtocol;
	// 是否开始记录转发消息
	bool			m_LogTransProtocol;
	// 是否开始记录团本消息
	bool			m_LogTeamCopyProtocol;

	// 过滤部分缓存消息
	std::set<UInt32> m_FilterPacketBufferSet;

	//重新加载表格
	bool	m_ReloadTable;

	// 被踢后清空保存的包
	bool	m_ClearPacketWhenKick;

	// Tick定时器，1分钟
	Avalon::SimpleTimer		m_TickTimerMin;

	// 是否开启保存转移账号文件
	bool			m_SaveConvertFile;
	// 是否开启截图
	bool			m_ConvertScreenShot;
	// 是否检查防沉迷
	bool			m_CheckAntiAddict;
	// 防沉迷游客登录间隔
	UInt32			m_TourLoginInterval;
	// 防沉迷游客在线时间
	UInt32			m_TourOnlineTime;

	// 统计消息计数的玩家
	std::set<UInt64> m_PacketCountPlayerSet;

	// 性能分析
	bool			m_OpenPerf;
};



#endif
