#ifndef _AVALON_CODE_QUEUE_H_
#define _AVALON_CODE_QUEUE_H_

#include <AvalonDefine.h>

//#ifndef AVALON_OS_WINDOWS
namespace Avalon
{
	class CShmMemory;

	/*
		�ֽ�����ʽ��ѭ�����У�ÿ����Ϣcode���浽������ʱ�����л�Ԥ��4��Byte������Ϣcode�ĳ��ȣ���������4��Byte����
	*/
	class CCodeQueue
	{
	public:

		enum
		{
			invalid_offset = -1, /**< ��Ч��ƫ���� */
			reserved_length = 8, /**< Ԥ���Ŀռ䳤��*/

			//fail = -1,
			//success = 0,
		};

		/** �������õĹ����ڴ��� */
		static CShmMemory* m_pstSharedMemory;

		/** ���ö������ڵĹ����ڴ��� */
		static void set_sharedmemory(CShmMemory* pMem);

		/** �õ�������ռ�õ��ڴ��С*/
		static size_t size(Int32 buffersize);

	protected://attributes
		//�������ܳ��ȣ���λ: Byte��
		Int32 m_iSize;
		//����������ʼλ��
		Int32 m_iHead;
		//�������ݽ���λ��
		Int32 m_iTail;
		//��������ƫ��λ��
		Int32 m_iCodeOffset;

	public:
		CCodeQueue(Int32 size);
		~CCodeQueue();

		Int32 initialize(Int32 size);

		Int32 resume(Int32 size);

		void* operator new(size_t size) throw();

		void  operator delete(void *pbuffer);

		/*
		* @method:    append ��β��׷����Ϣbuffer���ú���ֻ�Ǹı�m_iTail����ʹ���ڶ��̻߳�����Ҳ����Ҫ��/����������(1v1���߳�ģʽ)
		*				��Ϊ���������ֽڵ�int32_t�Ķ�д������ԭ�Ӳ���
		* @fullname:  CCodeQueue::append
		* @access:    public
		* @param: const char * code
		* @param: int32_t size
		* @return:   int32_t��
		* - 0: �ɹ�
		* - < 0 : ʧ��
		* @qualifier:
		* @note
		* @par ʾ��:
		* @code

		* @endcode

		* @see
		* @deprecated
		*/
		Int32 append(const char* code, Int32 size);


		/*
		* @method:    pop �Ӷ���ͷ��ȡһ����Ϣ
		* @fullname:  CCodeQueue::pop
		* @access:    public
		* @param[in]: char * dst
		* @param[in,out]: short & outlength������dst�ĳ��ȣ�����ʵ����Ϣcode�ĳ���
		* @return:   int32_t
		* - 0: �ɹ�. outlength����ʵ��code�ĳ���
		* - <0 : ʧ�ܣ� dst,outlength��ֵ������
		* @qualifier:
		* @note
		* @par ʾ��:
		* @code

		* @endcode

		* @see
		* @deprecated
		*/
		Int32 pop(char* dst, Int32& outlength);

		/*
		* @method:    full �ж϶����Ƿ���
		* @fullname:  CCodeQueue::full
		* @access:    public
		* @param: void
		* @return:   bool
		* @qualifier:
		* @note
		* @par ʾ��:
		* @code

		* @endcode

		* @see
		* @deprecated
		*/
		bool full(void);


		/*
		* @method:    empty �ж������Ƿ�Ϊ��
		* @fullname:  CCodeQueue::empty
		* @access:    public
		* @param: void
		* @return:   bool
		* @qualifier:
		* @note
		* @par ʾ��:
		* @code

		* @endcode

		* @see
		* @deprecated
		*/
		bool empty(void);

	protected:
		/** ����������ƫ�� */
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
		* - ���ؿ��ÿռ�Ĵ�С�����û�п��ÿռ䣬����ֵΪ0��Ҳ����˵�ú�����
		*	����ֵ����>=0�ġ�
		* @qualifier: const
		* @note	��֤����buffer�ĳ��Ȳ���Ϊ0,������Ϊ����.
		* @par ʾ��:
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
