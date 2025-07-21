#include "Common/common_base.h"
#include "Common/common_object.h"
#include "Common/common_objectqueue.h"

SERVER_BEGIN



CObjectQueue::CObjectQueue()
{

}

CObjectQueue::CObjectQueue(CObjectPool *pool, int32_t next_index)
{
	if (object_createmode_init == CObject::m_iCreateMode)
	{
		initialize(pool, next_index);
	}
	else
	{
		resume(pool);
	}
}

CObjectQueue::~CObjectQueue()
{
	//do nothing here!
}

int32_t CObjectQueue::initialize(CObjectPool *pool, int32_t next_index)
{
	if (NULL == pool)
	{
		return fail;
	}

	if (next_index < 0 || next_index >= CObjectIndex::max_additional_data_size)
	{
		return fail;
	}
	
	m_pstObjectPool = pool;
	m_iNextIndex = next_index;

	m_iHead = invalid_object_index;
	m_iTail = invalid_object_index;

	return success;
}

int32_t CObjectQueue::resume(CObjectPool* pool)
{
	if (NULL == pool)
	{
		return fail;
	}

	m_pstObjectPool = pool;
	return success;
}


int32_t CObjectQueue::next(int32_t object) const
{
	CObjectIndex *pstIndex = m_pstObjectPool->get_index(object);
	if (NULL == pstIndex)
	{
		return invalid_object_index;
	}
	int32_t value = invalid_object_index;
	//return pstIndex->get_additional_data_of(m_iNextIndex, value); //by april liu 2008-03-18 9:09
	if (success != pstIndex->get_additional_data_of(m_iNextIndex, value))
	{
		return invalid_object_index;
	}

	return value;
}

int32_t CObjectQueue::set_next(int32_t object, int32_t next)
{
	CObjectIndex* pstTmpIndex = m_pstObjectPool->get_index(object);
	if (NULL == pstTmpIndex)
	{
		return fail;
	}

	pstTmpIndex->set_additional_data_of(m_iNextIndex, next);

	return success;
}

int32_t CObjectQueue::append(int32_t object)
{
	CObjectIndex* pstTmpIndex = m_pstObjectPool->get_index(object);
	if (NULL == pstTmpIndex)
	{
		return fail;
	}

	set_next(object, invalid_object_index);

	//若队列为空
	if (invalid_object_index == m_iTail)
	{
		m_iHead = object;
		m_iTail = object;

		return success;
	}

	set_next(m_iTail, object);
	m_iTail = object;

	return success;
}

int32_t CObjectQueue::push(int32_t object)
{
	CObjectIndex* pstTmpIndex = m_pstObjectPool->get_index(object);
	if (NULL == pstTmpIndex)
	{
		return fail;
	}

	set_next(object, m_iHead);

	//若队列为空
	if (invalid_object_index == m_iHead)
	{
		m_iTail = object;
	}

	m_iHead = object;

	return success;
}

int32_t CObjectQueue::insert(int32_t prev, int32_t object)
{
	CObjectIndex* pstTmpIndex = m_pstObjectPool->get_index(object);
	if (NULL == pstTmpIndex)
	{
		return fail;
	}

	CObjectIndex* pstPrevIndex = m_pstObjectPool->get_index(prev);
	if (NULL == pstPrevIndex)
	{
		return fail;
	}

	int32_t prev_next = next(prev);
	set_next(object, prev_next);
	set_next(prev, object);

	return success;

}

int32_t CObjectQueue::pop()
{
	int32_t old_head = m_iHead;

	if (invalid_object_index == m_iHead)
	{
		return invalid_object_id;
	}

	if (success != erase(m_iHead))
	{
		old_head = invalid_object_id;
	}
	
	return old_head;

}

int32_t CObjectQueue::erase(int32_t object)
{
	CObjectIndex* pstTmpIndex = m_pstObjectPool->get_index(object);
	if (NULL == pstTmpIndex)
	{
		return fail;
	}

	//
	if (object == m_iHead)
	{
		m_iHead = next(m_iHead);
		set_next(object, invalid_object_index);//清除该对象索引用于形成该队列所用到的additional信息
		if (invalid_object_index == m_iHead) //若队列为空了
		{
			m_iTail = m_iHead;
		}

		return success;

	}

	//如果删除的不是头元素，由于该队列是单向队列，必须要从队列首部遍历，直到找到待删除对象object的前驱对象
	int32_t prev = invalid_object_index;
	for (prev = m_iHead; prev != invalid_object_index; prev = next(prev))
	{
		if (next(prev) == object)//是否为object的前驱对象
		{
			break;
		}
	}

	if (invalid_object_index == prev) //没有找到前驱对象
	{
		return fail;
	}

	//delete the object
	set_next(prev, next(object));
	set_next(object, invalid_object_index);//清除该对象索引用于形成该队列所用到的additional信息

	if (object == m_iTail)//若待删除的对象是队列的尾对象，则要更新尾对象指针
	{
		m_iTail = prev;
	}

	return success;

}

void CObjectQueue::dump(const char* file)
{
	if (NULL == file)
	{
		return;
	}

	FILE* fp = fopen(file, "a+");
	if (NULL == fp)
	{
		return;
	}

	int32_t iIdx = head();
	int32_t i = 0;
	while (iIdx != invalid_object_id)
	{
		if (!(i%16))
		{
			fprintf(fp, "\n%06d>>\t", i);
		}

		fprintf(fp, "%06d  ", iIdx);
		iIdx = next(iIdx);
		++i;
	}

	fclose(fp);
}

SERVER_END
