#ifndef _US_GAMEPARAM_MGR_H_
#define _US_GAMEPARAM_MGR_H_

#include <AvalonSingleton.h>
#include <CLRecordCallback.h>
#include <map>


#define CHAMPION_STATUS_PARAM "champion_status"

/**
 *@brief 游戏参数
 */
struct USGameParam
{
	USGameParam():id(0), value(0){}

	//主键
	ObjID_t	id;
	//值
	UInt32	value;
};

/**
 *@brief 游戏参数管理器
 */
class USGameParamMgr : public Avalon::Singleton<USGameParamMgr>
{
	friend class SelectGameParamsCallback;
	typedef std::map<std::string, USGameParam>	GameParamMap;

public:
	USGameParamMgr();
	~USGameParamMgr();
	bool IsLoaded() { return m_IsLoad; }

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
	bool OnSelectDataRet(CLRecordCallback* callback);
	void OnLoadEnd();

private:
	//游戏参数
	GameParamMap	m_GameParams;

	// 是否load
	bool			m_IsLoad;
};

#endif
