#ifndef _TCS_MGR_H_
#define _TCS_MGR_H_

#include <CLDefine.h>

/**
*brief �ű�������
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
	*brief �ű���ֵ����
	*/
	UInt32 GetCfgval(UInt32 type);
	void SetCfgval(UInt32 type, UInt32 val);

private:

	std::map<UInt32, UInt32> m_ValCfgMap;
};

#endif