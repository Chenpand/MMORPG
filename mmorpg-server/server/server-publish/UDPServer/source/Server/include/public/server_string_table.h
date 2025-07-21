#ifndef __SERVER_STRING_TABLE_H__
#define __SERVER_STRING_TABLE_H__

#include "Common/common_base.h"
#include "Common/common_string.h"
#include "tinyxml/tinyxml.h"

namespace KingNet { namespace Server {



template<int32_t max_count, int16_t string_size, int32_t max_id>
class CStringTable
{
public:
	enum
	{
		__MAX_COUNT__	= max_count,
		__STRING_SIZE__	= string_size,
		__MAX_ID__		= max_id,

		__MAX_IDX__		= (__MAX_ID__ + 1),

		__invalid_idx__ = -1,
	};

	typedef char StringEntry[__STRING_SIZE__];

public:
	CStringTable()
	{
		//do nothing
	}

	~CStringTable()
	{
		//do nothing
	}

	CStringTable&   operator=(const   CStringTable& rhs)
	{
		if(this!=&rhs)   
  		{   
  			m_nVersion=rhs.m_nVersion;   
  	 		m_nAreaCode = rhs.m_nAreaCode;
  	 		m_iCount = rhs.m_iCount;
  	 		memcpy(&m_iIdx[0],&rhs.m_iIdx[0],sizeof(m_iIdx));
  	 		memcpy(&m_Entries[0],&rhs.m_Entries[0],sizeof(m_Entries));
  	 		m_iMinID = rhs.m_iMinID;
  	 		m_iMaxID = rhs.m_iMaxID;
  		}   
 		return   *this;   	
	};   

	int32_t load(const char* file);

	char* get_string(int32_t id);

protected:
	int16_t m_nVersion;
	int16_t m_nAreaCode;

	int32_t m_iCount;
	int32_t m_iIdx[__MAX_IDX__];
	StringEntry m_Entries[__MAX_COUNT__];

	int32_t m_iMinID;			//字符表内最小id
	int32_t m_iMaxID;			//字符表内最大id
private:
};




template<int32_t max_count, int16_t string_size, int32_t max_id>
int32_t CStringTable<max_count, string_size, max_id>::load(const char *file)
{
	if (NULL == file)
	{
		return fail;
	}

	TiXmlDocument string_table(file); 
	if (!string_table.LoadFile(TIXML_ENCODING_UTF8))
	{
		return fail;
	}

	TiXmlHandle handle(&string_table);

	for (size_t i = 0; i < sizeof(m_iIdx)/sizeof(m_iIdx[0]); ++i)
	{
		m_iIdx[i] = __invalid_idx__;
	}

	for (size_t i = 0; i < sizeof(m_Entries)/sizeof(m_Entries[0]); ++i)
	{
		m_Entries[i][0] = '\0';
	}

	m_iCount = 0;
	m_nVersion = 0;
	m_iMinID = 0;
	m_iMaxID = 0;

	int value = 0;
	const char* pszValue = NULL;

	TiXmlElement* head = handle.FirstChild("table").FirstChild("head").ToElement();
	if (NULL == head)
	{
		return fail;
	}

	pszValue = head->Attribute("version", &value);
	if (NULL != pszValue)
	{
		m_nVersion = int16_t(value);
	}
	else
	{
		m_nVersion = 0;
	}

	pszValue = head->Attribute("areacode", &value);
	if (NULL != pszValue)
	{
		m_nAreaCode = int16_t(value);
	}
	
	TiXmlElement* entry = handle.FirstChild("table").FirstChild("body").FirstChild("entry").ToElement();

	while(NULL != entry)
	{
		if (m_iCount >= __MAX_COUNT__)
		{
			break;
		}

		pszValue = entry->Attribute("id", &value);

		if (NULL == pszValue)
		{
			continue;
		}
			
		if (value < 0 || value > __MAX_ID__)
		{
			continue;
		}

		//记录entry在m_Entries中的索引值(下标)
		m_iIdx[value] = m_iCount;

		if (value > m_iMaxID)
		{
			m_iMaxID = (int32_t)value;
		}
		else if (value < m_iMinID)
		{
			m_iMinID = (int32_t)value;
		}

		//
		pszValue = entry->Attribute("content");
		if (NULL != pszValue)
		{
			STRNCPY(m_Entries[m_iCount], pszValue, sizeof(m_Entries[m_iCount]));
			
		}
		
		

		++m_iCount;
		entry = entry->NextSiblingElement("entry");
	}//while(NULL != entry)



	return success;
}

template<int32_t max_count, int16_t string_size, int32_t max_id>
char* CStringTable<max_count, string_size, max_id>::get_string(int32_t id)
{
	if (id < 0 || id > __MAX_ID__)
	{
		return NULL;
	}

	if (id < m_iMinID || id > m_iMaxID)
	{
		return NULL;
	}

	int32_t idx = m_iIdx[id];
	if (idx < 0 || idx >= int32_t(sizeof(m_Entries)/sizeof(m_Entries[0])))
	{
		return NULL;
	}

	return &m_Entries[idx][0];

}


/*

template<int32_t max_count, int16_t string_size, int32_t max_id>
CStringTable<max_count, string_size, max_id>&   CStringTable<max_count, string_size, max_id>::operator=(const   CStringTable<max_count, string_size, max_id>& rhs)
{
	if(this!=&rhs)   
  	{   
  		m_nVersion=rhs.m_nVersion;   
  	 	m_nAreaCode = rhs.m_nAreaCode;
  	 	m_iCount = rhs.m_iCount;
  	 	memcpy(&m_iIdx[0],&rhs.m_iIdx[0],sizeof(m_iIdx));
  	 	memcpy(&m_Entries[0],&rhs.m_Entries[0],sizeof(m_Entries));
  	 	m_iMinID = rhs.m_iMinID;
  	 	m_iMaxID = rhs.m_iMaxID;
  	}   
 	return   *this;   
}
*/

}}//namespace KingNet { namespace Server {

#endif //__SERVER_STRING_TABLE_H__

