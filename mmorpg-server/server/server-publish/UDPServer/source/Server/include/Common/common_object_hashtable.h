#ifndef __COMMON_OBJECT_HASHTABLE_H__
#define __COMMON_OBJECT_HASHTABLE_H__

#include "Common/common_base.h"
#include "Common/common_object.h"
#include "common_namespace_definition.h"


/*
 * @defgroup common_object_hashtable
 * @author  aprilliu
 * @version 
 * @date    2008/02/22
 * @purpose ʵ��һ��HashTable���ײ���CObjectPool��ʵ�֡��ر�ע�⣺SIZE����Ϊ0��
 *			Object��key ������ַ������ͣ�keylengthΪstrlen()�ķ���ֵ��
 * @{
 */

SERVER_BEGIN

// bucket of hash table 
struct stBucket 
{
	int32_t m_iHead;     //��slot/bucket��ͻ������Ԫ��(��ʵ�Ƕ��������ֵ)
};

template<int32_t SIZE>
class CHashTable
{
public:
	enum
	{
		__SIZE__ = SIZE,
		MAX_KEY_LENGTH = max_object_key_length,
	};
public:
	CHashTable();
	CHashTable(CObjectPool* pool, int32_t next);
	virtual ~CHashTable();

	int32_t initialize(void);
	int32_t initialize(CObjectPool* pool, int32_t next);
	int32_t resume(CObjectPool* pool);

	/*
	* @method:    key_to_index ��key�õ���Ӧ�����Ͱ����
	* @fullname:  Game51::Server::CHashTable<SIZE>::key_to_index
	* @access:    virtual public 
	* @param: const void * key
	* @param: int32_t keylength
	* @return:   int32_t
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	virtual int32_t key_to_index(const void* key, int32_t keylength);

	/*
	* @method:    get_object_by_key 
	* @fullname:  Game51::Server::CHashTable<SIZE>::get_object_by_key
	* @access:    public 
	* @param: const void * key
	* @param: int32_t keylength
	* @return:   CObject*
	* - NULL ʧ��
	* - ����key��Ӧ��CObject
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	CObject* get_object_by_key(const void* key, int32_t keylength);

	/*
	* @method:    delete_object_by_key
	* @fullname:  Game51::Server::CHashTable<SIZE>::delete_object_by_key
	* @access:    public 
	* @param: const void * key
	* @param: int32_t keylength
	* @return:   int32_t ���ػ��յĶ��������ֵ/����ID
	* - invalid_object_index : ��ʾ����ʧ��
	* - ���ն��������ֵ/����ID
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t delete_object_by_key(const void* key, int32_t keylength);

	/*
	* @method:    create_object_by_key
	* @fullname:  Game51::Server::CHashTable<SIZE>::create_object_by_key
	* @access:    public 
	* @param: const void * key
	* @param: int32_t keylength
	* @return:   CObject*
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	CObject* create_object_by_key(const void* key, int32_t keylength);

	/*
	* @method:    operator[]
	* @fullname:  Game51::Server::CHashTable<SIZE>::operator[]
	* @access:    public 
	* @param: int32_t index
	* @return:   stBucket&
	* @qualifier:
	* @note �������Լ���֤�β�index����ȷ��
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	stBucket& operator [](int32_t index);

protected:
	
	/*
	* @method:    get_hashcode ��������
	* @fullname:  Game51::Server::CHashTable<SIZE>::get_hashcode
	* @access:    protected 
	* @param: void * key
	* @param: int32_t keylength
	* @return:   uint32_t ����key�Ĺ���ֵ
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	uint32_t get_hashcode(const void* key, int32_t keylength);


	/*
	* @method:    equal_key ���ıȽϺ���
	* @fullname:  Game51::Server::CHashTable<SIZE>::equal_key
	* @access:    protected 
	* @param: const void * srckey
	* @param: int32_t srckeylength
	* @param: const void * dstkey
	* @param: int32_t dstkeylength
	* @return:   bool
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	bool equal_key(const void* srckey,int32_t srckeylength, const void* dstkey, int32_t dstkeylength);
private:
	int32_t m_iHashNext;					/** �����γɳ�ͻ�����õ�Ԫ����CObjectIndex::m_iAdditional[]�е��±�*/
	CObjectPool* m_stObjectPool;		/** hashtable�ĵײ��������� */
	stBucket m_stBucket[__SIZE__];	/** Ͱ*/
};

template<int32_t SIZE>
CHashTable<SIZE>::CHashTable()
{
	//do nothing
}

