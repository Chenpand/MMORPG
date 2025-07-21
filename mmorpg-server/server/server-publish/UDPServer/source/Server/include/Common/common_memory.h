#ifndef __COMMON_MEMEORY_H__
#define __COMMON_MEMEORY_H__

#include "common_namespace_definition.h"
//#include "common_types.h"

SERVER_BEGIN

/************************************************************************/
/*共享内存管理                                                           */
/*限制：单线															*/
/*																		*/
/************************************************************************/
class CShmMemory
{
public:
	enum 
	{
		shm_mode_resume = 0,
		shm_mode_init = 1

	};
public:
	static char* m_pszShmMemory;
protected:
	//共享内存键
	key_t	m_unShmKey;

	//内存总大小
	size_t	m_iShmSize;
	
	//内存头校验和
	uint32_t m_unCRC;

	//内存创建时间
	time_t	m_tmCreateTime;

	//最后访问时间
	time_t	m_tmLastTimeStamp;

	//可用内存
	char*	m_pszAvailableShm;
	//内存使用模式
	int32_t		m_iShmMode;
public:
	CShmMemory();
	CShmMemory(key_t unKey, size_t size);
	CShmMemory(key_t unKey, size_t iSize, int32_t iInitFlag );
	~CShmMemory();

	//打时间戳，防止内存失效	
	void set_time_stamp();

	void* operator new(size_t iSize) throw();

	void  operator delete(void *pMemory);

	int32_t get_mode();
	
	//分配大小为iSize的内存块，失败返回NULL
	void*  create_segment(size_t iSize);
	
	//得到可用空间大小，单位:Byte
	size_t	get_free_size();

	//得到已用空间大小，单位:Byte
	size_t get_used_size();

protected:

	//int32_t Initialize(key_t unKey, size_t iSize, int32_t iInitFlag = 0);
	int32_t Initialize(key_t unKey, size_t iSize);
private:
};

//创建共享内存
CShmMemory* create_shm_memory(key_t unKey, size_t iSize, int32_t iInitFlag = 0);

//回收共享内存
int32_t destroy_shm_memory(key_t unKey);

SERVER_END

#endif
