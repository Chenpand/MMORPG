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
	*@brief ��������
	*/
	void Final();

	/**
	*@brief ��ʱ��
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief �����˺�ID��ȡ�˺���Ϣ
	*/
	AccountInfo* FindAccount(UInt32 accid);

	/**
	*@brief ����˺���Ϣ
	*/
	void AddAccount(AccountInfo* info);

	/**
	*@brief ɾ���˺���Ϣ
	*/
	void DelAccount(UInt32 accid);

	/**
	*@brief �����˺���Ϣ
	*/
	void LoadAccountInfo(UInt32 accid, AccountCallback* cb);

	/**
	*@brief ��ӽ�ɫID���˺�ID�Ķ�Ӧ��ϵ
	*/
	void SetRoleID2AccID(ObjID_t roleid, UInt32 accid);

	/**
	*@brief ���ݽ�ɫID��ȡ�˺�ID
	*/
	UInt32 GetAccIDByRoleID(ObjID_t roleid);

private:
	HashMap<UInt32, AccountInfo*>		m_Accounts;
	HashMap<ObjID_t, UInt32>			m_RoleId2Accid;
};

#endif