template<int32_t SIZE>
CHashTable<SIZE>::CHashTable(CObjectPool *pool, int32_t next):m_stObjectPool(pool),m_iHashNext(next)
{
	if (object_createmode_init == CObject::m_iCreateMode)
	{
		initialize();
	}
}

template<int32_t SIZE>
CHashTable<SIZE>::~CHashTable()
{
	//do nothing
}

template<int32_t SIZE>
int32_t CHashTable<SIZE>::initialize()
{
	for (int32_t i = 0; i < __SIZE__; ++i)
	{
		m_stBucket[i].m_iHead = invalid_object_index;
	}

	return success;
}

template<int32_t SIZE>
int32_t CHashTable<SIZE>::initialize(CObjectPool *pool, int32_t next)
{
	if (next <= invalid_object_index)
	{
		return fail;
	}

	m_stObjectPool = pool;
	m_iHashNext = next;

	return initialize();
}

template<int32_t SIZE>
int32_t CHashTable<SIZE>::resume(CObjectPool *pool)
{

	m_stObjectPool = pool;
	return success;
}

template<int32_t SIZE>
stBucket& CHashTable<SIZE>::operator [](int32_t index)
{
	return m_stBucket[index];
}

template<int32_t SIZE>
CObject* CHashTable<SIZE>::create_object_by_key(const void *key, int32_t keylength)
{
	if (NULL == key || 0 >= keylength || NULL == m_stObjectPool)
	{
		return NULL;
	}

	char* tmp_key[MAX_KEY_LENGTH];
	int32_t tmp_keysize = 0;

	CObject* pstObject = NULL;
	CObjectIndex* pObjectIndex = NULL;

	int32_t tmp_index = invalid_object_index;
	int32_t bucket = key_to_index(key, keylength);
	if (m_stBucket[bucket].m_iHead >= 0)//�г�ͻ��
	{
		tmp_index = m_stBucket[bucket].m_iHead;
		while (tmp_index >= 0)
		{
			pObjectIndex = m_stObjectPool->get_index(tmp_index);
			if (NULL == pObjectIndex)
			{
				return NULL;
			}

			pstObject = m_stObjectPool->get_object(tmp_index);
			if (NULL == pstObject)
			{
				return NULL;
			}
			
			tmp_keysize = (int32_t)sizeof(tmp_key);
			pstObject->get_key(&tmp_key[0], tmp_keysize);
			if (equal_key(key, keylength, &tmp_key[0], tmp_keysize))
			{
				break;
			}

			//
			pObjectIndex->get_additional_data_of(m_iHashNext, tmp_index);
		}//while
		
		pstObject = m_stObjectPool->get_object(tmp_index);
		if (NULL != pstObject)
		{
			return pstObject;
		}
	}//if (m_stBucket[bucket].m_iHead >= 0)

	tmp_index = m_stObjectPool->create_object();
	if (invalid_object_index == tmp_index)
	{
		return NULL;
	}

	//�����ͻ��ͷ��
	pObjectIndex = m_stObjectPool->get_index(tmp_index);
	pObjectIndex->set_additional_data_of(m_iHashNext, m_stBucket[bucket].m_iHead);
	m_stBucket[bucket].m_iHead = tmp_index;
	pstObject = m_stObjectPool->get_object(tmp_index);
	pstObject->set_key(key, keylength);

	return pstObject;
}

template<int32_t SIZE>
CObject* CHashTable<SIZE>::get_object_by_key(const void *key, int32_t keylength)
{
	if (NULL == key || keylength <= 0 || NULL == m_stObjectPool)
	{
		return NULL;
	}

	int32_t index = key_to_index(key, keylength);
	if (m_stBucket[index].m_iHead == invalid_object_index)
	{
		return NULL;
	}

	//�ڳ�ͻ���в���
	char tmp_key[MAX_KEY_LENGTH];
	int32_t tmp_keysize = 0;

	int32_t tmp_index = m_stBucket[index].m_iHead;
	while (tmp_index >= 0)
	{
		CObjectIndex* tmp_objectindex = m_stObjectPool->get_index(tmp_index);
		if (NULL == tmp_objectindex)
		{
			return NULL;
		}

		CObject* tmp_object = m_stObjectPool->get_object(tmp_index);
		if (NULL == tmp_object)
		{
			return NULL;
		}
		
		tmp_keysize = (int32_t)sizeof(tmp_key);
		tmp_object->get_key(&tmp_key[0], tmp_keysize);
		if (equal_key(key, keylength, &tmp_key[0], tmp_keysize))//got it!
		{
			break;
		}

		//
		tmp_objectindex->get_additional_data_of(m_iHashNext, tmp_index);
		
	}//while

	return m_stObjectPool->get_object(tmp_index);

}

