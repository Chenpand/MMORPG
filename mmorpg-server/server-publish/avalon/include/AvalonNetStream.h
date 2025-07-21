/**
 *@author	 
 *@email	 
 *@date		2011-03-12	
 *@brief	������
 *@modified TODO: by aprilliu , �����ֽ���ת���Ĺ���
 */
#ifndef _AVALON_NETSTREAM_H_
#define _AVALON_NETSTREAM_H_

#include <string>
#include <vector>
#include <map>
#include "AvalonDefine.h"
#include "AvalonNetExceptions.h"

namespace Avalon
{
	class NetInputStream;
	class NetOutputStream;

	/**
	 *@brief �����л��ӿ�
	 */
	class INetSerializable
	{
	public:
		virtual ~INetSerializable(){}

		virtual void Input(NetInputStream& stream) = 0;

		virtual void Output(NetOutputStream& stream) = 0;
	};

	/**
	 *@ʹ������궨�����л�
	 */
#define AVALON_DEFINE_NETSERIALIZATION() \
	void Input(Avalon::NetInputStream& stream) { \
		Serialize(stream); \
	} \
	void Output(Avalon::NetOutputStream& stream) { \
		Serialize(stream); \
	} \
	template<typename StreamT> \
	void Serialize(StreamT& stream)

	/**
	 *@brief ������   noted by aprilliu  ���յ���stream �����message�У�Decode����)
	 */
	class NetInputStream
	{
	public:
		NetInputStream(UInt8* buffer,size_t size)
		{
			m_Buffer = buffer;
			m_Size = size;
			m_Index = 0;
		}
		~NetInputStream()
		{
		}

	public:

#define AVALON_NET_BASETYPE_INPUT(Type)\
		NetInputStream& operator&(Type& value){ \
			if(m_Index + sizeof(Type) > m_Size) \
				throw NetSerializeException("NetInputStream:out off buffer!"); \
			value = *(Type*)(m_Buffer + m_Index); \
			m_Index += sizeof(Type); \
			return *this; \
		}

		//noted by aprilliu, ����Real32, Real64����ʵ��float,double�����������ֽ���ת�� һ���ǲ���ת�ģ����Э�������и����� ����Լ��ת�����������ٽ����ֽ����ת��
		AVALON_NET_BASETYPE_INPUT(Int8)
		AVALON_NET_BASETYPE_INPUT(UInt8)
		AVALON_NET_BASETYPE_INPUT(Int16)
		AVALON_NET_BASETYPE_INPUT(UInt16)
		AVALON_NET_BASETYPE_INPUT(Int32)
		AVALON_NET_BASETYPE_INPUT(UInt32)
		AVALON_NET_BASETYPE_INPUT(Int64)
		AVALON_NET_BASETYPE_INPUT(UInt64)
		AVALON_NET_BASETYPE_INPUT(Real32)
		AVALON_NET_BASETYPE_INPUT(Real64)
		////��ʵ����Ϊmemory�����б���
#define AVALON_NET_BASETYPE_ARRAY_INPUT(Type) \
		NetInputStream& SeriaArray(Type* ptr,size_t num){ \
			size_t size = sizeof(Type) * num; \
			if(m_Index + size > m_Size) \
				throw NetSerializeException("NetInputStream::out off buffer!"); \
			memmove(ptr,m_Buffer + m_Index,size);\
			m_Index += size; \
			return *this; \
		}

		AVALON_NET_BASETYPE_ARRAY_INPUT(Int8)
		AVALON_NET_BASETYPE_ARRAY_INPUT(UInt8)
		AVALON_NET_BASETYPE_ARRAY_INPUT(Int16)
		AVALON_NET_BASETYPE_ARRAY_INPUT(UInt16)
		AVALON_NET_BASETYPE_ARRAY_INPUT(Int32)
		AVALON_NET_BASETYPE_ARRAY_INPUT(UInt32)
		AVALON_NET_BASETYPE_ARRAY_INPUT(Int64)
		AVALON_NET_BASETYPE_ARRAY_INPUT(UInt64)
		AVALON_NET_BASETYPE_ARRAY_INPUT(Real32)
		AVALON_NET_BASETYPE_ARRAY_INPUT(Real64)

		NetInputStream& operator&(std::string& str)
		{
			UInt16 length = 0;
			operator&(length);
			if(m_Index + length > m_Size)
				throw NetSerializeException("NetInputStream::out off buffer!");
			str.assign((char*)(m_Buffer + m_Index),length);
			m_Index += length;
			return *this;
		}

		NetInputStream& SeriaArray(std::string* strs,size_t num)
		{
			for(size_t i = 0;i < num;++i)
			{
				operator&(strs[i]);
			}
			return *this;
		}

		template<typename T>
		NetInputStream& operator&(T& value)
		{
			value.Input(*this);
			return *this;
		}

		template<typename T>
		NetInputStream& SeriaArray(T* ptr,size_t num)
		{
			for(size_t i = 0;i < num;++i)
			{
				this->operator&(ptr[i]);
			}
			return *this;
		}

