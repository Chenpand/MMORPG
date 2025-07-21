/**
 *@author	 
 *@email	 
 *@date		2011-04-24	
 *@brief	xml文档
 */
#ifndef _AVALON_XMLDOCUMENT_H_
#define _AVALON_XMLDOCUMENT_H_

#include "AvalonDefine.h"
#include <string>

class TiXmlElement;
class TiXmlDocument;

namespace Avalon
{
	typedef TiXmlElement XmlElement;

	/**
	 *@brief xml档案
	 */
	class XmlDocument
	{
	public:
		explicit XmlDocument(const std::string& name);
		~XmlDocument();

	public:
		/**
		 *@brief 加载文件
		 */
		bool LoadFile();

		/**
		 *@brief 获取根节点
		 */
		XmlElement* RootElement();

		/**
		 *@brief 获取第一个子节点
		 */
		XmlElement* FirstChildElement(XmlElement* element);

		/**
		 *@brief 获取第一个指定名字的子节点
		 */
		XmlElement* FirstChildElement(XmlElement* element,const std::string& name);

		/**
		 *@brief 获取下一个节点
		 */
		XmlElement* NextElement(XmlElement* element);

		/**
		 *@brief 获取下一个指定名字的节点
		 */
		XmlElement* NextElement(XmlElement* element,const std::string& name);

		/**
		 *@brief 获取一个属性
		 */
		bool GetAttribute(XmlElement* element,const std::string& name,Int16& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,UInt16& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,Int32& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,UInt32& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,Real32& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,Real64& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,std::string& str) const;

		/**
		 *@brief 获取内容
		 */
		const char* GetText(XmlElement* element) const;

		/**
		 *@brief 获取名字
		 */
		bool GetElementName(XmlElement* element, std::string& val);

	private:
		//tinyxml档案
		TiXmlDocument* m_pDocument;
	};
}

#endif
