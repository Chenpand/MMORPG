#ifndef __COMMON_OBJECT_H__
#define __COMMON_OBJECT_H__

/*
* @defgroup common_object
* @author  aprilliu
* @version 
* @date    2008/02/21
* @purpose 实现一个对象池管理类。所管理的对象都是从一个CObject的基类继承来的，在初始化对象池时可以设置用于创建
*			对象的函数，从这一点来说，对象池就是一个对象工厂。该对象池可以作为其他ADT的底层实现.
*			对象池的核心思想就是把对象区和对象索引区分开，由索引对对象进行各种形式的组织，以实现更复杂的数据结构。
*			对象池利用索引维护一个空闲对象链和一个已分配对象链，并且是双向的链。
* @{
*/

#include "gameplatform_types.h"
#include "Common/common_namespace_definition.h"
#include <new>

SERVER_BEGIN

enum 
{
	invalid_object_index = -1,			/**< 无效的对象索引值 */
	invalid_object_id = invalid_object_index, /**< 无效的对象ID，也就是无效的对象索引 */

	object_createmode_init = 0,			/**< 实际创建对象 */
	object_createmode_resume = 1,		/**< 恢复对象*/

	max_object_key_length = 512,		/**< 对象key的最大长度*/
};


//
#define DECLARE_DYN_CREATE \
public: \
	static CObject* create_one_object(void* pszMem);

