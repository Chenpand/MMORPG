#include "AvalonCodeQueue.h"
#include "AvalonSharedMemory.h"

//#ifndef AVALON_OS_WINDOWS
namespace Avalon
{
	CShmMemory* CCodeQueue::m_pstSharedMemory = NULL;
	void CCodeQueue::set_sharedmemory(CShmMemory* pMem)
	{
		m_pstSharedMemory = pMem;
	}

	size_t CCodeQueue::size(Int32 buffersize)
	{
		size_t itotal = sizeof(CCodeQueue);
		if (buffersize > 0)
		{
			itotal += buffersize;
		}

		return itotal;
	}


	CCodeQueue::CCodeQueue(Int32 size)
	{
		if (NULL == m_pstSharedMemory)
		{
			m_iSize = 0;
			m_iHead = 0;
			m_iTail = 0;
			m_iCodeOffset = invalid_offset;
		}

		if (m_pstSharedMemory->get_mode() == CShmMemory::shmmode_init)
		{
			initialize(size);
		}
		else
		{
			resume(size);
		}
	}

	CCodeQueue::~CCodeQueue()
	{

	}

	Int32 CCodeQueue::initialize(Int32 size)
	{
		char* code_zone = (char*)m_pstSharedMemory->create_segment(size);
		if (NULL == code_zone)
		{
			return fail;
		}

		m_iCodeOffset = Int32(code_zone - (char*)this);
		m_iSize = size; //调用者设置的总长度
		m_iHead = 0;
		m_iTail = 0;

		return success;
	}

	Int32 CCodeQueue::resume(Int32 size)
	{
		if (NULL == m_pstSharedMemory)
		{
			return fail;
		}

		m_pstSharedMemory->create_segment(size);

		return success;
	}

	Int32 CCodeQueue::append(const char* code, Int32 size)
	{
		if (invalid_offset == code_offset() || m_iSize <= 0)
		{
			return fail;
		}

		if (full())
		{
			return fail;
		}

		Int32 head = 0;
		Int32 tail = 0;

		get_boundary(head, tail);
		if (head < 0 || head >= m_iSize || tail < 0 || tail >= m_iSize)
		{
			//出错后，清除所有队列中的数据
			head = tail = 0;
			set_boundary(head, tail);
			return fail;
		}

		Int32 free_size = get_freesize();

		//每个消息code的最前面4Byte为消息code的长度(不包含这4个Byte)
		//pop()函数中也默认这一规则
		if ((Int32)sizeof(Int32)+size > free_size)
		{
			return fail;
		}

		char* pszDst = get_codebuffer();
		char* pszSrc = (char*)&size;

		for (size_t i = 0; i < sizeof(Int32); ++i)
		{
			pszDst[tail] = pszSrc[i];
			tail = (tail + 1) % m_iSize;  //注意：保证m_iSize不能为0!!
		}

		if (head > tail)//
		{
			memcpy(&pszDst[tail], (const void*)code, (size_t)size);
		}
		else
		{
			if (size > (m_iSize - tail))
			{
				//需要分段拷贝
				memcpy(&pszDst[tail], (const void*)code, (size_t)(m_iSize - tail));
				memcpy(&pszDst[0], (const void*)(code + (m_iSize - tail)), (size_t)(size - (m_iSize - tail)));
			}
			else
			{
				memcpy(&pszDst[tail], (const void*)code, (size_t)size);
			}
		}

		tail = (tail + size) % m_iSize;

		//仅设置m_iTail
		set_boundary(-1, tail);

		return success;
	}

