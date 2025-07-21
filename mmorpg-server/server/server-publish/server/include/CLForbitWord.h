#ifndef _CL_FORBIT_WORD_H_
#define _CL_FORBIT_WORD_H_

#include <AvalonSingleton.h>
#include <CLUtf8Define.h>

#define MAX_FORBIT_WORD_LEN 128
#define FORBIT_WORD_CHAR	'*'

enum ForbitWordRet
{
	FORBIT_WORD_NOUTF8 = 0,		//��utf8
	FORBIT_WORD_REPLACED = 1,	//���滻
	FORBIT_WORD_UNCHANGED = 2,  //δ�仯
};

/**
 *@brief �����ֽ�����
 */
class ForbitWordParser : public Avalon::Singleton<ForbitWordParser>
{
	typedef std::multimap<Int32, Int32*> WordMap;

public:
	ForbitWordParser();
	~ForbitWordParser();

public:
	bool Init(const std::string& file);

	// ��ʼ�����õ������ַ� 
	bool InitValidCoolName(const std::string file);

	void Final();

public:
	Int32 Parse(char* utf8str);

private:
	//������
	WordMap m_Words;
};

#endif
