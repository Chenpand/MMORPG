#ifndef _AVALON_CODE_QUEUE_H_
#define _AVALON_CODE_QUEUE_H_

#include <AvalonDefine.h>

//#ifndef AVALON_OS_WINDOWS
namespace Avalon
{
	class CShmMemory;

	/*
		字节流形式的循环队列，每个消息code保存到队列中时，队列会预留4个Byte保存消息code的长度（不包括这4个Byte本身）
	*/
	class CCodeQueue
	{
	public:

		enum
		{
			invalid_offset = -1, /**< 无效的偏移量 */
			reserved_length = 8, /**< 预留的空间长度*/

			//fail = -1,
			//success = 0,
		};

		/** 队列所用的共享内存区 */
		static CShmMemory* m_pstSharedMemory;

		/** 设置队列所在的共享内存区 */
		static void set_sharedmemory(CShmMemory* pMem);

		/** 得到队列所占用的内存大小*/
		static size_t size(Int32 buffersize);

	protected://attributes
		//数据区总长度，单位: Byte。
		Int32 m_iSize;
		//可用数据起始位置
		Int32 m_iHead;
		//可用数据结束位置
		Int32 m_iTail;
		//数据区的偏移位置
		Int32 m_iCodeOffset;

	public:
		CCodeQueue(Int32 size);
		~CCodeQueue();

		Int32 initialize(Int32 size);

		Int32 resume(Int32 size);

		void* operator new(size_t size) throw();

		void  operator delete(void *pbuffer);

		/*
		* @method:    append 从尾部追加消息buffer。该函数只是改变m_iTail，即使是在多线程环境下也不需要加/解锁操作，(1v1的线程模式)
		*				因为，对于四字节的int32_t的读写操作是原子操作
		* @fullname:  CCodeQueue::append
		* @access:    public
		* @param: const char * code
		* @param: int32_t size
		* @return:   int32_t。
		* - 0: 成功
		* - < 0 : 失败
		* @qualifier:
		* @note
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated
		*/
		Int32 append(const char* code, Int32 size);


		/*
		* @method:    pop 从队列头部取一条消息
		* @fullname:  CCodeQueue::pop
		* @access:    public
		* @param[in]: char * dst
		* @param[in,out]: short & outlength。传入dst的长度，传出实际消息code的长度
		* @return:   int32_t
		* - 0: 成功. outlength返回实际code的长度
		* - <0 : 失败， dst,outlength的值无意义
		* @qualifier:
		* @note
		* @par 示例:
		* @code

		* @endcode

		* @see
		* @deprecated
		*/
		Int32 pop(char* dst, Int32& outlength);

		/*
		* @method:    full 判断队列是否满
		* @fullname:  CCodeQueue::full
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
		* @fullname:  CCodeQueue::empty
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
		Int32 code_offset(void)const;

		Int32 set_boundary(Int32 head, Int32 tail);

		Int32 get_boundary(Int32& head, Int32& tail)const;

		char* get_codebuffer(void)const;

	public:
		/*
		* @method:    get_freesize
		* @fullname:  :CCodeQueue::get_freesize
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
		Int32 get_freesize(void)const;

		Int32 get_codesize(void)const;




	private:
		//forbidden copy constructor
		CCodeQueue(const CCodeQueue& init);
		CCodeQueue& operator =(const CCodeQueue&);
	};



}//end of namespace Avalon

#endif

//#endif
