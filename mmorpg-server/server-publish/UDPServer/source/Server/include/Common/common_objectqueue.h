#ifndef __COMMON_OBJECTQUEUE_H__
#define __COMMON_OBJECTQUEUE_H__

#include "common_namespace_definition.h"
#include "gameplatform_types.h"

/*
 * @defgroup common_objectqueue
 * @author  aprilliu
 * @version 
 * @date    2008/02/24
 * @purpose ��CObjectPoolΪ�ײ�����������ʵ��һ��������С���Ҫ�ṩ���ң�ɾ��������Ȳ�����
 *			����ΪLogicServer��������Ϣ����
 * @					
 * @{
 */
SERVER_BEGIN


class CObjectPool;

class CObjectQueue
{
public:
	CObjectQueue();
	CObjectQueue(CObjectPool* pool, int32_t next_index);
	~CObjectQueue();

	int32_t initialize(CObjectPool* pool, int32_t next_index);
	
	int32_t resume(CObjectPool* pool);


	inline int32_t head()
	{
		return m_iHead;
	}

	inline const int32_t head()const
	{
		return m_iHead;
	}

	inline int32_t tail()
	{
		return m_iTail;
	}

	inline const int32_t tail()const
	{
		return m_iTail;
	}

	/*
	* @method:    next
	* @fullname:  Game51::Server::CObjectQueue::next
	* @access:    public 
	* @param: int32_t object
	* @return:   Game51::int32_t ����object�����ڶ����еĺ��ö��������/ID
	* @qualifier: const
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t next(int32_t object)const;

	/*
	* @method:    set_next ����object����ĺ��ö���Ϊnext
	* @fullname:  Game51::Server::CObjectQueue::set_next
	* @access:    public 
	* @param: int32_t object
	* @param: int32_t next
	* @return:   Game51::int32_t
	* - success : �ɹ�
	* - ����: ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t set_next(int32_t object, int32_t next);

	/*
	* @method:    append ��β��׷��
	* @fullname:  Game51::Server::CObjectQueue::append
	* @access:    public 
	* @param: int32_t object_index
	* @return:   Game51::int32_t
	* - success : �ɹ�
	* - ����: ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t append(int32_t object);

	/*
	* @method:    push ��ͷ������
	* @fullname:  Game51::Server::CObjectQueue::push
	* @access:    public 
	* @param: int32_t object
	* @return:   Game51::int32_t
	* - success : �ɹ�
	* - ����    : ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t push(int32_t object);

	/*
	* @method:    pop ɾ��ͷ������
	* @fullname:  Game51::Server::CObjectQueue::pop
	* @access:    public 
	* @return:   Game51::int32_t ����ͷ�����������/ID
	* invalid_object_id ��ʾ��Ч�Ķ���id
	* 
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t pop();

	/*
	* @method:    insert ��prev�������object
	* @fullname:  Game51::Server::CObjectQueue::insert
	* @access:    public 
	* @param: int32_t prev
	* @param: int32_t object
	* @return:   Game51::int32_t
	* - success : �ɹ�
	* - ����    : ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t insert(int32_t prev, int32_t object);

	/*
	* @method:    erase ɾ��ָ������object
	* @fullname:  Game51::Server::CObjectQueue::erase
	* @access:    public 
	* @param: int32_t object
	* @return:   Game51::int32_t
	* - success : �ɹ�
	* - ����    : ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t erase(int32_t object);

	void dump(const char* file);


protected:
private:
	int32_t m_iHead;					//���е�ͷ�����Index(ID)
	int32_t m_iTail;					//���е����һ�������Index(ID)

	int32_t m_iNextIndex;				//ĳ�����CObjectIndex::m_iAdditionData[m_iNextIndex]����ָ�����ڶ����е���һ��Ԫ��
	CObjectPool* m_pstObjectPool;		//ʵ�ʶ���Ĺ�����

};


SERVER_END


#endif //__COMMON_OBJECTQUEUE_H__

