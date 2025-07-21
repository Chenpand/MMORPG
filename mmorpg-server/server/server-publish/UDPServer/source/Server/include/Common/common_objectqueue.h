#ifndef __COMMON_OBJECTQUEUE_H__
#define __COMMON_OBJECTQUEUE_H__

#include "common_namespace_definition.h"
#include "gameplatform_types.h"

/*
 * @defgroup common_objectqueue
 * @author  aprilliu
 * @version 
 * @date    2008/02/24
 * @purpose 以CObjectPool为底层对象管理器，实现一个单向队列。主要提供查找，删除，插入等操作。
 *			可作为LogicServer的主控消息队列
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
	* @return:   Game51::int32_t 返回object对象在对象中的后置对象的索引/ID
	* @qualifier: const
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t next(int32_t object)const;

	/*
	* @method:    set_next 设置object对象的后置对象为next
	* @fullname:  Game51::Server::CObjectQueue::set_next
	* @access:    public 
	* @param: int32_t object
	* @param: int32_t next
	* @return:   Game51::int32_t
	* - success : 成功
	* - 其他: 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t set_next(int32_t object, int32_t next);

	/*
	* @method:    append 从尾部追加
	* @fullname:  Game51::Server::CObjectQueue::append
	* @access:    public 
	* @param: int32_t object_index
	* @return:   Game51::int32_t
	* - success : 成功
	* - 其他: 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t append(int32_t object);

	/*
	* @method:    push 从头部插入
	* @fullname:  Game51::Server::CObjectQueue::push
	* @access:    public 
	* @param: int32_t object
	* @return:   Game51::int32_t
	* - success : 成功
	* - 其他    : 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t push(int32_t object);

	/*
	* @method:    pop 删除头部对象
	* @fullname:  Game51::Server::CObjectQueue::pop
	* @access:    public 
	* @return:   Game51::int32_t 返回头部对象的索引/ID
	* invalid_object_id 表示无效的对象id
	* 
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t pop();

	/*
	* @method:    insert 在prev后面插入object
	* @fullname:  Game51::Server::CObjectQueue::insert
	* @access:    public 
	* @param: int32_t prev
	* @param: int32_t object
	* @return:   Game51::int32_t
	* - success : 成功
	* - 其他    : 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t insert(int32_t prev, int32_t object);

	/*
	* @method:    erase 删除指定对象object
	* @fullname:  Game51::Server::CObjectQueue::erase
	* @access:    public 
	* @param: int32_t object
	* @return:   Game51::int32_t
	* - success : 成功
	* - 其他    : 失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t erase(int32_t object);

	void dump(const char* file);


protected:
private:
	int32_t m_iHead;					//队列的头对象的Index(ID)
	int32_t m_iTail;					//队列的最后一个对象的Index(ID)

	int32_t m_iNextIndex;				//某对象的CObjectIndex::m_iAdditionData[m_iNextIndex]用于指向其在队列中的下一个元素
	CObjectPool* m_pstObjectPool;		//实际对象的管理器

};


SERVER_END


#endif //__COMMON_OBJECTQUEUE_H__

