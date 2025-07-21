#ifndef __COMMON_CODEQUEUE_H__
#define __COMMON_CODEQUEUE_H__

/*
* @defgroup common_codequeue
* @author  aprilliu
* @version 
* @date    2008/02/20
* @purpose 字节流形式的队列，每个消息code保存到队列中时，队列会预留4个Byte保存消息code的长度（不包括这4个Byte本身）
* @{
*/

#include "Common/common_base.h"
#include "common_namespace_definition.h"


SERVER_BEGIN

	class CShmMemory;

	class CCodeQueue
	{
	public:

		enum
		{
			invalid_offset = -1, /**< 无效的偏移量 */
			reserved_length = 8, /**< 预留的空间长度*/
		};

		/** 队列所用的共享内存区 */
		static CShmMemory* m_pstSharedMemory;

		/** 设置队列所在的共享内存区 */
		static void set_sharedmemory(CShmMemory* pMem);

		/** 得到队列所占用的内存大小*/
		static size_t size(int32_t buffersize);

	protected://attributes
		//数据区总长度，单位: Byte。
		int32_t m_iSize;
		//可用数据起始位置
		int32_t m_iHead;
		//可用数据结束位置
		int32_t m_iTail;
		//数据区的偏移位置
		int32_t m_iCodeOffset;

	public:
		CCodeQueue(int32_t size);
		~CCodeQueue();

		int32_t initialize(int32_t size);

		int32_t resume(int32_t size);

		void* operator new( size_t size ) throw();

		void  operator delete( void *pbuffer );

		/*
		* @method:    append 从尾部追加消息buffer。该函数只是改变m_iTail，即使是在多线程环境下也不需要加/解锁操作，
		*				因为，对于四字节的int32_t的读写操作是原子操作
		* @fullname:  Game51::Server::CCodeQueue::append
		* @access:    public 
		* @param: const char * code
		* @param: int32_t size
		* @return:   int32_t。 
		* - 0: 成功
		* - >0 : 失败
		* @qualifier: 
		* @note	
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated 
		*/
		int32_t append(const char* code, int32_t size);


		/*
		* @method:    pop 从队列头部取一条消息
		* @fullname:  Game51::Server::CCodeQueue::pop
		* @access:    public 
		* @param[in]: char * dst
		* @param[in,out]: short & outlength。传入dst的长度，传出实际消息code的长度
		* @return:   int32_t
		* - 0: 成功. outlength返回实际code的长度
		* - >0 : 失败， dst,outlength的值无意义
		* @qualifier:
		* @note	
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated 
		*/
		int32_t pop(char* dst, int32_t& outlength);



		/*
		* @method:    pop_from
		* @fullname:  Game51::Server::CCodeQueue::pop_from
		* @access:    public 
		* @param[in]: int32_t offset 指定从offset处取code
		* @param[in]: int32_t codesize 指定获取的code长度
		* @param[in]: char * dst
		* @param[in,out]: int32_t & outlenght
		* @return:   int32_t
		* @qualifier:
		* @note	
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated  目前暂时不实现该方法，因为底层为连续存储的队列中取数据会造成数据移动，效率较低。
		*/
		int32_t pop_from(int32_t offset, int32_t codesize, char* dst, int32_t& outlenght);



		/*
		* @method:    full 判断队列是否满
		* @fullname:  Game51::Server::CCodeQueue::full
		* @access:    public 
		* @param: void
		* @return:   bool
		* @qualifier:
		* @note	
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated 
		*/
		bool full(void);


		/*
		* @method:    empty 判读队列是否为空
		* @fullname:  Game51::Server::CCodeQueue::empty
		* @access:    public 
		* @param: void
		* @return:   bool
		* @qualifier:
		* @note	
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated 
		*/
		bool empty(void);

	protected:
		/** 返回数据区偏移 */
		int32_t code_offset(void)const;

		int32_t set_boundary(int32_t head, int32_t tail);

		int32_t get_boundary(int32_t& head, int32_t& tail)const;

		char* get_codebuffer(void)const;

	public:
		/*
		* @method:    get_freesize
		* @fullname:  Game51::Server::CCodeQueue::get_freesize
		* @access:    protected 
		* @param: void
		* @return:   int32_t
		* - 返回可用空间的大小。如果没有可用空间，返回值为0。也就是说该函数的
		*	返回值总是>=0的。
		* @qualifier: const
		* @note	保证队列buffer的长度不能为0,否则结果为定义.
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated 
		*/
		int32_t get_freesize(void)const;

		int32_t get_codesize(void)const;




	private:
		//forbidden copy constructor
		CCodeQueue(const CCodeQueue& init);
		CCodeQueue& operator =(const CCodeQueue&);
	};

SERVER_END

#endif

