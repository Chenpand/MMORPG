#include "AvalonMutex.h"

namespace Avalon
{
	Mutex::Mutex()
	{
		mutex_init(m_Mutex);
	}

	Mutex::~Mutex()
	{
		mutex_destroy(m_Mutex);
	}

	void Mutex::Acquire()
	{
		mutex_lock(m_Mutex);
	}

	void Mutex::Release()
	{
		mutex_unlock(m_Mutex);
	}
}

