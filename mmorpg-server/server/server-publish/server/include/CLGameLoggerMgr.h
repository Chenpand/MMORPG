#ifndef _CL_GAMELOGGER_MGR_H_
#define _CL_GAMELOGGER_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonLogger.h>
#include <AvalonFileAppender.h>
#include <AvalonNetConnection.h>

/**
 *@brief 游戏日志目录
 */
#define GAME_LOGGER_DIR "GameLog/"
/**
 *@brief 游戏日志配
 */
#define GAME_LOGGER_CFG "../Config/GameLog.xml"

/**
 *@brief 游戏日志管理器
 */
class GameLoggerMgr : public Avalon::Singleton<GameLoggerMgr>
{
public:
	GameLoggerMgr();
	~GameLoggerMgr();

public:
	bool Init(const std::string& config, bool bEcho, Avalon::NetConnectionPtr conn = NULL);
	void Final();

private:
	/**
	 *@brief 初始化某种日志
	 */
	bool InitLogger(const std::string& logName, Avalon::FileRollType rollType);

private:
	//日志服务端连接
	Avalon::NetConnectionPtr m_Conn;
	//是否回写
	bool	m_bEcho;
};

#endif
