#ifndef __VERIFY_SERVER_MGR_H__
#define __VERIFY_SERVER_MGR_H__

#include <AvalonSingleton.h>
#include <AvalonTime.h>
#include <CLObjectIndex.h>
#include <CLVerifyDefine.h>
#include "CLVerifyNodeMgr.h"

/**
*@brief 验证服务器节点管理器
*/
class VerifyServerMgr : public CLVerifyNodeMgr, public Avalon::Singleton<VerifyServerMgr>
{
public:
	VerifyServerMgr();
	~VerifyServerMgr();

public:
	virtual void OnUpdateAbility(UInt32 id, UInt32 ability);

	/**
	*@brief 新增node事件
	*/
	virtual void OnAddNode(CLVerifyNode* node);

	/**
	*@brief 新增node事件
	*/
	virtual void OnDelNode(UInt32 id);

	/**
	*@brief 显示所有node的负载情况
	*/
	void ShowAbility();
};

#endif
