#include "AvalonSharedMemory.h"
#include <stdlib.h>
#include <errno.h>

#ifdef AVALON_OS_WINDOWS
#include <windows.h>
#else
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#endif

#include <new>

//#ifndef AVALON_OS_WINDOWS

namespace Avalon
{

// 	char* strjoin(const char* strA, const char* strB)
// 	{
// 		size_t lenA = strlen(strA);
// 		size_t lenB = strlen(strB);
// 		char* ret = (char*)malloc(lenA + lenB + 1);
// 		strcpy(ret, strA);
// 		strcat(ret, strB);
// 		return ret;
// 	}
// 	void strfree(char* str)
// 	{
// 		free(str);
// 	}

	char* CShmMemory::m_pszShmMemory = NULL;
	CShmMemory::CShmMemory()
	{
		m_pszAvailableShm = m_pszShmMemory + sizeof(*this);
		m_enMode = shmmode_resume;
	}

	CShmMemory::CShmMemory(key_t key, size_t size)
	{
		m_pszAvailableShm = m_pszShmMemory + sizeof(*this);

		if (!check(key, size))
		{
			Initialize(key, size);
			return;
		}
		else
		{
			m_enMode = shmmode_resume;
		}

	}

	CShmMemory::~CShmMemory()
	{

	}

	void* CShmMemory::create_segment(size_t size)
	{
		if (!size)
		{
			return NULL;
		}

		if (size > get_free_size())
		{
			return NULL;
		}

		char* pMem = m_pszAvailableShm;
		m_pszAvailableShm += size;

		return (void*)pMem;
	}

	size_t CShmMemory::get_free_size()
	{
		size_t used = get_used_size();
		return (m_unShmSize - used);
	}

	size_t CShmMemory::get_used_size()
	{
		return (size_t)(m_pszAvailableShm - (char*)this);
	}

	bool CShmMemory::Initialize(key_t key, size_t size)
	{
		m_unShmKey = key;
		m_unShmSize = size;
		strncpy(m_sMagic, SHAREDMEMORY_MAGIC, sizeof(m_sMagic));
		m_enMode = shmmode_init;
		return true;
	}

// 	void* CShmMemory::operator new(size_t size) throw()
// 	{
// 
// 		if (NULL == m_pszShmMemory)
// 		{
// 			return NULL;
// 		}
// 
// 		return(void*)m_pszShmMemory;
// 	}
// 
// 	void CShmMemory::operator delete(void *pMemory)
// 	{
// 		//do nothing here!
// 	}

	bool CShmMemory::check(key_t key, size_t size)
	{
		return((size == m_unShmSize) && (strncmp(m_sMagic, SHAREDMEMORY_MAGIC, sizeof(m_sMagic)) == 0) && (m_unShmKey == key));
	}

	CShmMemory::shm_mode CShmMemory::get_mode()
	{
		return m_enMode;
	}


	CShmMemory* create_shm_memory(key_t key, size_t size)
	{
		if (!size || !key)
		{
			return NULL;
		}

		size_t iRealSize = size + sizeof(CShmMemory);


#ifdef AVALON_OS_WINDOWS
		//bool success = false;
		char* mapped = NULL;
		bool created = false;

		HANDLE hMapFile;
		char szName[128];
		strncpy_s(szName, "Global\\", sizeof(szName));
		sprintf(szName, "Global\\%08d", key);

		hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, (const char*)szName);
		if (hMapFile == NULL)
		{
			hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE | SEC_COMMIT, 0, (DWORD)iRealSize, (const char*)szName);
			if (NULL != hMapFile)
			{
				created = true;
			}
			else
			{
				DWORD err = GetLastError();
				return NULL;
			}
		}

		if (NULL != hMapFile)
		{
			mapped = (char*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, iRealSize);
		}

		if (NULL == mapped)
		{
			if (NULL != hMapFile)
			{
				CloseHandle(hMapFile);
			}
			return NULL;
		}

		CShmMemory::m_pszShmMemory = mapped;
		return new(CShmMemory::m_pszShmMemory) CShmMemory(key, iRealSize);
#else
		//linux os
		//Int32 iKey = StringUtil::ConvertToValue(key);
		Int32 iShmID = 0;

		iShmID = shmget(key, iRealSize, IPC_CREAT | IPC_EXCL | 0666);
		if (iShmID < 0)
		{
			if (errno != EEXIST)
			{
				return NULL;
			}

			iShmID = shmget(key, iRealSize, 0666);
			if (iShmID < 0)
			{
				iShmID = shmget(key, 0, 0666);
				if (iShmID < 0)
				{
					return NULL;
				}
				else
				{
					if (shmctl(iShmID, IPC_RMID, NULL))
					{
						return NULL;
					}

					iShmID = shmget(key, iRealSize, IPC_CREAT | IPC_EXCL | 0666);
					if (iShmID < 0)
					{
						return NULL;
					}
				}
			}
			else
			{
				//attach the existed shared memory
			}

		}

		CShmMemory::m_pszShmMemory = (char*)shmat(iShmID, NULL, 0);
		if (NULL == CShmMemory::m_pszShmMemory)
		{
			return NULL;
		}

		return new(CShmMemory::m_pszShmMemory) CShmMemory(key, iRealSize);
#endif

		return NULL;
	}

	//TODO: to complete the routine for windows platform, and now  just for compiling
	Int32 destroy_shm_memory(key_t unKey)
	{
#ifndef AVALON_OS_WINDOWS
		int iShmID = 0;

		iShmID = shmget(unKey, 0, 0666);
		if (iShmID < 0)
		{
			return AVALON_FAIL;
		}

		if (shmctl(iShmID, IPC_RMID, NULL))
		{
			return AVALON_FAIL;
		}
#endif
		return AVALON_SUCCESS;
	}

}//end of namespace Avalon

//#endif
