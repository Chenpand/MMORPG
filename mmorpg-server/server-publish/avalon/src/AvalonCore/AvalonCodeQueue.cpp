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
		m_iSize = size; //���������õ��ܳ���
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
			//�����������ж����е�����
			head = tail = 0;
			set_boundary(head, tail);
			return fail;
		}

		Int32 free_size = get_freesize();

		//ÿ����Ϣcode����ǰ��4ByteΪ��Ϣcode�ĳ���(��������4��Byte)
		//pop()������ҲĬ����һ����
		if ((Int32)sizeof(Int32)+size > free_size)
		{
			return fail;
		}

		char* pszDst = get_codebuffer();
		char* pszSrc = (char*)&size;

		for (size_t i = 0; i < sizeof(Int32); ++i)
		{
			pszDst[tail] = pszSrc[i];
			tail = (tail + 1) % m_iSize;  //ע�⣺��֤m_iSize����Ϊ0!!
		}

		if (head > tail)//
		{
			memcpy(&pszDst[tail], (const void*)code, (size_t)size);
		}
		else
		{
			if (size > (m_iSize - tail))
			{
				//��Ҫ�ֶο���
				memcpy(&pszDst[tail], (const void*)code, (size_t)(m_iSize - tail));
				memcpy(&pszDst[0], (const void*)(code + (m_iSize - tail)), (size_t)(size - (m_iSize - tail)));
			}
			else
			{
				memcpy(&pszDst[tail], (const void*)code, (size_t)size);
			}
		}

		tail = (tail + size) % m_iSize;

		//������m_iTail
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

		//���code����4��Byte,��ζ�Ŷ�����
		if (code_size < (Int32)sizeof(Int32))
		{
			//�Ѷ������
			outlength = 0;
			head = tail = 0;
			set_boundary(head, tail);
			return fail;
		}

		//Ԥ�ȱ������buffer�Ĵ�С
		Int32 buffer_size = outlength;

		//ʹ��real_outlength����ʵ��code�ĳ���
		Int32 real_outlength = 0;
		char* pszDst = (char*)&real_outlength;

		char* code_zone = get_codebuffer();
		if (NULL == code_zone)
		{
			return fail;
		}

		//����ȡcode�ĳ���(Ҳ����ǰ4Byte)
		for (size_t i = 0; i < sizeof(Int32); ++i)
		{
			pszDst[i] = code_zone[head];
			head = (head + 1) % m_iSize;
		}

		//����Ӷ����ж�ȡ��code���Ȳ�����Ҫ��
		if (real_outlength <= 0 || real_outlength >(code_size - (Int32)sizeof(Int32)))
		{
			outlength = 0;
			head = tail = 0;
			set_boundary(head, tail);
			return fail;
		}

		//���buffer����
		if (buffer_size < real_outlength)
		{
			//�����Ƿ���.���������Ƿ��׵�? real_outlength�ĸ��ֽڵĳ�����connectorsvrd����
			//Ӧ���ǿ��ŵ�
			head = (head + real_outlength) % m_iSize;
			set_boundary(head, -1);//������headָ��
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
			//�����ǰcode���ֶַ�
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

		//�ƶ�ͷָ��
		head = (head + outlength) % m_iSize;
		set_boundary(head, -1);//������headָ��

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

		if (head == tail)//�ն���
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

		//ȥ��Ԥ���Ŀռ�
		free_size -= reserved_length;
		if (free_size < 0)
		{
			free_size = 0;
		}

		return free_size;
	}

	Int32 CCodeQueue::get_codesize(void) const
	{
		//����get_freesize�ķ���ֵ����>=0(������ָ�ֵ)�����Ըú��������Ĵ�������ȷ��
		return (m_iSize - get_freesize() - reserved_length);
	}

	void* CCodeQueue::operator new(size_t size)throw()
	{
		if (NULL == m_pstSharedMemory)
		{
			return NULL;
		}

		//ΪCCodeQueue�����������ռ�
		return (void*)(m_pstSharedMemory->create_segment((Int32)size));
	}

	void CCodeQueue::operator delete(void *pbuffer)
	{
		//do nothing,here!
	}

}//end of namespace Avalon

//#endif
