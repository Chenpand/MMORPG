#ifndef _ITEMREASON_MGR_H_
#define _ITEMREASON_MGR_H_

#include <AvalonSingleton.h>
#include <string>
#include <map>

/**
 *@brief 道具变动原因管理器
 */
class ItemReasonMgr : public Avalon::Singleton<ItemReasonMgr>
{
	typedef std::map<std::string, std::string>	ReasonMap;

public:
	ItemReasonMgr();
	~ItemReasonMgr();

public:
	/**
	 *@brief 初始化终止
	 */
	bool Init(const std::string& config);
	void Final();

	/**
	 *@brief 查找一个原因
	 */
	bool FindReason(const std::string& key, std::string& reason);

private:
	//道具变动原因列表
	ReasonMap	m_Reasons;
};

#endif
