#ifndef _ITEMREASON_MGR_H_
#define _ITEMREASON_MGR_H_

#include <AvalonSingleton.h>
#include <string>
#include <map>

/**
 *@brief ���߱䶯ԭ�������
 */
class ItemReasonMgr : public Avalon::Singleton<ItemReasonMgr>
{
	typedef std::map<std::string, std::string>	ReasonMap;

public:
	ItemReasonMgr();
	~ItemReasonMgr();

public:
	/**
	 *@brief ��ʼ����ֹ
	 */
	bool Init(const std::string& config);
	void Final();

	/**
	 *@brief ����һ��ԭ��
	 */
	bool FindReason(const std::string& key, std::string& reason);

private:
	//���߱䶯ԭ���б�
	ReasonMap	m_Reasons;
};

#endif
