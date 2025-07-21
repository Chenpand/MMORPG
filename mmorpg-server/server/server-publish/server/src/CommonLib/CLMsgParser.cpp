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
	
	if(type == 0 || type == 1) //�ձ�ǩ
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
		case MSGTAG_LEFT_SEPARATOR:	//��ǩ��ʼ
			{
				char* tmpStr = avalon_strdup(ptr);
				*ptr++ = 0;
				ptr = ParseTag(ptr);

				//�������ɹ����Ƿ���ԭ�ַ���
				if (ptr == NULL)
				{
					MsgTag* tag = new MsgTag();
					m_Tags[m_TagNum++] = tag;
					tag->type = 1;	//��ת������ǽ���ʧ�ܵ��ַ���
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
		default:	//�ձ�ǩ
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
	//�Ƿ�Ϸ���ǩ
	if (!IsValidTag(*str)) {
		return NULL;
	}

	MsgTag* tag = new MsgTag();
	m_Tags[m_TagNum++] = tag;
	tag->type = *str++;

	//��������
	str = ParseParams(tag, str);
	if (str == NULL) {
		// �������ɹ� �ع�tag
		UInt32 curTagNum = --m_TagNum;
		CL_SAFE_DELETE(m_Tags[curTagNum]);
		m_Tags[curTagNum] = NULL;
		return NULL;
	}

	
	switch (*str)
	{
	case MSGTAG_CONTENT_SEPARATOR:	//��ǩ���ݿ�ʼ
	{
		tag->content = ++str;

		while (!(*str == MSGTAG_RIGHT_SEPARATOR || *str == 0)) ++str;
		if ((*str) == 0) {
			// �������ɹ� �ع�tag
			UInt32 curTagNum = --m_TagNum;
			CL_SAFE_DELETE(m_Tags[curTagNum]);
			m_Tags[curTagNum] = NULL;
			return NULL;
		}

		*str = 0; //content 0��β

		++str;
	}
		break;
	case MSGTAG_RIGHT_SEPARATOR:	//û������
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
		while(*str == MSGTAG_PARAM_SEPARATOR) ++str;	//�����հ׷�

		if(*str == 0) return NULL;	//������ֹ��
		if(*str == MSGTAG_CONTENT_SEPARATOR) return str;	//��ǩ���ݿ�ʼ
		if(*str == MSGTAG_RIGHT_SEPARATOR) return str;		//��ǩ�ҷָ���
		if(tag->paramNum == MAX_TAGPARAM_NUM) return NULL;

		//��������
		char* param = tag->params[tag->paramNum++];
		char* end = param + MAX_TAGPARAM_LENGTH - 1;
		while(!IS_PARAMEND(*str))
		{
			//����������󳤶�����
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

