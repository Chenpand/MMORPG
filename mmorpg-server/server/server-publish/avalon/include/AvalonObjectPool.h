/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	�����
 */
#ifndef _AVALON_OBJECT_POOL_H_
#define _AVALON_OBJECT_POOL_H_

#include <new>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "AvalonDefine.h"
#include "AvalonMemoryTracker.h"

namespace Avalon
{
	/**
	 *@brief ����
	 */
	class NullMutex
	{
	public:
		void Acquire(){}

		void Release(){}
	};

	/**
	 *@brief �ڴ�ؽӿ�
	 */
	struct MemPoolInterface
	{
		virtual UInt32 GetNodeCount() const = 0;
		virtual UInt32 GetFreeCount() const = 0;
		virtual size_t GetBlockSize() const = 0;
		virtual const std::string& GetNodeName() const = 0;
	};

	/**
	 *@brief �ڴ��  ÿ�η���̶���С�ڴ�飨m_BlockSize��С��
	 */
	template<typename MutexT = NullMutex>
	class MemPool : public MemPoolInterface
	{
		struct ListNode
		{
			// ��ǣ��Ƿ��Ѿ������ȥ
			UInt32		flag;
			ListNode*	next;
			size_t		data; //��������ʼռλ��
		};

#define AVALON_POOLDATA_OFFSET ((char*)&((ListNode*)1)->data - (char*)1)

	public:
		explicit MemPool(size_t size) :m_pFreeList(NULL), m_BlockSize(size), m_NodeCount(0),
			m_FreeCount(0) { MEMORY_OBJECT_ALLOC(this); }
		~MemPool()
		{
			m_Mutex.Acquire();
			while(m_pFreeList != NULL)
			{
				ListNode* node = m_pFreeList;
				m_pFreeList = m_pFreeList->next;
				free(node);

				--m_NodeCount;
				--m_FreeCount;
			}
			m_Mutex.Release();
		}

	public:
		/**
		 *@brief �����ڴ�
		 */
		void* Alloc();

		/**
		 *@brief �ͷ��ڴ�
		 */
		void Free(void* ptr);

		/**
		 *@brief ��ȡ�����ڴ���С
		 */
		virtual size_t GetBlockSize() const { return m_BlockSize; }

		/**
		 *@brief �����ڴ�������
		 */
		void SetNodeName(const char* name) { m_NodeName = name; }

		/**
		 *@brief ��ȡ�ڴ�������
		 */
		virtual UInt32 GetNodeCount() const { return m_NodeCount; }

		/**
		 *@brief ��ȡ�ڴ��������
		 */
		virtual UInt32 GetFreeCount() const { return m_FreeCount; }

		/**
		 *@brief ��ȡ�ڴ�������
		 */
		virtual const std::string& GetNodeName() const { return m_NodeName; }

	private:
		/**
		 *@brief �ͷ��ڴ��
		 */
		void FreePartMemoryPool();

	private:
		//�����б�
		ListNode*	m_pFreeList;  //�����ڴ���б����ȴӴ��б���䣬���Ϊ�գ��Ŵ�ϵͳ���䡣
		//������
		MutexT		m_Mutex;
		//���С
		size_t		m_BlockSize;
		//�ڵ�����
		UInt32		m_NodeCount;
		//��������
		UInt32		m_FreeCount;
		//�ڵ�����
		std::string m_NodeName;
	};

	template<typename MutexT>
	void* MemPool<MutexT>::Alloc()
	{
		ListNode* node = NULL;
		m_Mutex.Acquire();
		if(m_pFreeList != NULL)
		{
			node = m_pFreeList;
			m_pFreeList = m_pFreeList->next;

			--m_FreeCount;
		}

		assert(!node || node->flag == 0);
		if (node)
		{
			node->flag = 1;
		}
		m_Mutex.Release();
		if (NULL == node)
		{
			node = (ListNode*)malloc(sizeof(ListNode)+m_BlockSize);
			node->flag = 1;

			++m_NodeCount;
		}

		return node != NULL ? &node->data : NULL;
	}

