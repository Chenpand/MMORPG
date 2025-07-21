#ifndef __SERVER_DIRTY_WORDS_FILTER_H__
#define __SERVER_DIRTY_WORDS_FILTER_H__

#include "Common/common_base.h"

namespace KingNet { namespace Server {

//��ʿ�
enum enmDirtyWordsLibConfig
{
	max_dirty_word_length = 64,		//ÿ����ʵ���󳤶�
	max_dirty_words_count = 4096,	//��ʸ��������ֵ

	max_input_string = 4096,		//��������string
	
	filter_level_low	= 1,		//���˵ȼ���3Ϊ��ߣ���ʾ���е����дʶ��ù��ˣ�1Ϊ��͵ȼ���ֻ���˵ȼ�Ϊ1�����д�
	filter_level_mid	= 2,
	filter_level_high	= 3,
};

typedef char TDirty[max_dirty_word_length];		//�������

struct stDirtyWordsConifg
{
	TDirty m_szWord;
	int8_t m_cLevel;	//���дʵȼ���1Ϊ��ߣ���ʾ���жȵȼ����
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

	//���ˣ��������true����˵������ʣ����false����û�����
	bool  filter(char* haystack, int8_t level=filter_level_high);
	//���ˣ��������ֵ��ΪNULL���򷵻ص��ǵ�һ��ƥ���λ�ã�needle_index��Ϊƥ���е�����������е��±ꣻ���ΪNULL����û�����
	char* filter(char* haystack, int32_t& needle_index, int8_t level=filter_level_high);
	//ͨ��needle_index��ȡƥ�䴮
	char* get_needle(int32_t needle_index);

	void clear();
protected:


private:
	//������ÿ�����ʵ�����
	int32_t m_iCount;
	//�������
	stDirtyWordsConifg m_stConifg[max_dirty_words_count];
};

}}

#endif //__SERVER_DIRTY_WORDS_FILTER_H__
