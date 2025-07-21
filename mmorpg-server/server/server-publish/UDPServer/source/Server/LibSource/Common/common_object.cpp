
#include "Common/common_base.h"
#include "Common/common_memory.h"
#include "Common/common_object.h"

SERVER_BEGIN

/************************************************************************/
/* CObjectPool                                                          */
/************************************************************************/
CShmMemory* CObjectPool::m_pstMemory = NULL;


size_t CObjectPool::size(int32_t objectcount, size_t objectsize)
{
	size_t total = sizeof(CObjectPool);
	total += objectcount * sizeof(CObjectIndex);
	total += objectcount * objectsize;

	return total;
}

void CObjectPool::set_memory(CShmMemory* memory)
{
	m_pstMemory = memory;
}

CObjectPool::CObjectPool(int32_t objectcount, size_t objectsize, CreateObjectFunction create_method)
{
	if (NULL == m_pstMemory)
	{
		return;
	}

	if (CShmMemory::shm_mode_init == m_pstMemory->get_mode())
	{
		m_iObjectCount = objectcount;
		m_uiObjectSize = objectsize;
	}

	m_pstIndex = (CObjectIndex*)(m_pstMemory->create_segment( sizeof(CObjectIndex)*m_iObjectCount));
	if (NULL == m_pstIndex)
	{
		return;
	}

	m_pstObject = (CObject*)(m_pstMemory->create_segment(m_iObjectCount * (int32_t)m_uiObjectSize));
	if (NULL == m_pstObject)
	{
		return;
	}
	
	m_pfCreator = create_method;

	if (CShmMemory::shm_mode_init == m_pstMemory->get_mode())
	{
		initialize();
		CObject::m_iCreateMode = object_createmode_init;
	}
	else
	{
		CObject::m_iCreateMode = object_createmode_resume;
	}

	//格式化对象池
	format_objects();

	return;
}

CObjectPool::~CObjectPool()
{
	//do nothing
}


int32_t CObjectPool::initialize(void)
{
	m_iFreeHead = invalid_object_index;
	m_iUsedHead = invalid_object_index;
	m_iUsedCount = 0;

	format_index();
	m_nIsFormat = object_pool_formated;

	return success;
}

int32_t CObjectPool::format_objects()
{
	if (NULL == m_pfCreator)
	{
		return fail;
	}

	CObject* pstObject = NULL;
	for (int32_t i = 0; i < m_iObjectCount; ++i)
	{
		pstObject = (*m_pfCreator)((char*)m_pstObject + m_uiObjectSize*i);
        pstObject->m_iObjectID = i;
		m_pstIndex[i].set_object(pstObject);
	}

	return success;
}

int32_t CObjectPool::format_index()
{
	m_iFreeHead = 0;
	m_iUsedHead = invalid_object_index;

	//单独处理第0个索引
	m_pstIndex[m_iFreeHead].initialize();
	m_pstIndex[m_iFreeHead].set_prev(invalid_object_index);
	
	for (int32_t i = 1; i < m_iObjectCount; ++i)
	{
		m_pstIndex[i].initialize();
		m_pstIndex[i-1].set_next(i);
		m_pstIndex[i].set_prev(i-1);
	}

	//最后一个索引的下一个是m_pstIndex[m_iFreeHead]
	m_pstIndex[m_iObjectCount-1].set_next(invalid_object_index);

	return success;

}

void CObjectPool::set_create_object_method(CreateObjectFunction method)
{
	m_pfCreator = method;
}

int32_t CObjectPool::create_object()
{
	int32_t allocated_index = allocate_one_object();
	if (invalid_object_index == allocated_index)
	{
		return allocated_index;
	}

	CObject* allocated_object = get_object(allocated_index);
	allocated_object->initialize();

	return allocated_index;
}

int32_t CObjectPool::destroy_object(int32_t object_index)
{
	if (!is_valid(object_index))
	{
		return invalid_object_index;
	}

	if (!m_pstIndex[object_index].is_used())
	{
		return invalid_object_index;
	}

	//
	if (object_index == m_iUsedHead)//如果待回收的对象是已分配对象链的首对象
	{
		m_iUsedHead = m_pstIndex[m_iUsedHead].get_next();
	}

	if (m_pstIndex[object_index].get_next() >= 0) //待回收对象有后续对象
	{
		m_pstIndex[m_pstIndex[object_index].get_next()].set_prev(m_pstIndex[object_index].get_prev());
	}

	if (m_pstIndex[object_index].get_prev() >= 0)//待回收对象有前置对象
	{
		m_pstIndex[m_pstIndex[object_index].get_prev()].set_next(m_pstIndex[object_index].get_next());
	}

	//释放并插入空闲对象链首部
	m_pstIndex[object_index].set_free();
	m_pstIndex[object_index].set_prev(invalid_object_index);
	m_pstIndex[object_index].set_next(m_iFreeHead);
	if (m_iFreeHead >= 0)
	{
		m_pstIndex[m_iFreeHead].set_prev(object_index);
	}

	m_iFreeHead = object_index;
	m_iUsedCount--;

	return object_index;
}

