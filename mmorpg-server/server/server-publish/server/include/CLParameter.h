#ifndef _CL_PARAMETER_H_
#define _CL_PARAMETER_H_

#include <AvalonStringUtil.h>
#include <AvalonNetStream.h>
#include <string>
#include <map>
#include "CLDefine.h"

/**
 *@brief ����,key=value ��ʽ
 */
class CLParameter
{
	//����map
	typedef std::map<std::string,std::string> ParamMap;

	/**
	 *@brief ֵ
	 */
	class Value
	{
	public:
		Value(const std::string& val):m_Value(val){}

#define	STRINGVAL_TO_BASETYPE(Type) \
		operator Type() const { \
			return Avalon::StringUtil::ConvertToValue<Type>(m_Value); \
		}

		STRINGVAL_TO_BASETYPE(Int8)
		STRINGVAL_TO_BASETYPE(UInt8)
		STRINGVAL_TO_BASETYPE(Int16)
		STRINGVAL_TO_BASETYPE(UInt16)
		STRINGVAL_TO_BASETYPE(Int32)
		STRINGVAL_TO_BASETYPE(UInt32)
		STRINGVAL_TO_BASETYPE(Int64)
		STRINGVAL_TO_BASETYPE(UInt64)
		STRINGVAL_TO_BASETYPE(Real32)
		STRINGVAL_TO_BASETYPE(Real64)
		
		const std::string& GetString() const{
			return m_Value;
		}

	private:
		std::string m_Value;
	};

public:
	CLParameter();
	~CLParameter();

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & m_Params;
	}

public:
	/**
	 *@brief ��������
	 */
	bool ParseParams(const std::string& params);

	/**
	 *@brief ���
	 */
	void Clear(); 

public:

	/**
	 *@brief �ж��Ƿ����ƶ�����
	 */
	bool HasParam(const std::string& name) const;

	/**
	 *@brief ��ȡһ������
	 */
	const Value GetParam(const std::string& name) const;

	/**
	 *@brief ����һ������
	 */
	void SetParam(const std::string& name,const std::string& val);
	template<typename T>
	void SetParam(const std::string& name,T val);

	/**
	 *@brief ת��Ϊ�ַ���
	 */
	std::string ToString() const;

private:
	//����
	ParamMap	m_Params;
};

template<typename T>
inline void CLParameter::SetParam(const std::string &key, T val)
{
	SetParam(key, Avalon::StringUtil::ConvertToString(val));
}

#endif