//
#define DECLARE_ALLOCATED_CREATE(class_name)\
public:\
	static int32_t set_object_pool(size_t iObjectSize, int32_t iObjectCount);	\
	static CObjectPool* get_object_pool( ){return class_name::class_name##m_pstObjectPool;};\
	static void* operator new(size_t size ) throw();\
	static void  operator delete( void* pszMem) ;\
	static class_name* get_object_by_id(int32_t id){return (class_name *)class_name##m_pstObjectPool->get_object(id);}\
	static int32_t get_next_object_id(int32_t id);\
	static int32_t get_next_object_id(CObject* pstObject);\
	virtual int32_t get_object_id(){return class_name##m_pstObjectPool->get_object_id(this);}\
	static CObject* create_object_on_allocated_memory(void* pszMem);\
	static int32_t get_used_count(){return class_name##m_pstObjectPool->get_used_count();}\
	static int32_t get_free_count(){return class_name##m_pstObjectPool->get_free_count();}\
	static int32_t get_used_head(){return class_name##m_pstObjectPool->get_used_head();}\
	static int32_t get_free_head(){return class_name##m_pstObjectPool->get_free_head();}\
	static CObjectPool * class_name##m_pstObjectPool;\


#define IMPLEMENT_DYN_CREATE(class_name) \
	CObject* class_name::create_one_object(void* pszMem) \
{ \
	CObject::pszMemory = (char *)pszMem; \
	return (CObject *)(new class_name); \
	}


#define IMPLEMENT_ALLOCATED_CREATE(class_name) \
	CObjectPool* class_name::class_name##m_pstObjectPool = NULL;\
	void* class_name::operator new( size_t size ) throw()\
{\
	if(NULL == class_name##m_pstObjectPool)\
{\
	return NULL;\
}\
	return  class_name##m_pstObjectPool->allocate_memory_for_object();\
}\
	void  class_name::operator delete(void* pszMem)\
{\
	int32_t idx = -1;\
	idx = class_name##m_pstObjectPool->get_object_id((CObject *)pszMem);\
	if(idx < 0 )\
{\
	return;\
}\
	class_name##m_pstObjectPool->destroy_object(idx);\
	return;\
}\
	int32_t  class_name::set_object_pool(size_t iObjectSize, int32_t iObjectCount)\
{\
	class_name##m_pstObjectPool = new CObjectPool(iObjectCount, iObjectSize, NULL);\
	class_name##m_pstObjectPool->set_create_object_method(class_name::create_object_on_allocated_memory); \
	class_name##m_pstObjectPool->format_objects();\
	return 0;\
}\
	CObject* class_name::create_object_on_allocated_memory(void* pszMem) \
{ \
	if (NULL == pszMem)\
{\
	return NULL;\
}\
	return (CObject *)(::new(pszMem) class_name);\
}\
	int32_t class_name::get_next_object_id(int32_t id) \
{ \
	if (0 > id)\
{\
	return invalid_object_id;\
}\
	CObjectIndex *pstObjectIdx = class_name##m_pstObjectPool->get_index(id);\
	if (NULL == pstObjectIdx)\
{\
	return invalid_object_id;\
}\
	return pstObjectIdx->get_next(); \
}\
	int32_t class_name::get_next_object_id(CObject* pstObject) \
{ \
	int32_t id = pstObject->get_object_id();\
	if (0 > id)\
{\
	return invalid_object_id;\
}\
	return get_next_object_id(id); \
}\


class CShmMemory;



class CObject;
class CObjectIndex;
class CObjectPool;

typedef CObject*(*CreateObjectFunction)(void*);



//对象池。用于管理指定类型，指定数量的对象
class CObjectPool
{
public:
	enum
	{
		object_pool_formated = 1,      /**< 对象池已经进行了格式化*/
		object_pool_raw      = 0       /**< 对象池未进行了格式化*/
	};
public:
	CObjectPool(int32_t objectcount, size_t objectsize, CreateObjectFunction create_method);
	~CObjectPool();

	

	int32_t initialize(void);
	
	/*
	* @method:    format_objects 格式化对象区
	* @fullname:  Game51::Server::CObjectPool::format_objects
	* @access:    public 
	* @return:   int32_t
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t format_objects();

	/*
	* @method:    format_index 格式化对象索引区
	* @fullname:  Game51::Server::CObjectPool::format_index
	* @access:    public 
	* @return:   int32_t
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t format_index();

	void set_create_object_method(CreateObjectFunction method);

	/*
	* @method:    create_object 从空闲对象链的头部分配一个对象，并放入已分配对象链的头部
	* @fullname:  Game51::Server::CObjectPool::create_object
	* @access:    public 
	* @return:   int32_t 分配到的对象的索引值。invalid_object_index/invalid_object_id表示分配失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t create_object();

	/*
	* @method:    destroy_object 回收指定的对象（并没有析构和回收内存资源），并放入空闲对象链头部
	* @fullname:  Game51::Server::CObjectPool::destroy_object
	* @access:    public 
	* @param: int32_t object_index 回收的对象的索引/ID
	* @return:   int32_t 返回回收的对象的索引。invalid_object_index表示回收失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t destroy_object(int32_t object_index);

	CObject* get_object(int32_t object_index);
	CObjectIndex* get_index(int32_t object_index);

	void* operator new(size_t size)throw();
	void operator delete(void* pobject);

	int32_t get_object_id(CObject* object);

	char* allocate_memory_for_object();
	
	//得到对象池的最大对象数目
	inline int32_t get_pool_size(void)const
	{
		return m_iObjectCount;
	}

	inline int32_t get_used_count(void)const
	{
		return m_iUsedCount;
	}

	inline int32_t get_free_count(void)const
	{
		return (m_iObjectCount - m_iUsedCount);
	}

	inline int32_t get_free_head(void)const
	{
		return m_iFreeHead;
	}

	inline int32_t get_used_head(void)const
	{
		return m_iUsedHead;
	}


	/*
	* @method:    empty 判断对象池是否为空(没有分配任何对象)
	* @fullname:  Game51::Server::CObjectPool::empty
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier: const
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	inline bool empty(void)const
	{
		return (0 == m_iUsedCount);
	}


	/*
	* @method:    full 判断对象池是已满（分配完了所有对象）
	* @fullname:  Game51::Server::CObjectPool::full
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier: const
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	inline bool full(void)const
	{
		return((m_iUsedCount == m_iObjectCount) != 0);
	}
	

protected:

	/*
	* @method:    allocate_one_object 操作对象索引，把分配的对象的索引从空闲队列移入已分配队列的头部
	* @fullname:  Game51::Server::CObjectPool::allocate_one_object
	* @access:    protected 
	* @param: void
	* @return:   int32_t 分配到的对象的索引值。invalid_object_index表示分配失败
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t allocate_one_object(void);

	bool is_valid(int32_t objectindex);
public:
    static CShmMemory* m_pstMemory;


	/*
	* @method:    size 计算对象池所占用的内存大小
	* @fullname:  Game51::Server::CObjectPool::size
	* @access:    public static 
	* @param: void
	* @return:   int32_t 返回对象池所占用的内存大小，单位:Byte
	* @qualifier:
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	static size_t size(int32_t objectcount, size_t objectsize);

	static void set_memory(CShmMemory* memory);
protected:

private:
	int16_t m_nIsFormat;          //是否已经对对象区和索引区进行了格式化。
	size_t m_uiObjectSize;       //对象大小
	int32_t	   m_iObjectCount;      //对象区最大对象数目
	int32_t    m_iFreeHead;         //未分配对象链的起始对象的索引值
	int32_t    m_iUsedHead;         //已分配对象链的起始对象的索引值
	int32_t    m_iUsedCount;        //已分配对象的个数

	CObjectIndex* m_pstIndex;      //对象索引区 
	CObject*      m_pstObject;     //对象区
    CreateObjectFunction m_pfCreator; //对象创建方法
};

class CObjectIndex
{
public:
	enum
	{
		max_additional_data_size = 8, /**< 最大附加字段长度 */

		object_index_used = 1,					  /**< 已使用标识*/
		object_index_free = 0					  /**< 未使用标识*/
	};


public:
	CObjectIndex();
	~CObjectIndex();

	int32_t initialize();

	void set_used();
	void set_free();

	inline int32_t is_used()const
	{
		return m_iFlag;
	}

	void set_next(int32_t next);
	void set_prev(int32_t prev);

	int32_t get_next(void);
	int32_t get_prev(void);

	int32_t get_additional_data_of(int32_t index, int32_t& value);
	int32_t set_additional_data_of(int32_t index, int32_t value);
	void clear_additional_data(void);

	inline CObject* get_object(void)const
	{
		return m_stObject;
	}

	int32_t set_object(CObject* object);


protected:
	
private:
	int32_t m_iFlag;
	int32_t m_iNext;
	int32_t m_iPrev;
	int32_t m_iAdditionData[max_additional_data_size];

	CObject* m_stObject;  //与该index关联的CObject对象
};



/** 所有实体对象的虚基类 */
class CObject
{
public:
	virtual ~CObject()
	{

	}

	virtual int32_t initialize(void) = 0;

	virtual int32_t resume(void) = 0;

	/*
	* @method:    get_key
	* @fullname:  Game51::Server::CObject::get_key
	* @access:    virtual public 
	* @param: void * key
	* @param: int32_t & keylength  传入void* key的最大长度，传出void* key的实际长度。单位:Byte
	* @return:   int32_t
	* @qualifier: 
	* @note	
	* @par 示例:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	virtual int32_t get_key(void* key, int32_t& keylength) = 0;

	virtual int32_t set_key(const void*key, int32_t keylength) = 0;

	virtual int32_t dump(int32_t handle) = 0;

	virtual int32_t get_object_id(void)
	{
		return m_iObjectID;
	}
protected:
	static void* operator new(size_t size)throw();
	static void	 operator delete(void* object);
	static char* pszMemory;
public:
	static int32_t  m_iCreateMode;

private:
	int32_t m_iObjectID;

	//由于CObjectPool::format_objects()需要直接操作m_iObjectID
	friend int32_t CObjectPool::format_objects();

protected:
private:
};

SERVER_END

/**//** @}*/ 

#endif
