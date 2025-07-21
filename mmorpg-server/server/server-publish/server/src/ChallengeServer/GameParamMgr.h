#ifndef _GAME_PARAM_MGR_H_
#define _GAME_PARAM_MGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include <map>

/**
 *@brief 游戏参数管理器
 */
class GameParamMgr : public Avalon::Singleton<GameParamMgr>
{
	friend class SelectGameParamsCallback;
	typedef std::map<std::string, UInt32>	GameParamMap;

public:
	GameParamMgr();
	~GameParamMgr();

public:
	bool Init();
	void Final();

public:
	/**
	 *@brief 设置获取值
	 */
	void SetValue(const char* name, UInt32 value);
	UInt32 GetValue(const char* name) const;

private:
	//游戏参数
	GameParamMap	m_GameParams;
};

#endif
