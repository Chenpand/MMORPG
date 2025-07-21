#include "CLMsgParser.h"


MsgTag::MsgTag()
{	
	type = MSGTAG_NONE;
	ClearParams();
	content = NULL;
}

MsgTag::~MsgTag()
{
	if (1 == type && content)
	{
		free(content);
		content = NULL;
	}
	
}

void MsgTag::ClearParams()
{
	for(unsigned int i = 0; i < MAX_TAGPARAM_NUM; ++i)
		params[i][0] = 0;
	paramNum = 0;
}

int MsgTag::Dump(char* buf, int size)
{
	if(buf == NULL || size == 0) return -1;
	
	if(type == 0 || type == 1) //空标签
	{
		if(content == NULL) return -1;

		int len = strlen(content);
		if(len > size) return -1;
		avalon_memmove(buf, content, len);
		return len;
	}
	else
	{
		if(size < 3) return -1;

		int index = 0;
		buf[index++] = MSGTAG_LEFT_SEPARATOR;
		buf[index++] = type;

		for(int i = 0; i < paramNum; ++i)
		{
			int len = strlen(params[i]);
			if(index + len + 1 > size) return -1;
			buf[index++] = MSGTAG_PARAM_SEPARATOR;
			avalon_memmove(buf + index, params[i], len);
			index += len;
		}

		if(content != NULL)
		{
			int len = strlen(content);
			if(index +  len + 1 > size) return -1;
			buf[index++] = MSGTAG_CONTENT_SEPARATOR;
			avalon_memmove(buf + index, content, len);
			index += len;
		}

		if(index + 1 > size) return -1;
		buf[index++] = MSGTAG_RIGHT_SEPARATOR;
		return index;
	}
}


MsgParser::MsgParser()
{
	m_TagNum = 0;
	memset(m_Tags, 0, sizeof(m_Tags));
	m_Buffer = NULL;
}

MsgParser::~MsgParser()
{
	Clear();
}

bool MsgParser::Parse(const char* str)
{
	if(str == NULL) return false;
	Clear();

	m_Buffer = avalon_strdup(str);

	char* ptr = m_Buffer;
	while(m_TagNum < MAX_TAG_NUM)
	{
		switch(*ptr)
		{
		case MSGTAG_LEFT_SEPARATOR:	//标签开始
			{
				char* tmpStr = avalon_strdup(ptr);
				*ptr++ = 0;
				ptr = ParseTag(ptr);

				//解析不成功还是返回原字符串
				if (ptr == NULL)
				{
					MsgTag* tag = new MsgTag();
					m_Tags[m_TagNum++] = tag;
					tag->type = 1;	//有转义符但是解析失败的字符串
					tag->content = tmpStr;

					while (!(*tmpStr == 0))
					{
						tmpStr++;
					}
					ptr = tmpStr;
				}
				else
				{
					free(tmpStr);
				}
			}
			break;
		case 0:
			return true;
		default:	//空标签
			{
				MsgTag* tag = new MsgTag();
				m_Tags[m_TagNum++] = tag;
				tag->type = 0;
				tag->content = ptr;

				while(!(*ptr == 0 || *ptr == MSGTAG_LEFT_SEPARATOR)) ++ptr;
			}
			break;
		}
	}

	return false;
}

char* MsgParser::ParseTag(char* str)
{
	//是否合法标签
	if (!IsValidTag(*str)) {
		return NULL;
	}

	MsgTag* tag = new MsgTag();
	m_Tags[m_TagNum++] = tag;
	tag->type = *str++;

	//解析参数
	str = ParseParams(tag, str);
	if (str == NULL) {
		// 解析不成功 回滚tag
		UInt32 curTagNum = --m_TagNum;
		CL_SAFE_DELETE(m_Tags[curTagNum]);
		m_Tags[curTagNum] = NULL;
		return NULL;
	}

	
	switch (*str)
	{
	case MSGTAG_CONTENT_SEPARATOR:	//标签内容开始
	{
		tag->content = ++str;

		while (!(*str == MSGTAG_RIGHT_SEPARATOR || *str == 0)) ++str;
		if ((*str) == 0) {
			// 解析不成功 回滚tag
			UInt32 curTagNum = --m_TagNum;
			CL_SAFE_DELETE(m_Tags[curTagNum]);
			m_Tags[curTagNum] = NULL;
			return NULL;
		}

		*str = 0; //content 0结尾

		++str;
	}
		break;
	case MSGTAG_RIGHT_SEPARATOR:	//没有内容
	{
		++str;
	}
		break;
	}
	
	return str;
}


#define IS_PARAMEND(Char) (Char == 0 || Char == MSGTAG_CONTENT_SEPARATOR \
	|| Char == MSGTAG_RIGHT_SEPARATOR || Char == MSGTAG_PARAM_SEPARATOR)

char* MsgParser::ParseParams(MsgTag* tag, char* str)
{
	while(tag->paramNum <= MAX_TAGPARAM_NUM)
	{
		while(*str == MSGTAG_PARAM_SEPARATOR) ++str;	//跳过空白符

		if(*str == 0) return NULL;	//意外终止符
		if(*str == MSGTAG_CONTENT_SEPARATOR) return str;	//标签内容开始
		if(*str == MSGTAG_RIGHT_SEPARATOR) return str;		//标签右分隔符
		if(tag->paramNum == MAX_TAGPARAM_NUM) return NULL;

		//拷贝参数
		char* param = tag->params[tag->paramNum++];
		char* end = param + MAX_TAGPARAM_LENGTH - 1;
		while(!IS_PARAMEND(*str))
		{
			//超出参数最大长度限制
			if(param >= end) return NULL;
			*param++ = *str++;
		}
		*param = 0;
	}

	return NULL;
}

int MsgParser::GetTagNum(int type)
{
	int num = 0;
	for(int i = 0; i < m_TagNum; ++i)
	{
		if(m_Tags[i]->type == type) ++num; 
	}
	return num;
}

bool MsgParser::Visit(TagVisitor& visitor)
{
	for(int i = 0; i < m_TagNum; ++i)
	{
		if(!visitor(m_Tags[i])) return false;
	}
	return true;
}

int MsgParser::Dump(char* buf, int size)
{
	if(buf == NULL || size == 0) return -1;
	
	int index = 0;

	for(int i = 0; i < m_TagNum; ++i)
	{
		if(index >= size) return -1;
		int len = m_Tags[i]->Dump(buf + index, size - index);
		if(len < 0) return -1;
		index+= len;
	}
	return index;
}

bool MsgParser::IsValidTag(char tag)
{
	for(size_t i = 0; i < sizeof(MSG_TAGS) / sizeof(MSG_TAGS[0]); ++i)
		if(tag == MSG_TAGS[i]) return true;
	return false;
}

void MsgParser::Clear()
{
	for(int i = 0; i < m_TagNum; ++i)
		CL_SAFE_DELETE(m_Tags[i]);
	m_TagNum = 0;

	if(m_Buffer != NULL)
	{
		free(m_Buffer);
		m_Buffer = NULL;
	}
}