		template<typename T>
		NetInputStream& operator&(std::vector<T>& vec)
		{
			UInt16 length = 0;
			operator&(length);
			vec.resize(length);
			for(size_t i = 0; i < vec.size();++i)
			{
				operator&(vec[i]);
			}
			return *this;
		}

		template<typename KeyT,typename ValueT>
		NetInputStream& operator&(std::map<KeyT,ValueT>& list)
		{
			UInt16 length = 0;
			operator&(length);
			for(size_t i = 0; i < length;++i)
			{
				KeyT key;
				operator&(key);
				operator&(list[key]);
			}
			return *this;
		}

		size_t GetPos() const
		{
			return m_Index;
		}

	private:
		//�ֽڻ���
		UInt8* m_Buffer;
		//�����С
		size_t m_Size;
		//��ǰ����
		size_t m_Index;
	};

	/**
	 *@brief �����  noted by aprilliu  ��message�����stream (Encode����)
	 */
	class NetOutputStream
	{
	public:
		NetOutputStream(UInt8* buffer,size_t size)
		{
			m_Buffer = buffer;
			m_Size = size;
			m_Index = 0;
		}
		~NetOutputStream()
		{
		}

	public:

#define AVALON_NET_BASETYPE_OUTPUT(Type)\
		NetOutputStream& operator&(Type value){ \
			if(m_Index + sizeof(Type) > m_Size) \
				throw NetSerializeException("NetOutputStream:out off buffer!"); \
			*(Type*)(m_Buffer + m_Index) = value; \
			m_Index += sizeof(Type); \
			return *this; \
		}

		AVALON_NET_BASETYPE_OUTPUT(Int8)
		AVALON_NET_BASETYPE_OUTPUT(UInt8)
		AVALON_NET_BASETYPE_OUTPUT(Int16)
		AVALON_NET_BASETYPE_OUTPUT(UInt16)
		AVALON_NET_BASETYPE_OUTPUT(Int32)
		AVALON_NET_BASETYPE_OUTPUT(UInt32)
		AVALON_NET_BASETYPE_OUTPUT(Int64)
		AVALON_NET_BASETYPE_OUTPUT(UInt64)
		AVALON_NET_BASETYPE_OUTPUT(Real32)
		AVALON_NET_BASETYPE_OUTPUT(Real64)

#define AVALON_NET_BASETYPE_ARRAY_OUTPUT(Type) \
		NetOutputStream& SeriaArray(const Type* ptr,size_t num){ \
			size_t size = sizeof(Type) * num; \
			if(m_Index + size > m_Size) \
				throw NetSerializeException("NetOutputStream::out off buffer!"); \
			memmove(m_Buffer + m_Index,ptr,size);\
			m_Index += size; \
			return *this; \
		} \
		NetOutputStream& SeriaArray(Type* ptr,size_t num){ \
			return SeriaArray((const Type*)ptr, num); \
		}

		AVALON_NET_BASETYPE_ARRAY_OUTPUT(Int8)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(UInt8)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(Int16)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(UInt16)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(Int32)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(UInt32)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(Int64)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(UInt64)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(Real32)
		AVALON_NET_BASETYPE_ARRAY_OUTPUT(Real64)

		NetOutputStream& operator&(std::string& str)
		{
			return operator&((const std::string&)str);
		}

		NetOutputStream& operator&(const std::string& str)
		{
			UInt16 length = (UInt16)str.length();
			operator&(length);
			if(m_Index + length > m_Size)
				throw NetSerializeException("NetOutputStream::out off buffer!");
			memmove(m_Buffer + m_Index,str.c_str(),length);
			m_Index += length;
			return *this;
		}

		template<typename T>
		NetOutputStream& operator&(T& value)
		{
			value.Output(*this);
			return *this;
		}

		template<typename T>
		NetOutputStream& SeriaArray(T* ptr,size_t num)
		{
			for(size_t i = 0;i < num;++i)
			{
				this->operator&(ptr[i]);
			}
			return *this;
		}

		template<typename T>
		NetOutputStream& operator&(std::vector<T>& vec)
		{
			UInt16 length = (UInt16)vec.size();
			operator&(length);
			for(size_t i = 0; i < vec.size() ; ++i)
			{
				operator&(vec[i]);
			}
			return *this;
		}

		template<typename KeyT,typename ValueT>
		NetOutputStream& operator&(std::map<KeyT,ValueT>& list)
		{
			UInt16 length = (UInt16)list.size();
			operator&(length);
			for(typename std::map<KeyT,ValueT>::iterator iter = list.begin();iter != list.end();++iter)
			{
				operator&((KeyT)iter->first);
				operator&(iter->second);
			}
			return *this;
		}

		size_t GetPos() const
		{
			return m_Index;
		}

	private:
		//�ֽڻ���
		UInt8* m_Buffer;
		//�����С
		size_t m_Size;
		//��ǰ����
		size_t m_Index;
	};
}

#endif
