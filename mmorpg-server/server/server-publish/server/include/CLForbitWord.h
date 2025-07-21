#ifndef _CL_FORBIT_WORD_H_
#define _CL_FORBIT_WORD_H_

#include <AvalonSingleton.h>
#include <CLUtf8Define.h>

#define MAX_FORBIT_WORD_LEN 128
#define FORBIT_WORD_CHAR	'*'

enum ForbitWordRet
{
	FORBIT_WORD_NOUTF8 = 0,		//非utf8
	FORBIT_WORD_REPLACED = 1,	//已替换
	FORBIT_WORD_UNCHANGED = 2,  //未变化
};

/**
 *@brief 屏蔽字解析器
 */
class ForbitWordParser : public Avalon::Singleton<ForbitWordParser>
{
	typedef std::multimap<Int32, Int32*> WordMap;

public:
	ForbitWordParser();
	~ForbitWordParser();

public:
	bool Init(const std::string& file);

	// 初始化可用的特殊字符 
	bool InitValidCoolName(const std::string file);

	void Final();

public:
	Int32 Parse(char* utf8str);

private:
	//屏蔽字
	WordMap m_Words;
};

#endif
