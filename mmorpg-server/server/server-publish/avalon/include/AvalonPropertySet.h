/**
 *@author	 
 *@email	 
 *@date		2011-02-26		
 *@brief	属性集，用于解析key-value型的配置
 */
#ifndef _AVALON_PROPERTYSET_H_
#define _AVALON_PROPERTYSET_H_

#include "AvalonStringUtil.h"
#include <string>
#include <map>

namespace Avalon
{
	/**
	 *@brief 属性遍历器
	 */
	class PropertyVisitor
	{
	public:
		virtual ~PropertyVisitor(){}

		virtual bool operator()(const std::string& key, const std::string& value) = 0;
	};

	/**
	 *@brief 属性集，属性名不区分大小写
	 */
	class PropertySet
	{
		typedef std::map<std::string,std::string> PROPERTY_MAP;
		typedef std::map<std::string,PROPERTY_MAP> SECTION_MAP;

		class Property
		{
		public:
			Property(const std::string& val):m_Value(val){}

			//定义到各种基本类型的转化操作符

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
		 *@brief 从文件中加载属性
		 *@param path 文件路径
		 */
		bool LoadFromFile(const std::string& path);

		/**
		 *@brief 从命令行加载属性
		 */
		bool LoadFromCmdline(const std::string& section,int argc,char* argv[]);

		/**
		 *@brief 保存到文件中
		 *@param path 文件路径
		 */
		bool SaveToFile(const std::string& path);

		/**
		 *@brief 获取一个属性
		 *@param key 属性名
		 */
		const Property GetProperty(const std::string& section,const std::string& key);

		/**
		 *@brief 设置一个属性
		 *@param key 属性名
		 */
		void SetProperty(const std::string& section,const std::string& key,const std::string& val);

		template<typename T>
		void SetProperty(const std::string& section,const std::string& key,T val);

		/**
		 *@brief 遍历属性
		 */
		void Visit(const std::string& section, PropertyVisitor& visitor);

	private:
		/**
		 *@brief 加载一个值
		 */
		bool LoadPair(PROPERTY_MAP* pSection,const std::string& pair);

		//属性map
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
