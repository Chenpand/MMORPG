#include "AvalonRegex.h"

namespace Avalon
{
	#include "pcre/pcre.h"

	Regex::Regex(const char* pattern)
		:m_pErrMsg(NULL), m_pPcre(NULL), m_pPcreExtra(NULL)
	{
		Compile(pattern);
	}

	Regex::Regex()
		:m_pErrMsg(NULL), m_pPcre(NULL), m_pPcreExtra(NULL)
	{
	}

	Regex::~Regex()
	{
		Clear();
	}

	bool Regex::Compile(const char* pattern)
	{
		Clear();

		int erroffset = 0;
		m_pPcre = pcre_compile(pattern, PCRE_UTF8|PCRE_NO_AUTO_CAPTURE, &m_pErrMsg, &erroffset, NULL);
		if(m_pPcre == NULL) return false;

		m_pPcreExtra = pcre_study(m_pPcre, 0, &m_pErrMsg);
		if(m_pPcreExtra == NULL && m_pErrMsg != NULL)
		{
			pcre_free(m_pPcre);
			m_pPcre = NULL;
			return false;
		}

		return true;
	}

	void Regex::Clear()
	{
		m_pErrMsg = NULL;
		pcre_free(m_pPcre);
		m_pPcre = NULL;
		pcre_free_study(m_pPcreExtra);
		m_pPcreExtra = NULL;
	}

	int Regex::Match(const char* str, int len, int* offsets, int oflen) const
	{
		if(!IsValid() || str == NULL || len < 0) return -1;
		int ret = pcre_exec(m_pPcre, m_pPcreExtra, str, len, 0, PCRE_NO_UTF8_CHECK, offsets, oflen);
		if(ret < 0) return -1;
		return ret;
	}
}
