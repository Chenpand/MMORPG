#ifndef __COMMON_OBJECT_H__
#define __COMMON_OBJECT_H__

/*
* @defgroup common_object
* @author  aprilliu
* @version 
* @date    2008/02/21
* @purpose ʵ��һ������ع����ࡣ������Ķ����Ǵ�һ��CObject�Ļ���̳����ģ��ڳ�ʼ�������ʱ�����������ڴ���
*			����ĺ���������һ����˵������ؾ���һ�����󹤳����ö���ؿ�����Ϊ����ADT�ĵײ�ʵ��.
*			����صĺ���˼����ǰѶ������Ͷ����������ֿ����������Զ�����и�����ʽ����֯����ʵ�ָ����ӵ����ݽṹ��
*			�������������ά��һ�����ж�������һ���ѷ����������������˫�������
* @{
*/

#include "gameplatform_types.h"
#include "Common/common_namespace_definition.h"
#include <new>

SERVER_BEGIN

enum 
{
	invalid_object_index = -1,			/**< ��Ч�Ķ�������ֵ */
	invalid_object_id = invalid_object_index, /**< ��Ч�Ķ���ID��Ҳ������Ч�Ķ������� */

	object_createmode_init = 0,			/**< ʵ�ʴ������� */
	object_createmode_resume = 1,		/**< �ָ�����*/

	max_object_key_length = 512,		/**< ����key����󳤶�*/
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



//����ء����ڹ���ָ�����ͣ�ָ�������Ķ���
class CObjectPool
{
public:
	enum
	{
		object_pool_formated = 1,      /**< ������Ѿ������˸�ʽ��*/
		object_pool_raw      = 0       /**< �����δ�����˸�ʽ��*/
	};
public:
	CObjectPool(int32_t objectcount, size_t objectsize, CreateObjectFunction create_method);
	~CObjectPool();

	

	int32_t initialize(void);
	
	/*
	* @method:    format_objects ��ʽ��������
	* @fullname:  Game51::Server::CObjectPool::format_objects
	* @access:    public 
	* @return:   int32_t
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t format_objects();

	/*
	* @method:    format_index ��ʽ������������
	* @fullname:  Game51::Server::CObjectPool::format_index
	* @access:    public 
	* @return:   int32_t
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t format_index();

	void set_create_object_method(CreateObjectFunction method);

	/*
	* @method:    create_object �ӿ��ж�������ͷ������һ�����󣬲������ѷ����������ͷ��
	* @fullname:  Game51::Server::CObjectPool::create_object
	* @access:    public 
	* @return:   int32_t ���䵽�Ķ��������ֵ��invalid_object_index/invalid_object_id��ʾ����ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	int32_t create_object();

	/*
	* @method:    destroy_object ����ָ���Ķ��󣨲�û�������ͻ����ڴ���Դ������������ж�����ͷ��
	* @fullname:  Game51::Server::CObjectPool::destroy_object
	* @access:    public 
	* @param: int32_t object_index ���յĶ��������/ID
	* @return:   int32_t ���ػ��յĶ����������invalid_object_index��ʾ����ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
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
	
	//�õ�����ص���������Ŀ
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
	* @method:    empty �ж϶�����Ƿ�Ϊ��(û�з����κζ���)
	* @fullname:  Game51::Server::CObjectPool::empty
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier: const
	* @note	
	* @par ʾ��:
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
	* @method:    full �ж϶�����������������������ж���
	* @fullname:  Game51::Server::CObjectPool::full
	* @access:    public 
	* @param: void
	* @return:   bool
	* @qualifier: const
	* @note	
	* @par ʾ��:
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
	* @method:    allocate_one_object ���������������ѷ���Ķ���������ӿ��ж��������ѷ�����е�ͷ��
	* @fullname:  Game51::Server::CObjectPool::allocate_one_object
	* @access:    protected 
	* @param: void
	* @return:   int32_t ���䵽�Ķ��������ֵ��invalid_object_index��ʾ����ʧ��
	* @qualifier:
	* @note	
	* @par ʾ��:
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
	* @method:    size ����������ռ�õ��ڴ��С
	* @fullname:  Game51::Server::CObjectPool::size
	* @access:    public static 
	* @param: void
	* @return:   int32_t ���ض������ռ�õ��ڴ��С����λ:Byte
	* @qualifier:
	* @note	
	* @par ʾ��:
	* @code
	
	* @endcode
	 
	* @see
	* @deprecated 
	*/
	static size_t size(int32_t objectcount, size_t objectsize);

	static void set_memory(CShmMemory* memory);
protected:

private:
	int16_t m_nIsFormat;          //�Ƿ��Ѿ��Զ������������������˸�ʽ����
	size_t m_uiObjectSize;       //�����С
	int32_t	   m_iObjectCount;      //��������������Ŀ
	int32_t    m_iFreeHead;         //δ�������������ʼ���������ֵ
	int32_t    m_iUsedHead;         //�ѷ������������ʼ���������ֵ
	int32_t    m_iUsedCount;        //�ѷ������ĸ���

	CObjectIndex* m_pstIndex;      //���������� 
	CObject*      m_pstObject;     //������
    CreateObjectFunction m_pfCreator; //���󴴽�����
};

class CObjectIndex
{
public:
	enum
	{
		max_additional_data_size = 8, /**< ��󸽼��ֶγ��� */

		object_index_used = 1,					  /**< ��ʹ�ñ�ʶ*/
		object_index_free = 0					  /**< δʹ�ñ�ʶ*/
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

	CObject* m_stObject;  //���index������CObject����
};



/** ����ʵ����������� */
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
	* @param: int32_t & keylength  ����void* key����󳤶ȣ�����void* key��ʵ�ʳ��ȡ���λ:Byte
	* @return:   int32_t
	* @qualifier: 
	* @note	
	* @par ʾ��:
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

	//����CObjectPool::format_objects()��Ҫֱ�Ӳ���m_iObjectID
	friend int32_t CObjectPool::format_objects();

protected:
private:
};

SERVER_END

/**//** @}*/ 

#endif
