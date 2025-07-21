#include "CLForbitWord.h"
#include <fstream>
#include <vector>

std::set<Int32> COOL_NAME_CHARS;

ForbitWordParser::ForbitWordParser()
{
}

ForbitWordParser::~ForbitWordParser()
{
	Final();
}

bool ForbitWordParser::Init(const std::string& file)
{
	std::ifstream stream(file.c_str(), std::ios_base::in);
	if(!stream) return false;

	char buf[MAX_FORBIT_WORD_LEN];
	memset(buf, 0, sizeof(buf));
	Int32 unicodebuf[MAX_FORBIT_WORD_LEN];
	memset(unicodebuf, 0, sizeof(unicodebuf));

	while(stream)
	{
		stream.getline(buf, MAX_FORBIT_WORD_LEN - 1);
		buf[MAX_FORBIT_WORD_LEN - 1] = 0;

		char* ptr = buf;
		Int32 unicodeIndex = 0;
		while(*ptr != 0 && *ptr != 13)
		{
			int len = GetUtf8Length(*ptr);
			for(int i = 0; i < len; ++i)
			{
				if(*(ptr + i) == 0)
				{
					stream.close();
					Final();
					return false;
				}
			}

			Int32 unicode = GetUtf8Unicode(ptr, len);
			if (IsEnglishLowerUnicode(unicode))
				unicode = towupper(unicode);

			if(unicode < 0)
			{
				stream.close();
				Final();
				return false;
			}
			unicodebuf[unicodeIndex++] = unicode;

			ptr += len;
		};

		if(unicodeIndex > 0)
		{
			Int32* unicodestr = new Int32[unicodeIndex+1];
			memcpy(unicodestr, unicodebuf, sizeof(Int32)*unicodeIndex);
			unicodestr[unicodeIndex] = 0;

			m_Words.insert(std::make_pair(unicodestr[0], unicodestr));
		}
	}

	stream.close();

	return true;
}

bool ForbitWordParser::InitValidCoolName(const std::string file)
{
	std::ifstream stream(file.c_str(), std::ios_base::in);
	if (!stream) return false;

	char buf[10];
	int bufLen = sizeof(buf);
	memset(buf, 0, bufLen);
	Int32 unicodebuf[1];
	memset(unicodebuf, 0, sizeof(unicodebuf));

	while (stream)
	{
		stream.getline(buf, bufLen - 1);
		buf[bufLen - 1] = 0;

		char* ptr = buf;
		Int32 unicodeIndex = 0;
		while (*ptr != 0 && *ptr != 13)
		{
			int len = GetUtf8Length(*ptr);
			for (int i = 0; i < len; ++i)
			{
				if (*(ptr + i) == 0)
				{
					stream.close();
					Final();
					return false;
				}
			}

			Int32 unicode = GetUtf8Unicode(ptr, len);
			if (IsEnglishLowerUnicode(unicode))
				unicode = towupper(unicode);

			if (unicode < 0)
			{
				stream.close();
				Final();
				return false;
			}
			unicodebuf[unicodeIndex++] = unicode;

			ptr += len;
		};

		if (unicodeIndex > 0)
		{
			COOL_NAME_CHARS.insert(unicodebuf[0]);
		}
	}

	stream.close();

	return true;
}

void ForbitWordParser::Final()
{
	for(WordMap::iterator iter = m_Words.begin();
		iter != m_Words.end(); ++iter)
	{
		delete []iter->second;
	}
	m_Words.clear();
}

struct ForbitWord
{
	ForbitWord(char* _cstr, Int32* str)
		:cstr(_cstr),unicodestr(str),index(0){}
public:
	bool Check(Int32 unicode){ return unicodestr[index++] == unicode; }

	bool IsMatched() const{ return unicodestr[index] == 0; }

	char*  cstr;
	Int32* unicodestr;
	Int32 index;
};

Int32 ForbitWordParser::Parse(char* utf8str)
{
	Int32 ret = FORBIT_WORD_UNCHANGED;

	if(utf8str == NULL) return FORBIT_WORD_NOUTF8;
	if(*utf8str == 0) return ret;

	char* srcptr = utf8str;
	char* destptr = utf8str;	//直接转化源字符串

	std::list<ForbitWord> checkList;
	char* checkptr = utf8str;

	while(*srcptr != 0)
	{
		int len = GetUtf8Length(*srcptr);
		for(int i = 0; i < len; ++i)
		{
			if(*(srcptr + i) == 0) return FORBIT_WORD_NOUTF8;
		}

		Int32 unicode = GetUtf8Unicode(srcptr, len);
		if (IsEnglishLowerUnicode(unicode))
			unicode = towupper(unicode);

		if(unicode < 0) return FORBIT_WORD_NOUTF8;

		if(len > 1 && !IsChineseChar(unicode) && !IsOtherChatChar(unicode) && !IsCoolNameChar(unicode))	//单个字符是否禁用
		{
			checkList.clear();
			while(checkptr != srcptr)
			{
				*destptr = *checkptr;
				++destptr;
				++checkptr;
			}
			*destptr++ = FORBIT_WORD_CHAR;
			ret = FORBIT_WORD_REPLACED;
			srcptr += len;
			checkptr = srcptr;
		}
		else
		{
			//加入新匹配项
			WordMap::iterator beginIter = m_Words.lower_bound(unicode);
			WordMap::iterator endIter = m_Words.upper_bound(unicode);
			for(;beginIter != endIter; ++beginIter)
			{
				checkList.push_back(ForbitWord(srcptr, beginIter->second));
			}

			bool bMatched = false;
			//先检查是否有匹配的
			for(std::list<ForbitWord>::iterator iter = checkList.begin();
				iter != checkList.end();)
			{
				//不匹配
				if(!iter->Check(unicode))
				{
					iter = checkList.erase(iter);
					continue;
				}
			
				//完全匹配
				if(iter->IsMatched())
				{
					Int32* ptr = iter->unicodestr;
					while(checkptr != iter->cstr)
					{
						*destptr = *checkptr;
						++destptr;
						++checkptr;
					}
					while(*ptr++ != 0) *destptr++ = FORBIT_WORD_CHAR;
					ret = FORBIT_WORD_REPLACED;
					bMatched = true;
					break;
				}

				++iter;
			}
			
			srcptr += len;
			if(bMatched)	//完全匹配
			{
				checkList.clear();
				checkptr = srcptr;
			}
			else if(checkList.empty() || *srcptr == 0)
			{
				while(checkptr != srcptr)
				{
					*destptr = *checkptr;
					++destptr;
					++checkptr;
				}
			}
		}
	}

	*destptr = 0;

	return ret;
}