CObject* CObjectPool::get_object(int32_t object_index)
{
	if (!is_valid(object_index))
	{
		return NULL;
	}

	if (!m_pstIndex[object_index].is_used())
	{
		return NULL;
	}

	return m_pstIndex[object_index].get_object();
}

//注意：该函数判断了对应的CObjectIndex是否是已分配了
CObjectIndex* CObjectPool::get_index(int32_t object_index)
{
	if (!is_valid(object_index))
	{
		return NULL;
	}

	if (!m_pstIndex[object_index].is_used())
	{
		return NULL;
	}

	return &m_pstIndex[object_index];
}

void* CObjectPool::operator new(size_t size)throw()
{
	if (NULL == m_pstMemory)
	{
		return NULL;
	}

	char* tmp = (char*)m_pstMemory->create_segment((int32_t)size);
	return (void*)tmp;
}

void CObjectPool::operator delete(void *pobject)
{
	//do nothing here.
	return;
}

int32_t CObjectPool::get_object_id(CObject* object)
{
	if (NULL == object)
	{
		return invalid_object_id;
	}

	if ((char*)object < (char*)m_pstObject)
	{
		return invalid_object_id;
	}

	if ((char*)object > (char*)((char*)m_pstObject + m_uiObjectSize*m_iObjectCount))
	{
		return invalid_object_id;
	}

	return(int32_t)(((char*)object -(char*)m_pstObject)/m_uiObjectSize);
}

char* CObjectPool::allocate_memory_for_object()
{
	int32_t index = create_object();
	if (invalid_object_index == index)
	{
		return NULL;
	}

	//return(char*)m_pstIndex[index].get_object();
	return (char*)get_object(index);
}

int32_t CObjectPool::allocate_one_object( void )
{
	int32_t allocated_index = invalid_object_index;

	if (full())
	{
		return allocated_index;
	}

	allocated_index = m_iFreeHead;
	m_iFreeHead = m_pstIndex[m_iFreeHead].get_next();
	if (m_iFreeHead >= 0)
	{
		m_pstIndex[m_iFreeHead].set_prev(invalid_object_index); //设置空闲链的起始对象的前置对象为无效
	}

	//分配对象，并放入已分配对象队列的头部
	m_pstIndex[allocated_index].set_used();
	m_pstIndex[allocated_index].set_next(m_iUsedHead);
	m_pstIndex[allocated_index].set_prev(invalid_object_index);

	if (m_iUsedHead >= 0)
	{
		m_pstIndex[m_iUsedHead].set_prev(allocated_index);
	}

	m_iUsedHead = allocated_index;
	m_pstIndex[allocated_index].clear_additional_data();
	m_iUsedCount++;

	return allocated_index;

}


bool CObjectPool::is_valid(int32_t objectindex)
{
	return (objectindex >= 0 && objectindex < m_iObjectCount);
}


/************************************************************************/
/*CObjectIndex                                                          */
/************************************************************************/
CObjectIndex::CObjectIndex()
{
	initialize();
}

CObjectIndex::~CObjectIndex()
{
	//do nothing here
}

int32_t CObjectIndex::initialize()
{
	m_iPrev = invalid_object_index;
	m_iNext = invalid_object_index;

	m_iFlag = object_index_free;
	m_stObject = NULL;

	clear_additional_data();

	return success;
}

void CObjectIndex::clear_additional_data(void)
{
	for (size_t i = 0; i < sizeof(m_iAdditionData)/sizeof(m_iAdditionData[0]); ++i)
	{
		m_iAdditionData[i] = invalid_object_index;
	}

	return;
}

void CObjectIndex::set_free()
{
	m_iFlag = object_index_free;
}

void CObjectIndex::set_used()
{
	m_iFlag = object_index_used;
}

void CObjectIndex::set_next(int32_t next)
{
	m_iNext = next;
}

void CObjectIndex::set_prev(int32_t prev)
{
	m_iPrev = prev;
}

int32_t CObjectIndex::get_next()
{
	return m_iNext;
}

int32_t CObjectIndex::get_prev()
{
	return m_iPrev;
}

int32_t CObjectIndex::get_additional_data_of(int32_t index, int32_t& value)
{
	if (index < 0 || index >= max_additional_data_size)
	{
		return fail;
	}

	value = m_iAdditionData[index];
	return success;
}

int32_t CObjectIndex::set_additional_data_of(int32_t index, int32_t value)
{
	if (index < 0 || index >= max_additional_data_size)
	{
		return fail;
	}

	m_iAdditionData[index] = value;
	return success;
}

int32_t CObjectIndex::set_object(CObject *object)
{
	//没有检查传入的形参object是否为空
	m_stObject = object;
	return success;
}

/************************************************************************/
/*CObject                                                               */
/************************************************************************/
int32_t CObject::m_iCreateMode = object_createmode_init;
char* CObject::pszMemory = NULL;

void* CObject::operator new(size_t size)throw()
{
	if (NULL == pszMemory)
	{
		return NULL;
	}

	return(void*)pszMemory;
}

void CObject::operator delete(void *object)
{
	//do nothing
	return;
}

SERVER_END
