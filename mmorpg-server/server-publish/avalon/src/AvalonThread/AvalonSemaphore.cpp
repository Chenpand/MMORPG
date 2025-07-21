#include "AvalonSemaphore.h"

namespace Avalon
{
	Semaphore::Semaphore(UInt32 value)
	{
		semaphore_init(m_Sem,value);
	}

	Semaphore::~Semaphore()
	{
		semaphore_destroy(m_Sem);
	}

	void Semaphore::Wait()
	{
		semaphore_wait(m_Sem);
	}

	void Semaphore::Post()
	{
		semaphore_post(m_Sem);
	}
}
