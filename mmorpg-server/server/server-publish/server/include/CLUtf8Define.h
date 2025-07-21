#ifndef _CL_UTF8DEFINE_H_
#define _CL_UTF8DEFINE_H_

#include <CLGameDefine.h>

/**
 *@brief �ж��Ƿ�Ϊ��ĸ
 */
inline bool IsEnglishChar(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); 
}

/**
 *@brief �ж��Ƿ�Ϊ����
 */
inline bool IsNumber(char c)
{
	return (c >= '0' && c <= '9');
}

/**
 *@brief ��ȡunicodeֵ
 */
inline int GetUtf8Unicode(const char* c, int len)
{
	int ret = -1;
	switch(len)
	{
	case 1: ret = c[0]; break;
	case 2: ret = int(c[0] & 0x1F) << 6 | (c[1] & 0x3F); break;
	case 3: ret = int(c[0] & 0x0F) << 12 | int(c[1] & 0x3F) << 6 | int(c[2] & 0x3F); break;
	case 4: ret = int(c[0] & 0x07) << 18 | int(c[1] & 0x3F) << 12 | int(c[2] & 0x3F) << 6 | int(c[3] & 0x3F); break;
	case 5: ret = int(c[0] & 0x03) << 24 | int(c[1] & 0x3F) << 18 | int(c[2] & 0x3F) << 12 | int(c[3] & 0x3F) << 6 | int(c[4] & 0x3F);  break;
	case 6: ret = int(c[0] & 0x01) << 30 | int(c[1] & 0x3F) << 24 | int(c[2] & 0x3F) << 18 | int(c[3] & 0x3F) << 12 | int(c[4] & 0x3F) << 6 | int(c[5] & 0x3F); break; 
	}
	return ret;
}

/**
 *@brief �ж��Ƿ�Ϊ����
 */
inline bool IsChineseChar(int unicode)
{
	return ( (unicode >= 0x4E00 && unicode <= 0x9FCF)
		|| (unicode >= 0x3400 && unicode <= 0x4DB5) );
}

extern std::set<Int32> COOL_NAME_CHARS;
/**
 *@brief �ж��Ƿ��������ָ����ַ�                                                                     
 */
inline bool IsCoolNameChar(int unicode)
{
	return COOL_NAME_CHARS.find(unicode) != COOL_NAME_CHARS.end();
}

/**
 *@brief �� ���ַ�������������������ַ�
 */
inline bool IsOtherChatChar(int unicode)
{
	return ((unicode >= 0xFF01 && unicode <= 0xFF0F)
		|| (unicode >= 0xFF1A && unicode <= 0xFF20)
		|| (unicode >= 0xFF3B && unicode <= 0xFF40)
		|| (unicode >= 0xFF5B && unicode <= 0xFF5E)
		|| (unicode >= 0x3001 && unicode <= 0x300F)
		|| (unicode >= 0x2010 && unicode <= 0x201F));//ȫ�Ǳ��
}

/**
*@brief �ж��Ƿ�СдӢ��
*/
inline bool IsEnglishLowerUnicode(int unicode)
{
	if (unicode >= 97 && unicode <= 122)
		return true;
	else
		return false;
}

/**
 *@brief �����׵�ַ��ȡutf8�ַ�����
 */
inline int GetUtf8Length(unsigned char c)
{
	if(c < 192) return 1;
	else if(c < 224) return 2;
	else if(c < 240) return 3;
	else if(c < 248) return 4;
	else if(c < 252) return 5;
	else return 6;
}

/**
 *@brief ��ȡ��ʾ��� ���ֽ��ַ�Ϊ1 ����Ϊ2
 */
inline int GetUtf8ShowWidth(const char* str)
{
	if (!str)
	{
		return -1;
	}

	const unsigned char* ustr = (const unsigned char*)str;
	int width = 0;
	while(*ustr != 0)
	{
		int len = GetUtf8Length(*ustr++);

		//���ֽ���ʾ���Ϊ1 ����Ϊ2
		if(len == 1) width += 1;
		else width += 2;

		while(--len > 0) if(*ustr++ == 0) return -1;	//���������ַ�
	}
	return width;
}

/**
 *@brief �ж����ֺϷ���
		 maxWidth�����ȣ�һ�������ַ�2�����
 */
inline bool CheckNameValid(const char* str, UInt32 maxWidth = NAME_SHOWWIDTH, bool canUseCoolname = true)
{
	if(str == 0) return false;
	UInt32 namelen = strlen(str);
	if (namelen == 0 || namelen >= 2 * maxWidth) return false;
	
	UInt32 width = 0;
	while(*str != 0)
	{
		if(IsEnglishChar(*str) || IsNumber(*str))	//Ӣ�Ļ�����
		{
			++str;
			++width;
		}
		else
		{
			int len = GetUtf8Length(*str);
			for (int i = 0; i < len; ++i)
			{
				if(*(str + i) == 0) return false;
			}
			int unicode = GetUtf8Unicode(str, len);
			if(unicode < 0) return false;

			if (!IsChineseChar(unicode) && (!canUseCoolname || !IsCoolNameChar(unicode))) return false;	//����
			
			str += len;
			width += 2;
		}
	}

	if (width > maxWidth) return false;

	return true;
}

/**
 *@brief ���ַָ��
 */
#define NAME_ZONE_SPLITTER '.'

#endif
