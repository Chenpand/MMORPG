#include "AvalonXmlDocument.h"
#include "AvalonStringUtil.h"
#include "tinyxml/tinyxml.h"

namespace Avalon
{

	XmlDocument::XmlDocument(const std::string& name)
	{
		m_pDocument = new TiXmlDocument(name);
	}

	XmlDocument::~XmlDocument()
	{
		delete m_pDocument;
	}

	bool XmlDocument::LoadFile()
	{
		return m_pDocument->LoadFile();
	}

	XmlElement* XmlDocument::RootElement()
	{
		return m_pDocument->RootElement();
	}

	XmlElement* XmlDocument::FirstChildElement(XmlElement *element)
	{
		return element == NULL ? NULL : element->FirstChildElement();
	}

	XmlElement* XmlDocument::FirstChildElement(XmlElement* element,const std::string& name)
	{
		return element == NULL ? NULL : element->FirstChildElement(name);
	}

	XmlElement* XmlDocument::NextElement(XmlElement *element)
	{
		return element == NULL ? NULL : element->NextSiblingElement();
	}

	XmlElement* XmlDocument::NextElement(XmlElement *element, const std::string &name)
	{
		return element == NULL ? NULL : element->NextSiblingElement(name);
	}

	bool XmlDocument::GetAttribute(XmlElement *element, const std::string &name, Int16 &val) const
	{
		if(element == NULL) return false;

		const char* str = element->Attribute(name.c_str());
		if(str != NULL)
		{
			val = StringUtil::ConvertToValue<Int16>(str);
			return true;
		}
		return false;
	}

	bool XmlDocument::GetAttribute(XmlElement *element, const std::string &name, UInt16 &val) const
	{
		if(element == NULL) return false;

		const char* str = element->Attribute(name.c_str());
		if(str != NULL)
		{
			val = StringUtil::ConvertToValue<UInt16>(str);
			return true;
		}
		return false;
	}

	bool XmlDocument::GetAttribute(XmlElement *element, const std::string &name, Int32 &val) const
	{
		if(element == NULL) return false;

		return element->QueryIntAttribute(name.c_str(),&val) == TIXML_SUCCESS;
	}

	bool XmlDocument::GetAttribute(XmlElement *element, const std::string &name, UInt32 &val) const
	{
		if(element == NULL) return false;

		const char* str = element->Attribute(name.c_str());
		if(str != NULL)
		{
			val = StringUtil::ConvertToValue<UInt32>(str);
			return true;
		}
		return false;
	}

	bool XmlDocument::GetAttribute(XmlElement* element, const std::string& name, Real32 &val) const
	{
		if(element == NULL) return false;

		return element->QueryFloatAttribute(name.c_str(),&val) == TIXML_SUCCESS;
	}

	bool XmlDocument::GetAttribute(XmlElement* element, const std::string& name, Real64 &val) const
	{
		if(element == NULL) return false;

		return element->QueryDoubleAttribute(name.c_str(),&val) == TIXML_SUCCESS;
	}

	bool XmlDocument::GetAttribute(XmlElement *element, const std::string &name, std::string &str) const
	{
		if(element == NULL) return false;

		return element->QueryStringAttribute(name.c_str(),&str) == TIXML_SUCCESS;
	}

	const char* XmlDocument::GetText(XmlElement* element) const
	{
		if(element == NULL) return NULL;

		return element->GetText();
	}

	bool XmlDocument::GetElementName(XmlElement* element, std::string& val)
	{
		if(element == NULL) return false;
		
		val = element->ValueStr();
		return true;
	}
}
