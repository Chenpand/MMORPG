#ifndef __SERVER_DIRTY_WORDS_FILTER_H__
#define __SERVER_DIRTY_WORDS_FILTER_H__

#include "Common/common_base.h"

namespace KingNet { namespace Server {

//脏词库
enum enmDirtyWordsLibConfig
{
	max_dirty_word_length = 64,		//每个脏词的最大长度
	max_dirty_words_count = 4096,	//脏词个数的最大值

	max_input_string = 4096,		//最大的输入string
	
	filter_level_low	= 1,		//过滤等级，3为最高，表示所有的敏感词都得过滤；1为最低等级，只过滤等级为1的敏感词
	filter_level_mid	= 2,
	filter_level_high	= 3,
};

typedef char TDirty[max_dirty_word_length];		//脏词类型

struct stDirtyWordsConifg
{
	TDirty m_szWord;
	int8_t m_cLevel;	//敏感词等级，1为最高，表示敏感度等级最高
};

class CDirtyWordsFilter
{
public:
	CDirtyWordsFilter();
	~CDirtyWordsFilter();

	int32_t load(char* file);
	int32_t reload(char* file);

	int32_t load(char* pszData, int32_t size);
	int32_t reload(char* pszData, int32_t size);

	//过滤，如果返回true，则说明有脏词；如果false，则没有脏词
	bool  filter(char* haystack, int8_t level=filter_level_high);
	//过滤，如果返回值不为NULL，则返回的是第一次匹配的位置，needle_index则为匹配中的脏词在数组中的下标；如果为NULL，则没有脏词
	char* filter(char* haystack, int32_t& needle_index, int8_t level=filter_level_high);
	//通过needle_index获取匹配串
	char* get_needle(int32_t needle_index);

	void clear();
protected:


private:
	//脏词配置库中脏词的数量
	int32_t m_iCount;
	//脏词数组
	stDirtyWordsConifg m_stConifg[max_dirty_words_count];
};

}}

#endif //__SERVER_DIRTY_WORDS_FILTER_H__
