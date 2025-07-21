#include "public/game_record_server_message_body.h"

namespace KingNet { namespace Server {




	//
	CRequestRecordGameRound::CRequestRecordGameRound()
	{
		//
		m_nGameID = -1;
		m_szGameTag[0] = '\0';
		m_iGameRecordSize = 0;
	}

	CRequestRecordGameRound::~CRequestRecordGameRound()
	{
		//
	}

	int32_t CRequestRecordGameRound::encode( char *pszOut, int32_t& iOutLength )
	{
		if(NULL == pszOut)
		{
			return fail;
		}
		int32_t encode_length = 0;
		char* ptmp = pszOut;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szGameTag, (const int16_t)sizeof(m_szGameTag));
		iOutLength += encode_length;

		if (0 > m_iGameRecordSize)
		{
			return fail;
		}

		if (max_game_record_data_size < m_iGameRecordSize)
		{
			m_iGameRecordSize = max_game_record_data_size;
		}

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iGameRecordSize);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_memory(&ptmp, m_szGameRecordData, (const int32_t)m_iGameRecordSize);
		iOutLength += encode_length;

		return success;

	}

	int32_t CRequestRecordGameRound::decode( const char *pszIn, const int32_t iInLength )
	{
		if(NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		
		int32_t remain_length = iInLength;
		char* ptmp  = const_cast<char*>(pszIn);
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szGameTag, (const int16_t)sizeof(m_szGameTag));
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iGameRecordSize);
		remain_length -= decode_length;

		if (0 > m_iGameRecordSize || max_game_record_data_size < m_iGameRecordSize)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_memory(&ptmp, m_szGameRecordData, (const int32_t)sizeof(m_szGameRecordData));
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestRecordGameRound::dump()
	{
		//
	}


}}//namespace KingNet { namespace Server {
