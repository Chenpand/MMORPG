/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	���Լ������ڽ���key-value�͵�����
 */
#ifndef _AVALON_PROPERTYSET_H_
#define _AVALON_PROPERTYSET_H_

#include "AvalonStringUtil.h"
#include <string>
#include <map>

namespace Avalon
{
	/**
	 *@brief ���Ա�����
	 */
	class PropertyVisitor
	{
	public:
		virtual ~PropertyVisitor(){}

		virtual bool operator()(const std::string& key, const std::string& value) = 0;
	};

	/**
	 *@brief ���Լ��������������ִ�Сд
	 */
	class PropertySet
	{
		typedef std::map<std::string,std::string> PROPERTY_MAP;
		typedef std::map<std::string,PROPERTY_MAP> SECTION_MAP;

		class Property
		{
		public:
			Property(const std::string& val):m_Value(val){}

			//���嵽���ֻ������͵�ת��������

#define		CL_PROPERTY_TO_BASETYPE(Type) \
			operator Type() const { \
				return StringUtil::ConvertToValue<Type>(m_Value); \
			}

			CL_PROPERTY_TO_BASETYPE(Int8)
			CL_PROPERTY_TO_BASETYPE(UInt8)
			CL_PROPERTY_TO_BASETYPE(Int16)
			CL_PROPERTY_TO_BASETYPE(UInt16)
			CL_PROPERTY_TO_BASETYPE(Int32)
			CL_PROPERTY_TO_BASETYPE(UInt32)
			CL_PROPERTY_TO_BASETYPE(Int64)
			CL_PROPERTY_TO_BASETYPE(UInt64)
			CL_PROPERTY_TO_BASETYPE(Real32)
			CL_PROPERTY_TO_BASETYPE(Real64)

			const std::string GetString() const{
				return m_Value;
			}

			operator bool() const
			{
				std::string val = m_Value;
				StringUtil::ToLowerCase(val);
				return "true" == val;
			}

		private:
			std::string m_Value;
		};

	public:
		PropertySet(){}
		~PropertySet(){}

		/**
		 *@brief ���ļ��м�������
		 *@param path �ļ�·��
		 */
		bool LoadFromFile(const std::string& path);

		/**
		 *@brief �������м�������
		 */
		bool LoadFromCmdline(const std::string& section,int argc,char* argv[]);

		/**
		 *@brief ���浽�ļ���
		 *@param path �ļ�·��
		 */
		bool SaveToFile(const std::string& path);

		/**
		 *@brief ��ȡһ������
		 *@param key ������
		 */
		const Property GetProperty(const std::string& section,const std::string& key);

		/**
		 *@brief ����һ������
		 *@param key ������
		 */
		void SetProperty(const std::string& section,const std::string& key,const std::string& val);

		template<typename T>
		void SetProperty(const std::string& section,const std::string& key,T val);

		/**
		 *@brief ��������
		 */
		void Visit(const std::string& section, PropertyVisitor& visitor);

	private:
		/**
		 *@brief ����һ��ֵ
		 */
		bool LoadPair(PROPERTY_MAP* pSection,const std::string& pair);

		//����map
		SECTION_MAP	m_Properties;
	};

	template<typename T>
	inline void PropertySet::SetProperty(const std::string& section,const std::string &key, T val)
	{
		SetProperty(section,key,StringUtil::ConvertToString(val));
	}

	template<>
	inline void PropertySet::SetProperty<bool>(const std::string& section,const std::string& key,bool val)
	{
		SetProperty(section,key,val?"true":"false");
	}
}

#endif
