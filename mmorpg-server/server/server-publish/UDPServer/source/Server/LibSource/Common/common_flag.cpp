
#include "Common/common_flag.h"

SERVER_BEGIN

	CFlag::CFlag(uint32_t flag):m_uiFlag(flag)
	{

	}

	CFlag::CFlag():m_uiFlag(0)
	{

	}

	CFlag::~CFlag()
	{

	}

	void CFlag::initialize(uint32_t flag)
	{
		m_uiFlag = flag;
	}

	uint32_t CFlag::flag()
	{
		return m_uiFlag;
	}

	void CFlag::set(uint32_t bit)
	{
		m_uiFlag |= bit; 
	}

	void CFlag::unset(uint32_t bit)
	{
		uint32_t tmp = 0;
		tmp = bit ^ all_bits;

		m_uiFlag = m_uiFlag & tmp;

	}

	bool CFlag::is_setted(uint32_t bit)
	{
		if (m_uiFlag & bit)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	uint32_t CFlag::get_allbit()
	{
		return m_uiFlag;
	}

SERVER_END
