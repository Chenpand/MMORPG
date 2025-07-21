#ifndef __VERIFY_SERVER_MGR_H__
#define __VERIFY_SERVER_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include <CLVerifyDefine.h>
#include "CLVerifyNodeMgr.h"

/**
*@brief ��֤�������ڵ������
*/
class VerifyServerMgr : public CLVerifyNodeMgr, public Avalon::Singleton<VerifyServerMgr>
{
public:
	VerifyServerMgr();
	~VerifyServerMgr();

public:
	virtual void OnUpdateAbility(UInt32 id, UInt32 ability);

	/**
	*@brief ����node�¼�
	*/
	virtual void OnAddNode(CLVerifyNode* node);

	/**
	*@brief ����node�¼�
	*/
	virtual void OnDelNode(UInt32 id);

	/**
	*@brief ��ʾ����node�ĸ������
	*/
	void ShowAbility();
};

#endif
