#ifndef _CL_GAMELOGGER_MGR_H_
#define _CL_GAMELOGGER_MGR_H_

#include <AvalonSingleton.h>
#include <AvalonLogger.h>
#include <AvalonFileAppender.h>
#include <AvalonNetConnection.h>

/**
 *@brief ��Ϸ��־Ŀ¼
 */
#define GAME_LOGGER_DIR "GameLog/"
/**
 *@brief ��Ϸ��־��
 */
#define GAME_LOGGER_CFG "../Config/GameLog.xml"

/**
 *@brief ��Ϸ��־������
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
	 *@brief ��ʼ��ĳ����־
	 */
	bool InitLogger(const std::string& logName, Avalon::FileRollType rollType);

private:
	//��־���������
	Avalon::NetConnectionPtr m_Conn;
	//�Ƿ��д
	bool	m_bEcho;
};

#endif
