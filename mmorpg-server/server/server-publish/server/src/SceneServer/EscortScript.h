#ifndef _ESCORT_SCRIPT_H_
#define _ESCORT_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLGameDefine.h>

/**
 *@brief 护送脚本配置
 */
class EscortScript : public Avalon::ScriptFile
{
public:
	EscortScript(Avalon::ScriptState* state);
	~EscortScript();

public:
	/**
	 *@brief 初始化
	 */
	void Init();

public:

	/**
	 *@brief 设置各品质下初始刷新概率
	 */
	void SetRefreshRate(int quality, UInt16 greenRate, UInt16 blueRate, UInt16 purpleRate, UInt16 goldRate);

	/**
	 *@brief 设置各品质下递增刷新概率
	 */
	void SetIncRefreshRate(int quality, UInt16 greenRate, UInt16 blueRate, UInt16 purpleRate, UInt16 goldRate);

public:
	/**
	 *@brief 获取当前品质下刷新几率
	 */
	void GetRefreshRate(int quality, UInt16 rates[5]);

	/**
	 *@brief 获取当前品质下递增概率
	 */
	void GetIncRefreshRate(int quality, UInt16 rates[5]);

private:
	//各品质下初始刷新概率(万分比)
	UInt16		m_RefreshRates[5][5];
	//跟品质下每次刷新增加概率(万分比)
	UInt16		m_IncRates[5][5];
};

#endif