	template<typename MutexT>
	void MemPool<MutexT>::Free(void *ptr)
	{
		const static size_t offset = AVALON_POOLDATA_OFFSET;
		ListNode* node = (ListNode*)((char*)ptr - offset);
		assert(node->flag == 1);
		m_Mutex.Acquire();
		FreePartMemoryPool();

		node->flag = 0;
		node->next = m_pFreeList;
		m_pFreeList = node;
		
		++m_FreeCount;
		m_Mutex.Release();
	}

	template<typename MutexT>
	void MemPool<MutexT>::FreePartMemoryPool()
	{
		if (m_FreeCount <= 100)
		{
			return;
		}

		UInt32 count = 0;
		while (m_pFreeList != NULL)
		{
			if (count >= 10)
			{
				break;
			}

			ListNode* node = m_pFreeList;
			m_pFreeList = m_pFreeList->next;
			free(node);

			--m_NodeCount;
			--m_FreeCount;
			++count;
		}
	}

	/**
	 *@brief �����
	 */
	template<typename T, typename MutexT = NullMutex>
	class ObjectPool
	{
	public:
		/**
		 *@brief �����ͷ��ڴ�
		 */
		static void* Alloc(){ return m_pPool->Alloc(); }
		static void Free(void* ptr){ return m_pPool->Free(ptr); }
					
		/**
		 *@brief �ڴ�ռ�úͿ���
		 */
		static UInt32 GetNodeCount() { return m_pPool->GetNodeCount(); }
		static UInt32 GetFreeCount() { return m_pPool->GetFreeCount(); }

		static void SetMemObjectName(const char* name) { m_pPool->SetNodeName(name); }

	private:
		//�ڴ����
		static MemPool<MutexT>*	m_pPool;
	};

	template<typename T, typename MutexT>
    MemPool<MutexT>* ObjectPool<T, MutexT>::m_pPool = new MemPool<MutexT>(sizeof(T));


