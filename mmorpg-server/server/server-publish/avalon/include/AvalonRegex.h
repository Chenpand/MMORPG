/**
 *@author	 
 *@email	 
 *@date		2012-10-27	
 *@brief	������ʽ
 */
#ifndef _AVALON_REGEX_H_
#define _AVALON_REGEX_H_

#include <AvalonDefine.h>

namespace Avalon
{
	struct real_pcre;
	struct pcre_extra;

	/**
	 *@brief ������ʽ
	 */
	class Regex
	{
	public:
		Regex(const char* pattern);
		Regex();
		~Regex();

	public:
		/**
		 *@brief ����һ��������ʽ
		 */
		bool Compile(const char* pattern);

		/**
		 *@brief �ж��Ƿ���Ч��������ʽ
		 */
		bool IsValid() const{ return m_pPcre != NULL; }

		/**
		 *@brief ���ش�����Ϣ
		 */
		const char* ErrMsg() const{ return m_pErrMsg == NULL ? "" : m_pErrMsg; }

		/**
		 *@brief ����ƥ��
		 */
		int Match(const char* str, int len, int* offsets, int oflen) const;

	private:
		void Clear();

	private:
		//������Ϣ
		const char*			m_pErrMsg;
		//pcre
		struct real_pcre*	m_pPcre;
		//pcre_extra
		struct pcre_extra*	m_pPcreExtra;
	};
};

#endif