template<int32_t SIZE>
int32_t CHashTable<SIZE>::delete_object_by_key(const void *key, int32_t keylength)
{
	if (NULL == key || 0 >= keylength || NULL == m_stObjectPool)
	{
		return invalid_object_index;
	}

	int32_t bucket = key_to_index(key, keylength);
	if (m_stBucket[bucket].m_iHead == invalid_object_index)//��Ӧ��slot/bucket��û���κζ���
	{
		return invalid_object_index;
	}

	
	CObjectIndex* pstObjectIndex = NULL;
	CObject* pstObject = NULL;
	char szTmpKey[MAX_KEY_LENGTH];
	int32_t iKeyLength = 0;

	int32_t iTmpPrev = invalid_object_index;

	//�ڳ�ͻ���н��в���
	int32_t tmp_index = m_stBucket[bucket].m_iHead;
	while (tmp_index >= 0)
	{
		pstObjectIndex = m_stObjectPool->get_index(tmp_index);
		if (NULL == pstObjectIndex)
		{
			return invalid_object_index;
		}

		pstObject = m_stObjectPool->get_object(tmp_index);
		if (NULL == pstObject)
		{
			return invalid_object_index;
		}
	
		iKeyLength = (int32_t)sizeof(szTmpKey);
		pstObject->get_key(&szTmpKey[0], iKeyLength);
		if (equal_key(key, keylength, &szTmpKey[0], iKeyLength))//got it!
		{
			break;
		}

		//otherwise
		iTmpPrev = tmp_index;//��¼��ͻ����ĳ����ǰ�����
		pstObjectIndex->get_additional_data_of(m_iHashNext, tmp_index);
	}

	if (invalid_object_index == tmp_index)//do not get it
	{
		return invalid_object_index;
	}

	//ɾ���ҵ��Ķ���
	int32_t iTmpNext = invalid_object_index;
	CObjectIndex* pstTmpPrevIndex = NULL;

	pstObjectIndex->get_additional_data_of(m_iHashNext, iTmpNext);
	pstTmpPrevIndex = m_stObjectPool->get_index(iTmpPrev);
	if (NULL != pstTmpPrevIndex)//��ɾ�������ڳ�ͻ������ǰ������
	{
		pstTmpPrevIndex->set_additional_data_of(m_iHashNext, iTmpNext);
	}

	//�����ɾ������������м�¼�������γɳ�ͻ������Ϣ 
	pstObjectIndex->set_additional_data_of(m_iHashNext, invalid_object_index);

	if (tmp_index == m_stBucket[bucket].m_iHead)
	{
		m_stBucket[bucket].m_iHead = iTmpNext;
	}

	return m_stObjectPool->destroy_object(tmp_index);
	
}

template<int32_t SIZE>
int32_t CHashTable<SIZE>::key_to_index(const void *key, int32_t keylength)
{
	uint32_t hash_code = get_hashcode(key, keylength);
	return (int32_t)(hash_code%__SIZE__);
}



template<int32_t SIZE>
uint32_t CHashTable<SIZE>::get_hashcode(const void *key, int32_t keylength)
{

	uint32_t hash_code = 0;
	uint16_t *pTmp = (uint16_t*)key;

	for (int32_t i = 0; i < (int32_t)(keylength/sizeof(uint16_t)); ++i)
	{
		hash_code += pTmp[i];
	}

	char* pszTmp = NULL;
	uint16_t nTmp = 0;
	if (keylength%sizeof(uint16_t) > 0)
	{
		pszTmp = ((char*)key + (keylength - (keylength%sizeof(uint16_t))));
		memcpy((void*)&nTmp, (const void*)pszTmp, size_t(keylength%sizeof(uint16_t)));
	}

	hash_code += nTmp;

	hash_code = (hash_code & (uint16_t)0x7fff);


	return hash_code;
}

template<int32_t SIZE>
bool CHashTable<SIZE>::equal_key(const void *srckey, int32_t srckeylength, const void *dstkey, int32_t dstkeylength)
{
	return(srckeylength == dstkeylength && !memcmp(srckey, dstkey, srckeylength));
}

SERVER_END

/**//** @}*/ 

#endif /* __COMMON_OBJECT_HASHTABLE_H__ */