	Int32 CCodeQueue::pop(char* dst, Int32& outlength)
	{
		if (NULL == dst || 0 >= outlength)
		{
			return fail;
		}

		if (invalid_offset == code_offset() || m_iSize <= 0)
		{
			return fail;
		}

		if (empty())
		{
			return fail;
		}

		Int32 code_size = get_codesize();

		Int32 head = 0;
		Int32 tail = 0;

		get_boundary(head, tail);

		//如果code不足4个Byte,意味着队列损坏
		if (code_size < (Int32)sizeof(Int32))
		{
			//把队列清空
			outlength = 0;
			head = tail = 0;
			set_boundary(head, tail);
			return fail;
		}

		//预先保存输出buffer的大小
		Int32 buffer_size = outlength;

		//使用real_outlength保存实际code的长度
		Int32 real_outlength = 0;
		char* pszDst = (char*)&real_outlength;

		char* code_zone = get_codebuffer();
		if (NULL == code_zone)
		{
			return fail;
		}

		//首先取code的长度(也就是前4Byte)
		for (size_t i = 0; i < sizeof(Int32); ++i)
		{
			pszDst[i] = code_zone[head];
			head = (head + 1) % m_iSize;
		}

		//如果从队列中读取的code长度不符合要求
		if (real_outlength <= 0 || real_outlength >(code_size - (Int32)sizeof(Int32)))
		{
			outlength = 0;
			head = tail = 0;
			set_boundary(head, tail);
			return fail;
		}

		//输出buffer不够
		if (buffer_size < real_outlength)
		{
			//跳过非法包.这样处理是否妥当? real_outlength四个字节的长度是connectorsvrd增加
			//应该是可信的
			head = (head + real_outlength) % m_iSize;
			set_boundary(head, -1);//仅设置head指针
			return fail;
		}

		//copy code buffer
		outlength = real_outlength;
		pszDst = dst;

		if (tail > head)
		{
			memcpy(pszDst, (const void*)&code_zone[head], (size_t)outlength);
		}
		else
		{
			//如果当前code出现分段
			if (outlength > (m_iSize - head))
			{
				memcpy(dst, (const void*)&code_zone[head], (size_t)(m_iSize - head));
				memcpy((void*)&dst[m_iSize - head], (const void*)&code_zone[0], size_t(outlength - (m_iSize - head)));
			}
			else
			{
				memcpy(dst, (const void*)&code_zone[head], size_t(outlength));
			}

		}//else

		//移动头指针
		head = (head + outlength) % m_iSize;
		set_boundary(head, -1);//仅设置head指针

		return success;
	}

	bool CCodeQueue::full(void)
	{
		return!(get_freesize() > 0);
	}

	bool CCodeQueue::empty(void)
	{
		return(m_iHead == m_iTail);
	}

	Int32 CCodeQueue::code_offset(void) const
	{
		return m_iCodeOffset;
	}

	Int32 CCodeQueue::set_boundary(Int32 head, Int32 tail)
	{
		if (head >= 0 && head < m_iSize)
		{
			m_iHead = head;
		}

		if (tail >= 0 && tail < m_iSize)
		{
			m_iTail = tail;
		}

		return 0;
	}

	Int32 CCodeQueue::get_boundary(Int32& head, Int32& tail) const
	{
		head = m_iHead;
		tail = m_iTail;
		return 0;
	}

	char* CCodeQueue::get_codebuffer(void) const
	{
		return((m_iCodeOffset != invalid_offset) ? (char*)((char*)this + m_iCodeOffset) : NULL);
	}

	Int32 CCodeQueue::get_freesize(void) const
	{
		Int32 head = 0;
		Int32 tail = 0;

		get_boundary(head, tail);

		Int32 free_size = 0;

		if (head == tail)//空队列
		{
			free_size = m_iSize;
		}
		else if (head > tail)
		{
			free_size = head - tail;
		}
		else
		{
			free_size = head + (m_iSize - tail);
		}

		//去掉预留的空间
		free_size -= reserved_length;
		if (free_size < 0)
		{
			free_size = 0;
		}

		return free_size;
	}

	Int32 CCodeQueue::get_codesize(void) const
	{
		//由于get_freesize的返回值总是>=0(不会出现负值)，所以该函数这样的处理是正确的
		return (m_iSize - get_freesize() - reserved_length);
	}

	void* CCodeQueue::operator new(size_t size)throw()
	{
		if (NULL == m_pstSharedMemory)
		{
			return NULL;
		}

		//为CCodeQueue对象自身分配空间
		return (void*)(m_pstSharedMemory->create_segment((Int32)size));
	}

	void CCodeQueue::operator delete(void *pbuffer)
	{
		//do nothing,here!
	}

}//end of namespace Avalon

//#endif
