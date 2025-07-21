#include "ActionCounterMgr.h"

ActionCounterMgr::ActionCounterMgr()
{
	m_pPlayer = NULL;
	memset(m_Count, 0, sizeof(m_Count));
}

ActionCounterMgr::~ActionCounterMgr()
{

}

UInt32 ActionCounterMgr::IncCounter(int type)
{
	if(type <= ACTION_COUNTER_INVALID || type >= ACTION_COUNTER_MAX) return 0;

	m_Count[type] += 1;
	OnIncCount(type, m_Count[type]);
	SetDirty();

	return m_Count[type];
}

UInt32 ActionCounterMgr::GetCounter(int type) const
{
	if(type <= ACTION_COUNTER_INVALID || type >= ACTION_COUNTER_MAX) return 0;
	return m_Count[type];
}

void ActionCounterMgr::DecodeString(std::istringstream& is)
{
	char split = 0;
	for(int i = 1; i < ACTION_COUNTER_MAX; ++i)
	{
		if(!is) break;
		is >> m_Count[i] >> split;
	}
}

void ActionCounterMgr::EncodeString(std::ostringstream& os)
{
	for(int i = 1; i < ACTION_COUNTER_MAX; ++i)
	{
		os << m_Count[i] << ',';
	}
}

void ActionCounterMgr::OnIncCount(int type, UInt32 count)
{

}
