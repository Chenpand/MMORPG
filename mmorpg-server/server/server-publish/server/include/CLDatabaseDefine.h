#ifndef _CL_DATABASE_DEFINE_H_
#define _CL_DATABASE_DEFINE_H_

#include <AvalonNetStream.h>
#include <AvalonObjectPool.h>
#include <AvalonMutex.h>
#include "CLDefine.h"

typedef UInt64 DBKey_t;

/**
 *@brief ֧�ֵ����ݿ���������ö��
 */
enum DBDataType
{
	DBT_INVALID,	//��Ч����
	DBT_INT8,		//int8
	DBT_UINT8,		//uint8
	DBT_INT16,		//int16
	DBT_UINT16,		//uint16
	DBT_INT32,		//int32
	DBT_UINT32,		//uint32
	DBT_INT64,		//int64
	DBT_UINT64,		//uint64
	DBT_REAL32,		//real32
	DBT_REAL64,		//real64
	DBT_STRING,		//string
	DBT_BINARY,		//binary
};

template<typename T>
struct DBTypeTraits;

#define CL_DEFINE_DBTYPE_TRAITS(Type,TypeID) \
	template<> \
	struct DBTypeTraits<Type> { \
		const static UInt8 ID = TypeID; \
	};

CL_DEFINE_DBTYPE_TRAITS(Int8,DBT_INT8)
CL_DEFINE_DBTYPE_TRAITS(UInt8,DBT_UINT8)
CL_DEFINE_DBTYPE_TRAITS(Int16,DBT_INT16)
CL_DEFINE_DBTYPE_TRAITS(UInt16,DBT_UINT16)
CL_DEFINE_DBTYPE_TRAITS(Int32,DBT_INT32)
CL_DEFINE_DBTYPE_TRAITS(UInt32,DBT_UINT32)
CL_DEFINE_DBTYPE_TRAITS(Int64,DBT_INT64)
CL_DEFINE_DBTYPE_TRAITS(UInt64,DBT_UINT64)
CL_DEFINE_DBTYPE_TRAITS(Real32,DBT_REAL32)
CL_DEFINE_DBTYPE_TRAITS(Real64,DBT_REAL64)
CL_DEFINE_DBTYPE_TRAITS(std::string,DBT_STRING)
CL_DEFINE_DBTYPE_TRAITS(char*,DBT_BINARY)

/**
 *@brief ���ݿ�����
 */
class CLDatabaseData
{
public:
	CLDatabaseData():m_Type(DBT_INVALID),m_Val(0){}
	
	CLDatabaseData(const CLDatabaseData& data)
	{
		m_Type = data.m_Type;
		if(IsBlock())
		{
			UInt16 len = *(UInt16*)data.m_Blob;
			m_Blob = new char[len + sizeof(UInt16)];
			memmove(m_Blob, data.m_Blob, len + sizeof(UInt16));
		}
		else m_Val = data.m_Val;
	}
	
	~CLDatabaseData()
	{
		if(IsBlock()) CL_SAFE_DELETE_ARRAY(m_Blob);
		m_Type = DBT_INVALID;
	}

	const CLDatabaseData& operator=(const CLDatabaseData& data)
	{
		if(this == &data) return *this;

		if(IsBlock()) CL_SAFE_DELETE_ARRAY(m_Blob);
		m_Type = data.m_Type;
		m_Val = 0;
		if(IsBlock())
		{
			UInt16 len = *(UInt16*)data.m_Blob;
			m_Blob = new char[len + sizeof(UInt16)];
			memmove(m_Blob, data.m_Blob, len + sizeof(UInt16));
		}
		else m_Val = data.m_Val;
		return *this;
	}

#define CL_BASE_TO_DBDATA(Type) \
	CLDatabaseData(Type val){ \
		m_Type = DBTypeTraits<Type>::ID; \
		m_##Type = val; \
	} \

	CL_BASE_TO_DBDATA(Int8)
	CL_BASE_TO_DBDATA(UInt8)
	CL_BASE_TO_DBDATA(Int16)
	CL_BASE_TO_DBDATA(UInt16)
	CL_BASE_TO_DBDATA(Int32)
	CL_BASE_TO_DBDATA(UInt32)
	CL_BASE_TO_DBDATA(Int64)
	CL_BASE_TO_DBDATA(UInt64)
	CL_BASE_TO_DBDATA(Real32)
	CL_BASE_TO_DBDATA(Real64)

	CLDatabaseData(const std::string& str){
		m_Type = DBTypeTraits<std::string>::ID;

		UInt16 len = UInt16(str.length());
		m_Blob = new char[len + sizeof(UInt16)];
		*(UInt16*)m_Blob = len;
		memmove(m_Blob + sizeof(UInt16), str.c_str(), len);
	}

	CLDatabaseData(const char* data, size_t len){
		m_Type = DBTypeTraits<char*>::ID;

		m_Blob = new char[UInt16(len) + sizeof(UInt16)];
		*(UInt16*)m_Blob = UInt16(len);
		memmove(m_Blob + sizeof(UInt16), data, UInt16(len));
	}


public:
	/**
	 *@brief �ж��ǲ�������
	 */
	bool IsInteger() const{
		return m_Type > DBT_INVALID && m_Type <= DBT_UINT64; 
	}

	/**
	 *@brief �ж��ǲ��Ǹ���
	 */
	bool IsFloat() const{
		return m_Type == DBT_REAL32 || m_Type == DBT_REAL64;
	}

	/**
	 *@brief �ж��ǲ��ǳ����ݿ�
	 */
	bool IsBlock() const{
		return m_Type == DBT_STRING || m_Type == DBT_BINARY;
	}

