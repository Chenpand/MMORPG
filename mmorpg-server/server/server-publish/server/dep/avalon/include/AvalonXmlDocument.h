/**
 *@author	 
 *@email	 
 *@date		2011-04-24	
 *@brief	xml�ĵ�
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
	 *@brief xml����
	 */
	class XmlDocument
	{
	public:
		explicit XmlDocument(const std::string& name);
		~XmlDocument();

	public:
		/**
		 *@brief �����ļ�
		 */
		bool LoadFile();

		/**
		 *@brief ��ȡ���ڵ�
		 */
		XmlElement* RootElement();

		/**
		 *@brief ��ȡ��һ���ӽڵ�
		 */
		XmlElement* FirstChildElement(XmlElement* element);

		/**
		 *@brief ��ȡ��һ��ָ�����ֵ��ӽڵ�
		 */
		XmlElement* FirstChildElement(XmlElement* element,const std::string& name);

		/**
		 *@brief ��ȡ��һ���ڵ�
		 */
		XmlElement* NextElement(XmlElement* element);

		/**
		 *@brief ��ȡ��һ��ָ�����ֵĽڵ�
		 */
		XmlElement* NextElement(XmlElement* element,const std::string& name);

		/**
		 *@brief ��ȡһ������
		 */
		bool GetAttribute(XmlElement* element,const std::string& name,Int16& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,UInt16& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,Int32& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,UInt32& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,Real32& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,Real64& val) const;
		bool GetAttribute(XmlElement* element,const std::string& name,std::string& str) const;

		/**
		 *@brief ��ȡ����
		 */
		const char* GetText(XmlElement* element) const;

		/**
		 *@brief ��ȡ����
		 */
		bool GetElementName(XmlElement* element, std::string& val);

	private:
		//tinyxml����
		TiXmlDocument* m_pDocument;
	};
}

#endif
