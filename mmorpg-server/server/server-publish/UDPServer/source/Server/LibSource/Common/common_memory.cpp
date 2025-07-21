
#include "Common/common_base.h"
#include "Common/common_ipc.h"
#include "Common/common_shm.h"
#include "Common/common_memory.h"

SERVER_BEGIN

char* CShmMemory::m_pszShmMemory = NULL;

CShmMemory::CShmMemory()
{
	m_pszAvailableShm = m_pszShmMemory + sizeof(*this);
}


CShmMemory::CShmMemory(key_t unKey, size_t iSize, int32_t iInitFlag)
{
	assert(iInitFlag);

	m_pszAvailableShm = m_pszShmMemory + sizeof(*this);
	Initialize(unKey, iSize);
	
	return;
}

CShmMemory::CShmMemory( key_t unKey, size_t size )
{
	m_pszAvailableShm = m_pszShmMemory + sizeof(*this);

	//
	if (unKey != m_unShmKey || size != m_iShmSize)
	{
		Initialize(unKey, size);
		return;
	}

	//检查校验和
	uint32_t tmpCRC = (uint32_t)m_unShmKey ^ (uint32_t)m_iShmSize ^ (uint32_t)m_tmCreateTime ^ 
		(uint32_t)m_tmLastTimeStamp ^ m_unCRC;
	if (0 != tmpCRC) //校验不通过,则进行初始化
	{
		Initialize(unKey, size);
		return;
	}

	time_t tmNow = time(NULL);
	if ((tmNow - m_tmCreateTime) < 0 || (tmNow - m_tmCreateTime) > 1*YEAR)
	{
		Initialize(unKey, size);
		return;
	}

	if ((tmNow - m_tmLastTimeStamp) > 2*HOUR)
	{
		Initialize(unKey, size);
		return;
	}

	set_time_stamp();
	m_iShmMode = shm_mode_resume;

	return;
}


CShmMemory::~CShmMemory()
{

}


int32_t CShmMemory::Initialize(key_t unKey, size_t iSize)
{
	m_iShmMode = shm_mode_init;
	m_unShmKey = unKey;
	m_iShmSize = iSize;

	time_t tmNow;
	time(&tmNow);
	m_tmCreateTime = tmNow;
	m_tmLastTimeStamp = tmNow;

	//计算校验和
	m_unCRC = (uint32_t)m_unShmKey ^ (uint32_t)m_iShmSize ^ (uint32_t)m_tmCreateTime ^ (uint32_t)m_tmLastTimeStamp;

	return success;
}

void CShmMemory::set_time_stamp()
{
	time_t tmNow;
	time(&tmNow);

	m_tmLastTimeStamp = tmNow;

	m_unCRC = (uint32_t)m_unShmKey ^ (uint32_t)m_iShmSize ^ (uint32_t)m_tmCreateTime ^ (uint32_t)m_tmLastTimeStamp;
}

void* CShmMemory::operator new(size_t iSize) throw()
{

	if (NULL == m_pszShmMemory)
	{
		return NULL;
	}

	return(void*)m_pszShmMemory;
}

void CShmMemory::operator delete(void *pMemory)
{
	//do nothing here!
}

int32_t CShmMemory::get_mode()
{

	return m_iShmMode;
}

void* CShmMemory::create_segment(size_t iSize)
{
	if (iSize <= 0)
	{
		return NULL;
	}

	if (iSize > get_free_size())
	{
		return NULL;
	}

	char* pMem = m_pszAvailableShm;
	m_pszAvailableShm += iSize;

	return (void*)pMem;

}

size_t CShmMemory::get_free_size()
{
	int iUsedSize = get_used_size();
	return (m_iShmSize - iUsedSize);
}

size_t CShmMemory::get_used_size()
{
	return (size_t)(m_pszAvailableShm - (char*)this);
}

CShmMemory* create_shm_memory(key_t unKey, size_t iSize, int32_t iInitFlag/*=0*/)
{
	if (iSize <= 0)
	{
		return NULL;
	}

	size_t iRealSize = iSize + sizeof(CShmMemory);

	int iShmID = 0;

	//首先去创建指定大小的共享内存
	iShmID = shmget(unKey, iRealSize,  IPC_CREAT|IPC_EXCL|0666);
	if (iShmID < 0)
	{
		if(errno != EEXIST )
		{
			return NULL;
		}
		
		//Ya! 失败，那就去Attach指定大小的内存
		iShmID = shmget(unKey, iRealSize, 0666);
		if (iShmID < 0)
		{
			//Yaya!还是失败，那爱attach多少就多少吧
			iShmID = shmget(unKey, 0, 0666);
			if (iShmID < 0)
			{
				//只有放弃
				return NULL;
			}
			else
			{
				//不符合内存大小的要求，删之
				if(shmctl(iShmID, IPC_RMID, NULL))
				{
					return NULL;
				}

				//然后，重新创建崭新的内存
				iShmID = shmget(unKey, iRealSize,  IPC_CREAT|IPC_EXCL|0666);
				if (iShmID < 0)
				{
					return NULL;
				}
			}//else

		}//
		else
		{
			//attach到已有内存区域，并没有真正创建新的内存
		}

	}//

	//attach到自己家先
	CShmMemory::m_pszShmMemory = (char*)shmat(iShmID, NULL, 0);
	if (NULL == CShmMemory::m_pszShmMemory)
	{
		return NULL;
	}
	
	//在得到的内存上创建一个管理对象先
	if (iInitFlag)
	{
		return (new CShmMemory(unKey, iRealSize, iInitFlag));
	}
	else
	{
		return (new CShmMemory(unKey, iRealSize)); 
	}
	return NULL;


}

int32_t destroy_shm_memory(key_t unKey)
{
	int iShmID = 0;

	iShmID = shmget(unKey, 0, 0666 );
	if (iShmID < 0)
	{
		return fail;
	}
	
	if(shmctl(iShmID, IPC_RMID, NULL))
	{
		return fail;
	}

	return success;
}

SERVER_END
