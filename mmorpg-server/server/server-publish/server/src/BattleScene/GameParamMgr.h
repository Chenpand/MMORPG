#ifndef _GAME_PARAM_MGR_H_
#define _GAME_PARAM_MGR_H_

#include <AvalonSingleton.h>
#include <CLDefine.h>

/**
 *@brief 游戏参数管理器，数据来源World，所有新增更新都需要同步到World
 */
class GameParamMgr : public Avalon::Singleton<GameParamMgr>
{
	typedef std::map<std::string, UInt32>	GameParamMap;

public:
	GameParamMgr();
	~GameParamMgr();

public:
	/**
	 *@brief 设置获取值
	 */
	void SetValue(const std::string& name, UInt32 value, bool isNotifyWorld = true);
	UInt32 GetValue(const std::string& name) const;

private:
	//游戏参数
	GameParamMap	m_GameParams;
};

#endif
