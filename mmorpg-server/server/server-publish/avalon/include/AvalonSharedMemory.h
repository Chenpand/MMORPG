#ifndef _AVALON_SHARED_MEMORY_H_
#define _AVALON_SHARED_MEMORY_H_

#include <AvalonDefine.h>
#include <sys/types.h>

//#ifndef AVALON_OS_WINDOWS

namespace Avalon
{
#define MAX_PATH_NAME_LENGTH	128
#define SHAREDMEMORY_MAGIC		"SMEM"
#define AVALON_SUCCESS		0
#define AVALON_FAIL		-1

	class CShmMemory
	{
	public:
		enum shm_mode
		{
			shmmode_resume = 0,
			shmmode_init = 1,	
		};
	public:
		CShmMemory();
		CShmMemory(key_t key, size_t size);

		~CShmMemory();

// 		void* operator new(size_t size) throw();
// 
// 		void  operator delete(void *pMemory);

		void*  create_segment(size_t size);
		//得到可用空间大小，单位:Byte
		size_t	get_free_size();

		//得到已用空间大小，单位:Byte
		size_t get_used_size();

		bool check(key_t key, size_t size);
		shm_mode get_mode();
	protected:
		bool Initialize(key_t key, size_t size);
	public:
		static char* m_pszShmMemory;
	protected:
	private:
		//char m_sKey[MAX_PATH_NAME_LENGTH];
		key_t m_unShmKey;
		size_t	m_unShmSize;
		char m_sMagic[4];		//magic : SMEM
		char*	m_pszAvailableShm;
		shm_mode m_enMode;
	};

	CShmMemory* create_shm_memory(key_t key, size_t size);
	Int32 destroy_shm_memory(key_t unKey);
}//end of namespace Avalon

#endif


//#endif
