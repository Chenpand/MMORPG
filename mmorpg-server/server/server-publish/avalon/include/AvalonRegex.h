/**
 *@author	 
 *@email	 
 *@date		2012-10-27	
 *@brief	正则表达式
 */
#ifndef _AVALON_REGEX_H_
#define _AVALON_REGEX_H_

#include <AvalonDefine.h>

namespace Avalon
{
	struct real_pcre;
	struct pcre_extra;

	/**
	 *@brief 正则表达式
	 */
	class Regex
	{
	public:
		Regex(const char* pattern);
		Regex();
		~Regex();

	public:
		/**
		 *@brief 编译一个正则表达式
		 */
		bool Compile(const char* pattern);

		/**
		 *@brief 判断是否有效的正则表达式
		 */
		bool IsValid() const{ return m_pPcre != NULL; }

		/**
		 *@brief 返回错误信息
		 */
		const char* ErrMsg() const{ return m_pErrMsg == NULL ? "" : m_pErrMsg; }

		/**
		 *@brief 进行匹配
		 */
		int Match(const char* str, int len, int* offsets, int oflen) const;

	private:
		void Clear();

	private:
		//错误信息
		const char*			m_pErrMsg;
		//pcre
		struct real_pcre*	m_pPcre;
		//pcre_extra
		struct pcre_extra*	m_pPcreExtra;
	};
};

#endif
