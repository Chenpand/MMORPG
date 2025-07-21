#include "AvalonCondition.h"

namespace Avalon
{
	Condition::Condition()
	{
		cond_init(m_Cond);
	}

	Condition::~Condition()
	{
		cond_destroy(m_Cond);
	}

	void Condition::Wait(Mutex& mutex)
	{
		cond_wait(m_Cond,mutex.m_Mutex);
	}

	void Condition::Signal()
	{
		cond_signal(m_Cond);
	}

	void Condition::Broadcast()
	{
		cond_broadcast(m_Cond);
	}
}
