#include "Common/common_base.h"
#include "Common/common_string.h"

SERVER_BEGIN

//剔除字符串前后的空字符，比如: '', '\t', '\n'等
void TrimString(char *pszString )
{
	char *pb = NULL;
	char *pe = NULL;
	size_t iTempLength = 0;

	if( pszString == NULL )
	{
		return;
	}

	iTempLength = strlen(pszString);
	if( iTempLength == 0 )
	{
		return;
	}

	pb = pszString;

	while (((*pb == ' ') || (*pb == '\t') || (*pb == '\n') || (*pb == '\r')) && (*pb != 0))
	{
		pb ++;
	}

	pe = &pszString[iTempLength-1];
	while ((pe >= pb) && ((*pe == ' ') || (*pe == '\t') || (*pe == '\n') || (*pe == '\r')))
	{
		pe --;
	}

	*(pe+1) = '\0';

	if (pb > pszString)
	{
		strcpy(pszString, pb);
	}

	//STRNCPY(pszString, pb, (pe-pb+2));
	 
	return;
}

//剔除字符串前后空字符，比如: '', '\t', '\n'等，并且转换ASCII字符为小写
void StringTrimAndToLowerCase(char *pszString )
{
	size_t iTempLength = 0;
	int32_t iNewIdx = 0;
	char cTmp = '\0';

	if( pszString == NULL )
	{
		return;
	}

	iTempLength = strlen(pszString);
	if( iTempLength == 0 )
	{
		return;
	}

	for(int32_t i=0; i<(int32_t)iTempLength; i++)
	{
		cTmp = pszString[i];

		//先处理全角转半角
		if ( ((uint8_t)cTmp == 0xe3) && (iTempLength - i > 2) && 
			((uint8_t)pszString[i+1] == 0x80) &&  ((uint8_t)pszString[i+2] == 0x80) )			//处理全角的空格符 0xe3 0x80 0x80
		{
			i = i+2;	//连续跳过两个字符
			continue;
		}
		else if ( ((uint8_t)cTmp == 0xef) && (iTempLength - i > 2) )	//处理其它全角字符
		{
			uint16_t sTmpVal = (uint8_t)pszString[i+1];
			sTmpVal = (sTmpVal << 8) + (uint8_t)pszString[i+2] ;

			if (sTmpVal >= 0xbc81 && sTmpVal <= 0xbd9e)		//全角字符对应的范围
			{
				cTmp = 0x20 + ( (sTmpVal & 0x0100) >> 2 ) + (sTmpVal & 0x3f);	//变换为半角字符
				i = i+2;	//连续跳过两个字符
			}
		}

		//处理空格等特殊字符
		if ((cTmp == ' ') || (cTmp == '\t') || (cTmp == '\n') || (cTmp == '\r'))	//过滤空格
		{
			continue;
		}
		else if(cTmp >= 0x41 && cTmp <= 0x5A)
		{
			pszString[iNewIdx++] = cTmp + 0x20;	//将大写字符转换成小写字符
		}
		else
		{
			pszString[iNewIdx++] = cTmp;
		}
	}

	pszString[iNewIdx] = '\0';

	return;
}

char* get_next_token(const char* src, char separator, char* out)
{
	char *pcPos = NULL;
	int iTokenLength = 0;

	if(!src)
	{
		if(out)
		{
			out[0] = '\0';
		}
		return NULL;
	}

	pcPos = strchr((char*)src, (int)separator);

	if( pcPos )
	{
		if(out)
		{
			iTokenLength = (int)(pcPos - src);
			memcpy((void *)out, (const void *)src, iTokenLength);
			out[iTokenLength] = '\0';
		}
		pcPos++;
	}
	else
	{
		if(out)
		{
			strcpy(out, src);
		}
	}

	return pcPos;
}

void InitDaemon(int8_t nochdir, int8_t noclose)
{
	daemon(nochdir, noclose);

	//ignore signals
	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
}

SERVER_END



