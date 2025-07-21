#include "Common/common_base.h"
#include "Common/common_string.h"

SERVER_BEGIN

//�޳��ַ���ǰ��Ŀ��ַ�������: '', '\t', '\n'��
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

//�޳��ַ���ǰ����ַ�������: '', '\t', '\n'�ȣ�����ת��ASCII�ַ�ΪСд
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

		//�ȴ���ȫ��ת���
		if ( ((uint8_t)cTmp == 0xe3) && (iTempLength - i > 2) && 
			((uint8_t)pszString[i+1] == 0x80) &&  ((uint8_t)pszString[i+2] == 0x80) )			//����ȫ�ǵĿո�� 0xe3 0x80 0x80
		{
			i = i+2;	//�������������ַ�
			continue;
		}
		else if ( ((uint8_t)cTmp == 0xef) && (iTempLength - i > 2) )	//��������ȫ���ַ�
		{
			uint16_t sTmpVal = (uint8_t)pszString[i+1];
			sTmpVal = (sTmpVal << 8) + (uint8_t)pszString[i+2] ;

			if (sTmpVal >= 0xbc81 && sTmpVal <= 0xbd9e)		//ȫ���ַ���Ӧ�ķ�Χ
			{
				cTmp = 0x20 + ( (sTmpVal & 0x0100) >> 2 ) + (sTmpVal & 0x3f);	//�任Ϊ����ַ�
				i = i+2;	//�������������ַ�
			}
		}

		//����ո�������ַ�
		if ((cTmp == ' ') || (cTmp == '\t') || (cTmp == '\n') || (cTmp == '\r'))	//���˿ո�
		{
			continue;
		}
		else if(cTmp >= 0x41 && cTmp <= 0x5A)
		{
			pszString[iNewIdx++] = cTmp + 0x20;	//����д�ַ�ת����Сд�ַ�
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



