#ifndef _TCS_MGR_H_
#define _TCS_MGR_H_

#include <CLDefine.h>

/**
*brief 团本管理器
*/
class TCSMgr : public Avalon::Singleton<TCSMgr>
{
public:
	TCSMgr();
	~TCSMgr();

public:

	/**
	*brief Init
	*/
	void Init();

	/**
	*brief 团本数值配置
	*/
	UInt32 GetCfgval(UInt32 type);
	void SetCfgval(UInt32 type, UInt32 val);

private:

	std::map<UInt32, UInt32> m_ValCfgMap;
};

#endif