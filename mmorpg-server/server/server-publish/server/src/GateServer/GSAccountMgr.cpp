#include "GSAccountMgr.h"

GSAccountMgr::GSAccountMgr()
{
}

GSAccountMgr::~GSAccountMgr()
{
}

void GSAccountMgr::Init()
{
}

void GSAccountMgr::Final()
{
}

void GSAccountMgr::AddAccount(AccountInfo& info)
{
	info.createTime = (UInt32)CURRENT_TIME.Sec();

	m_Mutex.Acquire();
	m_Accounts[info.accid] = info;
	m_Mutex.Release();
}

bool GSAccountMgr::Verify(UInt32 accid, const UInt8 *key)
{
	bool ret = false;
	m_Mutex.Acquire();
	AccountMap::iterator iter = m_Accounts.find(accid);
	if(iter != m_Accounts.end())
	{
		if (CURRENT_TIME.Sec() > iter->second.createTime + KEY_TIMEOUT)
		{
			ret = false;
		}
		else
		{
			ret = (memcmp(iter->second.key, key, 20) == 0);
		}
	}
	m_Mutex.Release();
	return ret;
}

void GSAccountMgr::OnTick(const Avalon::Time &now)
{
	/*m_Mutex.Acquire();

	for(AccountMap::iterator iter = m_Accounts.begin();iter != m_Accounts.end();)
	{
		if(now.Sec() > iter->second.createTime + KEY_TIMEOUT)
		{
			m_Accounts.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
	m_Mutex.Release();*/
}

bool GSAccountMgr::GetAccount(UInt32 accid,AccountInfo& info)
{
	m_Mutex.Acquire();
	AccountMap::iterator iter = m_Accounts.find(accid);
	if(iter != m_Accounts.end())
	{
		info = iter->second;
		//m_Accounts.erase(iter);
		m_Mutex.Release();
		return true;
	}
	m_Mutex.Release();
	return false;
}

void GSAccountMgr::DelAccount(UInt32 accid)
{
	m_Mutex.Acquire();
	m_Accounts.erase(accid);
	m_Mutex.Release();
}

void GSAccountMgr::AddReconnAccount(UInt32 accid, UInt32 sequence)
{
	m_Mutex.Acquire();
	ReconnInfo info;
	info.accid = accid;
	info.sequence = sequence;
	m_ReconnAccounts[accid] = info;
	m_Mutex.Release();
}

ReconnInfo GSAccountMgr::GetReconnInfo(UInt32 accid)
{
	ReconnInfo info;

	m_Mutex.Acquire();
	auto itr = m_ReconnAccounts.find(accid);
	if (itr == m_ReconnAccounts.end())
	{
		m_Mutex.Release();
		return info;
	}

	info = itr->second;
	m_ReconnAccounts.erase(itr);
	m_Mutex.Release();
	return info;
}

void GSAccountMgr::UpdateAntiAddictTime(UInt32 accid, UInt32 time)
{
	m_Mutex.Acquire();
	AccountMap::iterator iter = m_Accounts.find(accid);
	if (iter != m_Accounts.end())
	{
		iter->second.antiAddictInfo.addictSecTime = time;
	}
	m_Mutex.Release();
}