#ifndef _GAMEPARAM_MGR_H_
#define _GAMEPARAM_MGR_H_

#include <AvalonSingleton.h>
#include <CLRecordCallback.h>
#include <map>

/**
 *@brief 游戏参数
 */
struct GameParam
{
	GameParam():id(0), value(0){}

	//主键
	ObjID_t	id;
	//值
	UInt32	value;
};

/**
 *@brief 游戏参数管理器
 */
class GameParamMgr : public Avalon::Singleton<GameParamMgr>
{
	friend class SelectGameParamsCallback;
	typedef std::map<std::string, GameParam>	GameParamMap;

public:
	GameParamMgr();
	~GameParamMgr();

public:
	void Init();
	void Final();

	/**
	 *@brief 获取开服时间
	 */
	UInt32 GetGameStartTime() const;

	/**
	*@brief 计算世界等级
	*/
	void CalcWorldLevel(UInt64 now);

	void OnAdminConnected(UInt32 id);
	void OnSceneConnected(UInt32 id);
	void OnGateConnected(UInt32 id);

public:
	/**
	 *@brief 设置获取值
	 */
	void SetValue(const char* name, UInt32 value);
	UInt32 GetValue(const char* name) const;

	/**
	*@brief 是否第一次开服
	*/
	bool IsFirstService() const { return m_FirstService; };

	/**
	*@brief 同步到scene
	*/
	void GameParamSyncScene();

private:
	bool OnSelectDataRet(CLRecordCallback* callback);

private:
	//游戏参数
	GameParamMap	m_GameParams;

	// 是否load
	bool			m_IsLoad;

	//第一次开服
	bool			m_FirstService;
};

#endif
