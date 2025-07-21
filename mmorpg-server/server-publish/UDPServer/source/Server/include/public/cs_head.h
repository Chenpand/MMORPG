#ifndef __CSHEAD_MESSAGE_H__
#define __CSHEAD_MESSAGE_H__

#include "Common/common_namespace_definition.h"
#include "gameplatform_types.h"
#include "gameplatform_message.h"
#include <stdio.h>

//SERVER_BEGIN
namespace KingNet { namespace Server {

//deleted by aprilliu,2008-11-18, ʹ��MESSAGE_HEAD_SIZE����,��ʾCMessageHead�ĳ���
//#define  CS_MESSAGE_HEAD_SIZE	(3*sizeof(int32_t) + sizeof(int16_t) + 3*sizeof(int8_t))


	class  CCSHead
	{
	public:
		CCSHead();
		~CCSHead();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength, int16_t *pHeadLen/*=NULL*/ );
		int32_t size();
		int32_t dump(FILE* fp);
		int32_t dump();

	public:
		struct stCSPackageHead
		{
			int32_t nPackageLength;							//ͷ������ + Body����
			int32_t nUID;
			int16_t shFlag;
			int16_t nOptionalLen;							//signature ���ܴ�
			char lpbyOptional[MAX_CS_HEAD_OPTION_LENGTH];	
			int16_t nHeaderLen;
			int16_t shMessageID;
			int16_t shMessageType;
			int16_t shVersionType;             //�汾����
			int16_t shVersion;
			int32_t nPlayerID;

			int32_t nSequence;	//sequence ʹ�ü��ܴ���
		};

		stCSPackageHead m_stHead;
	private:
	};

	
}}//namespace KingNet { namespace Server {

#endif // 