    /**
     *@brief �����ڲ�ʹ�ã�ʹ�����ػ�
     */
#ifdef _CLOSE_OBJECTPOOL_
#define AVALON_DEFINE_OBJECTPOOL(T, MutexT) \
	public: \
	static UInt32 GetNodeCount() { return 0; } \
	static UInt32 GetFreeCount() { return 0; }
#else
#define AVALON_DEFINE_OBJECTPOOL(T,MutexT) \
	public: \
	static void* operator new(size_t size){ \
		if(size != sizeof(T)) return ::operator new(size); \
		while(true) {\
			Avalon::ObjectPool<T,MutexT>::SetMemObjectName(#T); \
			void* ret = Avalon::ObjectPool<T,MutexT>::Alloc(); \
			if(ret != NULL) { \
                RECORD_MEMORY_ALLOC(ret, size, #T); \
                return ret; \
            } \
			std::new_handler globalhandler = std::set_new_handler(0); \
			std::set_new_handler(globalhandler); \
			if(globalhandler) (*globalhandler)(); \
			else throw std::bad_alloc(); \
		};\
	} \
	static void operator delete(void *ptr,size_t size) { \
		if(NULL == ptr) return; \
		if(size != sizeof(T)) return ::operator delete(ptr); \
		Avalon::ObjectPool<T,MutexT>::Free(ptr); \
        RECORD_MEMORY_RELEASE(ptr); \
	} \
	static UInt32 GetNodeCount() { \
		return Avalon::ObjectPool<T, MutexT>::GetNodeCount(); \
	} \
	static UInt32 GetFreeCount() { \
		return Avalon::ObjectPool<T, MutexT>::GetFreeCount(); \
	}
#endif


	/**
	 *@brief �����ڴ汣������
	 */
	enum OBJMEM_PROTECT_TYPE
	{
		OBJMEM_PROTECT_HEAD = 0,	//ͷ�������ܱ���
		OBJMEM_PROTECT_TAIL = 1,	//β�������ܱ���
		OBJMEM_PROTECT_AMONG = 2,	//�м�ָ������
	};
	/**
	 *@brief �ܱ����Ķ����ڴ������
	 */
	template<typename T, OBJMEM_PROTECT_TYPE type>
	class ProtectObjMemAllocator
	{
	public:
		/**
		 *@brief �����ͷŶ����ڴ�
		 */
		static void* Alloc();
		static void Free(void* ptr);
	public:
		static int m_MemberOffset;
	};

#ifdef AVALON_OS_WINDOWS

	template<typename T, OBJMEM_PROTECT_TYPE type>
	void* ProtectObjMemAllocator<T, type>::Alloc()
	{
		return malloc(sizeof(T));
	}

	template<typename T, OBJMEM_PROTECT_TYPE type>
	void ProtectObjMemAllocator<T, type>::Free(void* ptr)
	{
		return free(ptr);
	}

	template<typename T, OBJMEM_PROTECT_TYPE type>
	int ProtectObjMemAllocator<T , type>::m_MemberOffset = 0;

#define	AVALON_DEFINE_PROTECT_MEMBER \
	public: \
	char _PROTECT_MEMBER; \
	private: 

#else

#include <fcntl.h>
//#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

	template<typename T, OBJMEM_PROTECT_TYPE type>
	void* ProtectObjMemAllocator<T, type>::Alloc()
	{
		int page_size = getpagesize();
		int fd = open ("/dev/zero", O_RDONLY);
		void* ret = NULL;
		if(type == OBJMEM_PROTECT_HEAD)
		{
			int alloc_size = page_size + ((sizeof(T) - 1) / page_size + 1) * page_size;
			void* mem = (char*)mmap(NULL, alloc_size, PROT_WRITE|PROT_READ|PROT_EXEC, MAP_PRIVATE, fd, 0);
			if(mem != MAP_FAILED)
			{
				if(mprotect(mem, page_size, PROT_NONE) == 0) ret = (char*)mem + page_size;
				else munmap(mem, alloc_size);
			}
		}
		else if(type == OBJMEM_PROTECT_TAIL)
		{
			int alloc_size = page_size + ((sizeof(T) - 1) / page_size + 1) * page_size;
			void* mem = (char*)mmap(NULL, alloc_size, PROT_WRITE|PROT_READ|PROT_EXEC, MAP_PRIVATE, fd, 0);
			if(mem != MAP_FAILED)
			{
				if(mprotect((char*)mem + alloc_size - page_size, page_size, PROT_NONE) == 0)
					ret = (char*)mem + alloc_size - page_size - sizeof(T);
				else munmap(mem, alloc_size);
			}
		}
		else if(type == OBJMEM_PROTECT_AMONG)
		{
			int alloc_size = page_size;
			int protect_offset = 0;
			if(m_MemberOffset > 0)
			{
				protect_offset = ((m_MemberOffset - 1) / page_size + 1) * page_size;
				alloc_size += protect_offset;
			}
			if(m_MemberOffset < int(sizeof(T)) - page_size)
			{
				alloc_size += ((sizeof(T) - page_size - m_MemberOffset - 1) / page_size + 1) * page_size;
			}
			void* mem = (char*)mmap(NULL, alloc_size, PROT_WRITE|PROT_READ|PROT_EXEC, MAP_PRIVATE, fd, 0);
			if(mem != MAP_FAILED)
			{
				if(mprotect((char*)mem + protect_offset, page_size, PROT_NONE) == 0) ret = (char*)mem + protect_offset - m_MemberOffset;
				else munmap(mem, alloc_size);
			}
		}
		close(fd);
		return ret;
	}

	template<typename T, OBJMEM_PROTECT_TYPE type>
	void ProtectObjMemAllocator<T, type>::Free(void* ptr)
	{
		int page_size = getpagesize();
		void* mem = NULL;
		int alloc_size = 0;
		if(type == OBJMEM_PROTECT_HEAD)
		{
			alloc_size = page_size + ((sizeof(T) - 1) / page_size + 1) * page_size;
			mem = (char*)ptr - page_size;

		}
		else if(type == OBJMEM_PROTECT_TAIL)
		{
			alloc_size = page_size + ((sizeof(T) - 1) / page_size + 1) * page_size;
			mem = (char*)ptr - (alloc_size - page_size - sizeof(T));
		}
		else if(type == OBJMEM_PROTECT_AMONG)
		{
			alloc_size = page_size;
			int protect_offset = 0;
			if(m_MemberOffset > 0)
			{
				protect_offset = ((m_MemberOffset - 1) / page_size + 1) * page_size;
				alloc_size += protect_offset;
			}
			if(m_MemberOffset < int(sizeof(T)) - page_size)
			{
				alloc_size += ((sizeof(T) - page_size - m_MemberOffset - 1) / page_size + 1) * page_size;
			}
			mem = (char*)ptr - (protect_offset - m_MemberOffset);
		}
		mprotect(mem, alloc_size, PROT_READ|PROT_WRITE|PROT_EXEC);
		munmap(mem, alloc_size);
	}
	
	template<typename T, OBJMEM_PROTECT_TYPE type>
	int ProtectObjMemAllocator<T , type>::m_MemberOffset = ((char*)&((T*)1)->_PROTECT_MEMBER - (char*)1);

#define	AVALON_DEFINE_PROTECT_MEMBER \
	public: \
	char _PROTECT_MEMBER[4096]; \
	private:

#endif

#define AVALON_DEFINE_HEADPROTECT_ALLOCATOR(T) \
	public: \
	static void* operator new(size_t size){ \
		if(size != sizeof(T)) return ::operator new(size); \
		while(true) {\
			void* ret = Avalon::ProtectObjMemAllocator<T,Avalon::OBJMEM_PROTECT_HEAD>::Alloc(); \
			if(ret != NULL) return ret; \
			std::new_handler globalhandler = std::set_new_handler(0); \
			std::set_new_handler(globalhandler); \
			if(globalhandler) (*globalhandler)(); \
			else throw std::bad_alloc(); \
		};\
	} \
	static void operator delete(void *ptr,size_t size) { \
		if(NULL == ptr) return; \
		if(size != sizeof(T)) return ::operator delete(ptr); \
		Avalon::ProtectObjMemAllocator<T,Avalon::OBJMEM_PROTECT_HEAD>::Free(ptr); \
	} \
	char _PROTECT_MEMBER;

#define AVALON_DEFINE_TAILPROTECT_ALLOCATOR(T) \
	public: \
	static void* operator new(size_t size){ \
		if(size != sizeof(T)) return ::operator new(size); \
		while(true) {\
			void* ret = Avalon::ProtectObjMemAllocator<T,Avalon::OBJMEM_PROTECT_TAIL>::Alloc(); \
			if(ret != NULL) return ret; \
			std::new_handler globalhandler = std::set_new_handler(0); \
			std::set_new_handler(globalhandler); \
			if(globalhandler) (*globalhandler)(); \
			else throw std::bad_alloc(); \
		};\
	} \
	static void operator delete(void *ptr,size_t size) { \
		if(NULL == ptr) return; \
		if(size != sizeof(T)) return ::operator delete(ptr); \
		Avalon::ProtectObjMemAllocator<T,Avalon::OBJMEM_PROTECT_TAIL>::Free(ptr); \
	} \
	char _PROTECT_MEMBER;

#define AVALON_DEFINE_AMONGPROTECT_ALLOCATOR(T) \
	public: \
	static void* operator new(size_t size){ \
		if(size != sizeof(T)) return ::operator new(size); \
		while(true) {\
			void* ret = Avalon::ProtectObjMemAllocator<T,Avalon::OBJMEM_PROTECT_AMONG>::Alloc(); \
			if(ret != NULL) return ret; \
			std::new_handler globalhandler = std::set_new_handler(0); \
			std::set_new_handler(globalhandler); \
			if(globalhandler) (*globalhandler)(); \
			else throw std::bad_alloc(); \
		};\
	} \
	static void operator delete(void *ptr,size_t size) { \
		if(NULL == ptr) return; \
		if(size != sizeof(T)) return ::operator delete(ptr); \
		Avalon::ProtectObjMemAllocator<T,Avalon::OBJMEM_PROTECT_AMONG>::Free(ptr); \
	}

}

#endif
