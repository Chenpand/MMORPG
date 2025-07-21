#ifndef __ACCOUNT_LOAD_DATA_CALLBACK_H__
#define __ACCOUNT_LOAD_DATA_CALLBACK_H__

#include <CLRecordClient.h>
#include "AccountInfo.h"
#include "AccountMgr.h"
#include "AccountActivityTaskMgr.h"
#include "WSShopMgr.h"
#include "MallMgr.h"

class LoadAccountDataCallback : public CLRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountDataCallback, Avalon::NullMutex)
public:
	LoadAccountDataCallback(AccountInfo* account)
		: m_AccId(account->GetAccID()), m_AccInfo(account), m_LoadSerial(account->IncLoadDataCount()) {}

public:
	virtual void OnFinished(AccountInfo* account) = 0;
	virtual void OnFailed(AccountInfo* account)
	{
		account->OnLoadFailure();
	}

private:
	void OnFinished() 
	{
		if (!m_AccInfo || !m_AccInfo->CheckLoadDataSerial(m_LoadSerial))
		{
			return;
		}

		OnFinished(m_AccInfo);
		m_AccInfo->SetLoadDataFinished(m_LoadSerial);

		if (m_AccInfo->IsLoadFinished())
		{
			auto account = AccountMgr::Instance()->FindAccount(m_AccInfo->GetAccID());
			if (!account)
			{
				AccountMgr::Instance()->AddAccount(m_AccInfo);
				account = m_AccInfo;
			}
			else
			{
				account->SetLoadFinishCallback(m_AccInfo->GetLoadFinishCallback());
				delete m_AccInfo;
			}

			account->OnLoadFinished();
		}
	}

	void OnLoadFailed()
	{
		if (!m_AccInfo || !m_AccInfo->CheckLoadDataSerial(m_LoadSerial))
		{
			return;
		}

		OnFailed(m_AccInfo);
	}

private:
	//玩家id
	UInt32			m_AccId;
	//账号信息
	AccountInfo*	m_AccInfo;
	//加载序号
	UInt8			m_LoadSerial;
};

class LoadChargeRecordCallback : public LoadAccountDataCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadChargeRecordCallback, Avalon::NullMutex)
public:
	LoadChargeRecordCallback(AccountInfo* info) : LoadAccountDataCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		while (NextRow())
		{
			auto elem = new AccountChargeElem();
			elem->SetID(GetKey());
			elem->SetDBData(this);
			account->GetChargeRecord().AddRecord(elem);
		}
	}

	virtual void OnFailed(AccountInfo* account)
	{
		account->OnLoadFailure();
	}
};

class LoadAccountCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountCallback, Avalon::NullMutex)
public:
	LoadAccountCallback(AccountInfo* info) : LoadChargeRecordCallback(info){}

	virtual void OnFinished(AccountInfo* account)
	{
		if (!NextRow())
		{
			OnFailed(account);
			return;
		}

		account->SetID(GetKey());
		account->SetDBData(this);
		account->InitSecurityState();
		account->ParseCommonDevMap();
	}

	virtual void OnFailed(AccountInfo* account)
	{
		account->OnLoadFailure();
	}
};

class LoadDungeonHardCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadDungeonHardCallback, Avalon::NullMutex)
public:
	LoadDungeonHardCallback(AccountInfo* info) : LoadChargeRecordCallback(info){}

	virtual void OnFinished(AccountInfo* account)
	{
		while (NextRow())
		{
			DungeonHardInfo* hard = DungeonHardInfo::CreateDungeonHardInfo(GetData("dungeon_id"));
			if (hard == NULL) continue;
			hard->SetDBData(this);
			hard->SetID(GetKey());
			account->LoadDungeonHardInfo(hard);
		}
	}
};

/**
*@brief 查询玩家账号下的活动任务回调
*/
class LoadAccountActivityTaskCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountActivityTaskCallback, Avalon::NullMutex)
public:
	LoadAccountActivityTaskCallback(AccountInfo* info) :LoadChargeRecordCallback(info){}

	virtual void OnFinished(AccountInfo* account)
	{
		AccountActivityTaskMgr::Instance()->OnLoadTaskInfo(this);
	}
};

