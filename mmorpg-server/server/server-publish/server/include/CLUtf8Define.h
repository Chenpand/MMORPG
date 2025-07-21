#ifndef _CL_UTF8DEFINE_H_
#define _CL_UTF8DEFINE_H_

#include <CLGameDefine.h>

/**
 *@brief 判断是否为字母
 */
inline bool IsEnglishChar(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); 
}

/**
 *@brief 判断是否为数字
 */
inline bool IsNumber(char c)
{
	return (c >= '0' && c <= '9');
}

/**
 *@brief 获取unicode值
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
 *@brief 判断是否为中文
 */
inline bool IsChineseChar(int unicode)
{
	return ( (unicode >= 0x4E00 && unicode <= 0x9FCF)
		|| (unicode >= 0x3400 && unicode <= 0x4DB5) );
}

extern std::set<Int32> COOL_NAME_CHARS;
/**
 *@brief 判断是否其他名字个性字符                                                                     
 */
inline bool IsCoolNameChar(int unicode)
{
	return COOL_NAME_CHARS.find(unicode) != COOL_NAME_CHARS.end();
}

/**
 *@brief 除 单字符，中文外的其他聊天字符
 */
inline bool IsOtherChatChar(int unicode)
{
	return ((unicode >= 0xFF01 && unicode <= 0xFF0F)
		|| (unicode >= 0xFF1A && unicode <= 0xFF20)
		|| (unicode >= 0xFF3B && unicode <= 0xFF40)
		|| (unicode >= 0xFF5B && unicode <= 0xFF5E)
		|| (unicode >= 0x3001 && unicode <= 0x300F)
		|| (unicode >= 0x2010 && unicode <= 0x201F));//全角标点
}

/**
*@brief 判断是否小写英文
*/
inline bool IsEnglishLowerUnicode(int unicode)
{
	if (unicode >= 97 && unicode <= 122)
		return true;
	else
		return false;
}

/**
 *@brief 根据首地址获取utf8字符长度
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
 *@brief 获取显示宽度 单字节字符为1 其它为2
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

		//单字节显示宽度为1 其他为2
		if(len == 1) width += 1;
		else width += 2;

		while(--len > 0) if(*ustr++ == 0) return -1;	//跳过后续字符
	}
	return width;
}

/**
 *@brief 判断名字合法性
		 maxWidth代表宽度，一个中文字符2个宽度
 */
inline bool CheckNameValid(const char* str, UInt32 maxWidth = NAME_SHOWWIDTH, bool canUseCoolname = true)
{
	if(str == 0) return false;
	UInt32 namelen = strlen(str);
	if (namelen == 0 || namelen >= 2 * maxWidth) return false;
	
	UInt32 width = 0;
	while(*str != 0)
	{
		if(IsEnglishChar(*str) || IsNumber(*str))	//英文或数字
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

			if (!IsChineseChar(unicode) && (!canUseCoolname || !IsCoolNameChar(unicode))) return false;	//中文
			
			str += len;
			width += 2;
		}
	}

	if (width > maxWidth) return false;

	return true;
}

/**
 *@brief 名字分割符
 */
#define NAME_ZONE_SPLITTER '.'

#endif
