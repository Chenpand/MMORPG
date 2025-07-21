

#include "Common/common_base.h"
#include "Common/common_string.h"
#include "Common/common_codeengine.h"
#include "Common/common_object.h"

#include "public/server_public_define.h"

#include "public/cs_head.h"


//SERVER_BEGIN
namespace KingNet { namespace Server {



	CCSHead::CCSHead()
	{
		memset(&m_stHead, 0, sizeof(m_stHead));
		m_stHead.nPlayerID = invalid_object_id;
	}

	CCSHead::~CCSHead()
	{

	}

	int32_t CCSHead::size()
	{
		return 3*sizeof(int32_t) + 7*sizeof(int16_t) + m_stHead.nOptionalLen + sizeof(int32_t);	
	}

	int32_t CCSHead::encode(char *pszOut, int32_t& iOutLength)
	{

		if(NULL == pszOut)
		{
			return fail;
		}
		char *ptmp = pszOut;
		int32_t coded_length = 0;
		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp,m_stHead.nPackageLength);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp,m_stHead.nUID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,m_stHead.shFlag);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,m_stHead.nOptionalLen);
		iOutLength += coded_length;

		if (m_stHead.nOptionalLen > 0 && m_stHead.nOptionalLen <= MAX_CS_HEAD_OPTION_LENGTH)
		{
			coded_length = CCodeEngine::encode_memory(&ptmp,m_stHead.lpbyOptional, m_stHead.nOptionalLen);
			iOutLength += coded_length;
		}

		coded_length = CCodeEngine::encode_int16(&ptmp,m_stHead.nHeaderLen);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,m_stHead.shMessageID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,m_stHead.shMessageType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,m_stHead.shVersionType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,m_stHead.shVersion);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp,m_stHead.nPlayerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp,m_stHead.nSequence);
		iOutLength += coded_length;		

		return success;
	}

	int32_t CCSHead::decode(const char *pszIn, const int32_t iInLength, int16_t *pHeadLen/*=NULL*/ )
	{

		if(NULL == pszIn || iInLength < 4 )
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stHead.nPackageLength));
		remained_length -= decoded_length;	
		decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stHead.nUID));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stHead.shFlag));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stHead.nOptionalLen));
		remained_length -= decoded_length;
		
		if (m_stHead.nOptionalLen > 0 && m_stHead.nOptionalLen <= MAX_CS_HEAD_OPTION_LENGTH)
		{
			decoded_length = CCodeEngine::decode_memory(&ptmp,m_stHead.lpbyOptional, m_stHead.nOptionalLen);
			remained_length -= decoded_length;
		}
		else if(m_stHead.nOptionalLen < 0 && m_stHead.nOptionalLen > MAX_CS_HEAD_OPTION_LENGTH)
		{
			return fail;
		}
		
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stHead.nHeaderLen));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stHead.shMessageID));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stHead.shMessageType));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stHead.shVersionType));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)(&m_stHead.shVersion));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stHead.nPlayerID));
		remained_length -= decoded_length;
		decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)(&m_stHead.nSequence));
		remained_length -= decoded_length;

		if(remained_length<0)
		{
			return fail;
		}

		if(pHeadLen != NULL)
		{
			*pHeadLen = iInLength - remained_length;
		}

		return success;
	}

	int32_t CCSHead::dump()
	{

		return success;
	}

	int32_t CCSHead::dump(FILE *fp)
	{

		return success;
	}

	//SERVER_END //
}}//namespace KingNet { namespace Server {