/**
*@brief 查询玩家账号下的商店商品回调
*/
class LoadAccountShopItemCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountShopItemCallback, Avalon::NullMutex)
public:
	LoadAccountShopItemCallback(AccountInfo* info) :LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		WSShopMgr::Instance()->LoadShopItemFromDB(this);
	}
};

/**
*@brief 查询玩家账号下的商店购买记录回调
*/
class LoadAccountShopItemAccountBuyRecordCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountShopItemAccountBuyRecordCallback, Avalon::NullMutex)
public:
	LoadAccountShopItemAccountBuyRecordCallback(AccountInfo* info) :LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		WSShopMgr::Instance()->LoadShopItemAccountBuyRecordFromDB(this);
	}
};

/**
*@brief 查询玩家账号计数回调
*/
class LoadAccountCounterCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountCounterCallback, Avalon::NullMutex)
public:
	LoadAccountCounterCallback(AccountInfo* info) :LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		if (!account) return;

		account->LoadAccountCounter(this);
	}
};

/**
*@brief 查询玩家账号下的商城购买记录回调
*/
class LoadAccountMallItemAccountBuyRecordCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountMallItemAccountBuyRecordCallback, Avalon::NullMutex)
public:
	LoadAccountMallItemAccountBuyRecordCallback(AccountInfo* info) :LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		MallMgr::Instance()->LoadMallItemAccountBuyRecordFromDB(this);
	}
};

/**
*@brief 查询玩家账号下的角色评分回调
*/
class LoadAccountRoleValueScoreCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountRoleValueScoreCallback, Avalon::NullMutex)
public:
	LoadAccountRoleValueScoreCallback(AccountInfo* info) :LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		if (!account) return;

		account->LoadRoleValueScoreCallback(this);
	}
};

/**
*@brief 查询远征地图回调
*/
class LoadAccountExpeditionMapCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountExpeditionMapCallback, Avalon::NullMutex)
public:
	LoadAccountExpeditionMapCallback(AccountInfo* info) :LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		if (!account) return;

		account->GetExpeditionMgr().LoadExpeditionMapCallback(this);
	}
};

/**
*@brief 查询远征成员回调
*/
class LoadAccountExpeditionMemberCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountExpeditionMemberCallback, Avalon::NullMutex)
public:
	LoadAccountExpeditionMemberCallback(AccountInfo* info) :LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		if (!account) return;

		account->GetExpeditionMgr().LoadExpeditionMemberCallback(this);
	}
};

/**
*@brief 查询玩家账号任务回调
*/
class LoadAccountTaskCallback : public LoadChargeRecordCallback
{
	AVALON_DEFINE_OBJECTPOOL(LoadAccountTaskCallback, Avalon::NullMutex)
public:
	LoadAccountTaskCallback(AccountInfo* info) : LoadChargeRecordCallback(info) {}

	virtual void OnFinished(AccountInfo* account)
	{
		if (!account) return;

		account->LoadAccountTask(this);
	}
};



/**
*@breif 加载大富翁回调
*/
class LoadMonopolyCallback : public LoadAccountDataCallback
{
public:
	LoadMonopolyCallback(AccountInfo* info) :LoadAccountDataCallback(info) {}
	~LoadMonopolyCallback() {}

	void OnFinished(AccountInfo* info)
	{
		while (this->NextRow())
		{
			DebugLogStream << info->GetAccID() << " Load Monopoly Data Success" << LogStream::eos;
			info->GetMonopolyInfo().SetData(this);
			info->GetMonopolyInfo().InitMap();
			info->GetMonopolyInfo().LoadCards();
			info->GetMonopolyInfo().LoadCardExchange();
			info->GetMonopolyInfo().SetLoaded();
		}
	}

	void OnFailed(AccountInfo* info)
	{
		ErrorLogStream << info->GetAccID() << "LoadMonopolyCallback cant't find record" << LogStream::eos;
	}
};



#endif