	/**
	 *@brief ��ȡ����
	 */
	int GetType() const{ return m_Type; }

public:

#define CL_DBDATA_TOBASE_BEGIN(Type) \
	operator Type() const{ \
		switch(m_Type){ 

#define CL_DBDATA_TOBASE_END() \
		} \
		return 0; \
	}

#define CL_CASE_DBDATA_TOBASE(Type) \
	case DBTypeTraits<Type>::ID: return m_##Type;

	CL_DBDATA_TOBASE_BEGIN(Int8)
	CL_CASE_DBDATA_TOBASE(Int8)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(UInt8)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(Int16)
	CL_CASE_DBDATA_TOBASE(Int8)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(Int16)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(UInt16)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(Int32)
	CL_CASE_DBDATA_TOBASE(Int8)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(Int16)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_CASE_DBDATA_TOBASE(Int32)
	CL_CASE_DBDATA_TOBASE(UInt32)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(UInt32)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_CASE_DBDATA_TOBASE(UInt32)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(Int64)
	CL_CASE_DBDATA_TOBASE(Int8)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(Int16)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_CASE_DBDATA_TOBASE(Int32)
	CL_CASE_DBDATA_TOBASE(UInt32)
	CL_CASE_DBDATA_TOBASE(Int64)
	CL_CASE_DBDATA_TOBASE(UInt64)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(UInt64)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_CASE_DBDATA_TOBASE(UInt32)
	CL_CASE_DBDATA_TOBASE(UInt64)
	CL_DBDATA_TOBASE_END()
	
	CL_DBDATA_TOBASE_BEGIN(Real32)
	CL_CASE_DBDATA_TOBASE(Int8)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(Int16)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_CASE_DBDATA_TOBASE(Real32)
	CL_DBDATA_TOBASE_END()

	CL_DBDATA_TOBASE_BEGIN(Real64)
	CL_CASE_DBDATA_TOBASE(Int8)
	CL_CASE_DBDATA_TOBASE(UInt8)
	CL_CASE_DBDATA_TOBASE(Int16)
	CL_CASE_DBDATA_TOBASE(UInt16)
	CL_CASE_DBDATA_TOBASE(Int32)
	CL_CASE_DBDATA_TOBASE(UInt32)
	CL_CASE_DBDATA_TOBASE(Real32)
	CL_CASE_DBDATA_TOBASE(Real64)
	CL_DBDATA_TOBASE_END()

	std::string GetString() const
	{
		if(m_Type == DBTypeTraits<std::string>::ID){
			std::string str;
			UInt16 len = *(UInt16*)m_Blob;
			str.assign(m_Blob + sizeof(UInt16), len);
			return str;
		}
		return "";
	}

	size_t GetBytes(char* buf, size_t len) const
	{
		if(IsBlock())
		{
			len = len < *(UInt16*)m_Blob ? len : *(UInt16*)m_Blob;
			memmove(buf, m_Blob + sizeof(UInt16), len);
			return len;
		}
		return 0;
	}

	const char* GetBlobPtr() const{
		return IsBlock() ? ( m_Blob + sizeof(UInt16) ) : NULL ;
	}

	size_t GetBlobSize() const{
		return IsBlock() ? *(UInt16*)m_Blob : 0;
	}

public://���л�����

	void Input(Avalon::NetInputStream& stream);
	void Output(Avalon::NetOutputStream& stream);

	void InputValue(UInt8 type,Avalon::NetInputStream& stream);
	void OutputValue(Avalon::NetOutputStream& stream);

	void OutputValue(UInt8 dataType, Avalon::NetOutputStream& stream);

private:
	//����
	UInt8	m_Type;

#define CL_DEFINE_DBBASE_VAL(Type) \
	Type	m_##Type;

	//����
	union
	{
		CL_DEFINE_DBBASE_VAL(Int8)
		CL_DEFINE_DBBASE_VAL(UInt8)
		CL_DEFINE_DBBASE_VAL(Int16)
		CL_DEFINE_DBBASE_VAL(UInt16)
		CL_DEFINE_DBBASE_VAL(Int32)
		CL_DEFINE_DBBASE_VAL(UInt32)
		CL_DEFINE_DBBASE_VAL(Int64)
		CL_DEFINE_DBBASE_VAL(UInt64)
		CL_DEFINE_DBBASE_VAL(Real32)
		CL_DEFINE_DBBASE_VAL(Real64)
		char*	m_Blob;
		Int64	m_Val;
	};
};


/**
 *@brief ����guid��guid�ṹ
 *    |10λzone id|4λ����������|6λ������id|44λ���к�|
 */
#define CL_MAKE_GUID(Seed) \
	((UInt64(ZONE_ID) << 54) + (UInt64(SERVER_TYPE) << 50) + (UInt64(SERVER_ID) << 44) + Seed)

//noted by aprilliu �����ڴ�����id��������scene object���ڴ�id�����������Ե�guid
#define CL_MAKE_TEMP_GUID(Seed) \
	((UInt64(SERVER_TYPE) << 50) + (UInt64(SERVER_ID) << 44) + Seed)

 /**
  *@brief ��guid��ȡ�ڵ�����
  */
#define CL_GET_NODETYPE_FROM_GUID(Guid) (UInt32(Guid << 10 >> 60))
/**
 *@brief ��guid��ȡ�ڵ�id
 */
#define CL_GET_NODEID_FROM_GUID(Guid) (UInt32(Guid << 14 >> 58))
 /**
  *@brief ��guid��ȡ����
  */
#define CL_GET_SEED_FROM_GUID(Guid) ((Guid << 20 >> 20))


#endif
