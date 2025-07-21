#include "public/server_dirty_words_filter.h"
#include "Common/common_string.h"
#include "Common/common_base.h"
#include "public/server_message.h"
#include "tinyxml/tinyxml.h"

namespace KingNet { namespace Server {

	CDirtyWordsFilter::CDirtyWordsFilter()
	{
		clear();
	}

	CDirtyWordsFilter::~CDirtyWordsFilter()
	{
		//do nothing
	}

	void CDirtyWordsFilter::clear()
	{
		m_iCount = 0;

		for (int32_t i=0; i<max_dirty_words_count; ++i)
		{
			m_stConifg[i].m_szWord[0] = '\0';
			m_stConifg[i].m_cLevel    = 0;
		}
	}

	int32_t CDirtyWordsFilter::load(char* file)
	{
		if (NULL == file)
		{
			return fail;
		}

		TiXmlDocument dirty_words(file); 
		if (!dirty_words.LoadFile(TIXML_ENCODING_UTF8))
		{
			return fail;
		}

		TiXmlHandle handle(&dirty_words);

		TiXmlElement* pstWordElement = handle.FirstChild("DirtyWords").FirstChild("Word").ToElement();
		if (NULL == pstWordElement)
		{
			return fail;
		}

		int value = 0;
		const char* pszvalue = NULL;

		m_iCount = 0;

		while (NULL != pstWordElement)
		{
			if (m_iCount >= int32_t(sizeof(m_stConifg)/sizeof(m_stConifg[0])))
			{
				break;
			}
		
			pszvalue = pstWordElement->Attribute("content");
			if (NULL == pszvalue)
			{
				continue;
			}

			STRNCPY(m_stConifg[m_iCount].m_szWord, pszvalue, sizeof(m_stConifg[m_iCount].m_szWord));
			StringTrimAndToLowerCase(m_stConifg[m_iCount].m_szWord);

			pszvalue = pstWordElement->Attribute("level", &value);
			if (NULL == pszvalue)
			{
				m_stConifg[m_iCount].m_cLevel = filter_level_low;
			}
			else
			{
				m_stConifg[m_iCount].m_cLevel = (int8_t)value;
			}

			//step
			++m_iCount;
			pstWordElement = pstWordElement->NextSiblingElement("Word");
		}

		return success;
	}

#if 0
	int32_t CDirtyWordsFilter::load(char* file)
	{
		if (NULL == file)
		{
			return fail;
		}

		TiXmlDocument dirty_words(file); 
		if (!dirty_words.LoadFile(TIXML_ENCODING_UTF8))
		{
			return fail;
		}

		TiXmlElement* root_element = dirty_words.RootElement();
		if (NULL == root_element)
		{
			return fail;
		}

		TiXmlElement* first_element = root_element->FirstChildElement("Count");
		if (NULL == first_element)
		{
			return fail;
		}

		m_iCount = atoi(first_element->GetText());
		if (0 >= m_iCount)
		{
			return fail;
		}
		else if (max_dirty_words_count < m_iCount)
		{
			m_iCount = max_dirty_words_count;
		}

		TiXmlElement* sibling_element  = first_element->NextSiblingElement("Word");
		const char* pszvalue = NULL;

		for (int32_t i=0; i<m_iCount; ++i)
		{
			if (NULL == sibling_element)
			{
				return fail;
			}

			pszvalue = sibling_element->Attribute("content");
			if (NULL == pszvalue)
			{
				return fail;
			}
			else
			{
				STRNCPY(m_Words[i], pszvalue, sizeof(m_Words[i]));
			}

			if (i == (m_iCount-1))
			{
				break;
			}
			else
			{
				sibling_element = sibling_element->NextSiblingElement("Word");
			}
		}

		return success;
	}


#endif

	int32_t CDirtyWordsFilter::reload(char* file)
	{
		if (NULL == file)
		{
			return fail;
		}

		clear();
		return load(file);
	}

	int32_t CDirtyWordsFilter::load( char* pszData, int32_t size )
	{
		if (NULL == pszData || size <= 0)
		{
			return fail;
		}
		
		TiXmlDocument xmldoc;

		xmldoc.Parse(pszData, NULL, TIXML_ENCODING_UTF8);
		if (xmldoc.Error())
		{
			return fail;
		}

		TiXmlHandle handle(&xmldoc);
		TiXmlElement* pstWordElement = handle.FirstChild("DirtyWords").FirstChild("Word").ToElement();
		if (NULL == pstWordElement)
		{
			return fail;
		}

		int value = 0;
		const char* pszvalue = NULL;

		m_iCount = 0;

		while (NULL != pstWordElement)
		{
			if (m_iCount >= int32_t(sizeof(m_stConifg)/sizeof(m_stConifg[0])))
			{
				break;
			}

			pszvalue = pstWordElement->Attribute("content");
			if (NULL == pszvalue)
			{
				continue;
			}

			STRNCPY(m_stConifg[m_iCount].m_szWord, pszvalue, sizeof(m_stConifg[m_iCount].m_szWord));
			StringTrimAndToLowerCase(m_stConifg[m_iCount].m_szWord);

			pszvalue = pstWordElement->Attribute("level", &value);
			if (NULL == pszvalue)
			{
				m_stConifg[m_iCount].m_cLevel = filter_level_low;
			}
			else
			{
				m_stConifg[m_iCount].m_cLevel = (int8_t)value;
			}

			//step
			++m_iCount;
			pstWordElement = pstWordElement->NextSiblingElement("Word");
		}

		return success;
	}

	int32_t CDirtyWordsFilter::reload( char* pszData, int32_t size )
	{
		if (NULL == pszData || size <= 0)
		{
			return fail;
		}

		clear();

		return load(pszData, size);
	}


	

	bool CDirtyWordsFilter::filter(char* haystack, int8_t level)
	{
		char szTmpStr[max_input_string] = {'\0',};

		if (NULL != haystack)
		{
			//copy string 
			STRNCPY(szTmpStr, haystack, sizeof(szTmpStr));

			//trim and to case
			StringTrimAndToLowerCase(szTmpStr);

			char* matched = NULL;
			for(int32_t i=0; i<m_iCount; ++i)
			{
				if (level >= m_stConifg[i].m_cLevel)
				{
					matched = strstr(szTmpStr, m_stConifg[i].m_szWord);
					if (NULL != matched)
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	char* CDirtyWordsFilter::filter(char* haystack, int32_t& needle_index, int8_t level)
	{
		char szTmpStr[max_input_string] = {'\0',};
		char* matched = NULL;
		needle_index  = -1;

		if (NULL != haystack)
		{
			//copy string 
			STRNCPY(szTmpStr, haystack, sizeof(szTmpStr));

			//trim and to case
			StringTrimAndToLowerCase(szTmpStr);

			for(int32_t i=0; i<m_iCount; ++i)
			{
				if (level >= m_stConifg[i].m_cLevel)
				{
					matched = strstr(szTmpStr, m_stConifg[i].m_szWord);
					if (NULL != matched)
					{
						needle_index = i;
						return matched;
					}
				}

			}
		}

		return matched;
	}

	char* CDirtyWordsFilter::get_needle(int32_t needle_index)
	{
		if ((needle_index < 0) || (needle_index >= m_iCount))
		{
			return NULL;
		}
		else
		{
			return m_stConifg[needle_index].m_szWord;
		}
	}

}}

