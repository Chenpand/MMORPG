#ifndef __ACCOUNT_MGR_H__
#define __ACCOUNT_MGR_H__

#include <AvalonSingleton.h>
#include <CLDefine.h>
#include "AccountInfo.h"

class AccountCallback
{
public:
	AccountCallback(UInt32 accid) : m_AccId(accid) {}
	virtual ~AccountCallback() {}

	virtual void OnFinish(AccountInfo* info) = 0;
	virtual void OnFailure() = 0;

protected:
	UInt32		m_AccId;
};

class AccountMgr : public Avalon::Singleton<AccountMgr>
{
public:
	AccountMgr();
	virtual ~AccountMgr();

	/**
	*@brief 结束操作
	*/
	void Final();

	/**
	*@brief 定时器
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief 根据账号ID获取账号信息
	*/
	AccountInfo* FindAccount(UInt32 accid);

	/**
	*@brief 添加账号信息
	*/
	void AddAccount(AccountInfo* info);

	/**
	*@brief 删除账号信息
	*/
	void DelAccount(UInt32 accid);

	/**
	*@brief 加载账号信息
	*/
	void LoadAccountInfo(UInt32 accid, AccountCallback* cb);

	/**
	*@brief 添加角色ID和账号ID的对应关系
	*/
	void SetRoleID2AccID(ObjID_t roleid, UInt32 accid);

	/**
	*@brief 根据角色ID获取账号ID
	*/
	UInt32 GetAccIDByRoleID(ObjID_t roleid);

private:
	HashMap<UInt32, AccountInfo*>		m_Accounts;
	HashMap<ObjID_t, UInt32>			m_RoleId2Accid;
};

#endif