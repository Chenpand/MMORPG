/**
@file   cs_pay_message.cpp
@brief  fast-cgi和market server通讯消息的定义

@author sjij@mail.51.com
@date Begin 2008.3
@date Dnd 2008.3
*/

#include "Common/common_base.h"
#include "Common/common_string.h"
#include "Common/common_codeengine.h"
#include "public/cs_pay_message.h"
#include "public/server_message.h"


namespace KingNet { namespace Server { 

	//充值请求协议
	CCSRequestPayToGame::CCSRequestPayToGame()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_iDstUin = 0;
		m_szDstAccount[0] = '\0';

		m_szOrderID[0] = '\0';
	}

	CCSRequestPayToGame::~CCSRequestPayToGame()
	{

	}

	CCSRequestPayToGame& CCSRequestPayToGame::operator =(const CCSRequestPayToGame& original)
	{
		if (this == &original)
		{
			return *this;
		}

		m_iSrcUin = original.m_iSrcUin;
		m_iDstUin = original.m_iDstUin;
		STRNCPY(m_szSrcAccount, original.m_szSrcAccount, sizeof(m_szSrcAccount));
		STRNCPY(m_szDstAccount, original.m_szDstAccount, sizeof(m_szDstAccount));
		
		STRNCPY(m_szOrderID, original.m_szOrderID, sizeof(m_szOrderID));
		
		m_iChannel = original.m_iChannel;
		m_iAmount = original.m_iAmount;
		m_iPayamount = original.m_iPayamount;
		m_iRequestTimestamp = original.m_iRequestTimestamp;
		
		return *this;
	}

	int32_t CCSRequestPayToGame::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iChannel);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iAmount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iPayamount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTimestamp);
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSRequestPayToGame::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChannel);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iAmount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayamount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRequestTimestamp);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSRequestPayToGame::dump()
	{
		return;
	}
	

	//支付响应协议
	CCSResponsePayToGame::CCSResponsePayToGame()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_iDstUin = 0;
		m_szDstAccount[0] = '\0';

		m_szOrderID[0] = '\0';

		m_szGameSequenceID[0] = '\0';
	}

	CCSResponsePayToGame::~CCSResponsePayToGame()
	{

	}

	int32_t CCSResponsePayToGame::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iChannel);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iAmount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iPayamount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iMoney);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTimestamp);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGameSequenceID, (const int16_t)sizeof(m_szGameSequenceID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iResponseTimestamp);
		iOutLength += coded_length;
		
		return success;
	}

	int32_t CCSResponsePayToGame::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChannel);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iAmount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayamount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoney);
		remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRequestTimestamp);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGameSequenceID, (const int16_t)sizeof(m_szGameSequenceID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iResponseTimestamp);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponsePayToGame::dump()
	{
		printf("m_shResultID:%d \n", m_shResultID);
		printf("m_iSrcUin:%d \n", m_iSrcUin);

		printf("m_szSrcAccount:%s \n", m_szSrcAccount);
		printf("m_iDstUin:%d \n", m_iDstUin);
		printf("m_szDstAccount:%s \n", m_szDstAccount);

		printf("m_szOrderID:%s \n", m_szOrderID);

		printf("m_iChannel:%d \n", m_iChannel);
		printf("m_iAmount:%d \n", m_iAmount);
		printf("m_iPayamount:%d \n", m_iPayamount);
		printf("m_iRequestTimestamp:%d \n", m_iRequestTimestamp);

		printf("m_szGameSequenceID:%s \n", m_szGameSequenceID);

		printf("m_iResponseTimestamp:%d \n", m_iResponseTimestamp);
		
		return;
	}

	
	//兑换请求协议
	CCSRequestPayExchange::CCSRequestPayExchange()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_iDstUin = 0;
		m_szDstAccount[0] = '\0';

		m_szOrderID[0] = '\0';
	}

	CCSRequestPayExchange::~CCSRequestPayExchange()
	{

	}

	CCSRequestPayExchange& CCSRequestPayExchange::operator =(const CCSRequestPayExchange& original)
	{
		if (this == &original)
		{
			return *this;
		}

		m_iSrcUin = original.m_iSrcUin;
		m_iDstUin = original.m_iDstUin;
		STRNCPY(m_szSrcAccount, original.m_szSrcAccount, sizeof(m_szSrcAccount));
		STRNCPY(m_szDstAccount, original.m_szDstAccount, sizeof(m_szDstAccount));

		STRNCPY(m_szOrderID, original.m_szOrderID, sizeof(m_szOrderID));

		m_iExcid = original.m_iExcid;
		m_iPayamount = original.m_iPayamount;
		m_iRequestTimestamp = original.m_iRequestTimestamp;
		return *this;
	}

	int32_t CCSRequestPayExchange::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iExcid);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iPayamount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTimestamp);
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSRequestPayExchange::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iExcid);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayamount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRequestTimestamp);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSRequestPayExchange::dump()
	{
		return;
	}


	//支付响应协议
	CCSResponsePayExchange::CCSResponsePayExchange()
	{
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';

		m_iDstUin = 0;
		m_szDstAccount[0] = '\0';

		m_szOrderID[0] = '\0';

		m_szGameSequenceID[0] = '\0';
	}

	CCSResponsePayExchange::~CCSResponsePayExchange()
	{

	}

	int32_t CCSResponsePayExchange::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iExcid);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iPayamount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTimestamp);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szGameSequenceID, (const int16_t)sizeof(m_szGameSequenceID));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iResponseTimestamp);
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSResponsePayExchange::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, (const int16_t)sizeof(m_szDstAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, (const int16_t)sizeof(m_szOrderID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iExcid);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayamount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRequestTimestamp);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szGameSequenceID, (const int16_t)sizeof(m_szGameSequenceID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iResponseTimestamp);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponsePayExchange::dump()
	{
		return;
	}


	//查询用户游戏币
	CCSRequestSearchGameMoney::CCSRequestSearchGameMoney()
	{
		m_iSrcUin = 0;		
	}

	CCSRequestSearchGameMoney::~CCSRequestSearchGameMoney()
	{

	}

	CCSRequestSearchGameMoney& CCSRequestSearchGameMoney::operator =(const CCSRequestSearchGameMoney& original)
	{
		if (this == &original)
		{
			return *this;
		}

		m_iSrcUin = original.m_iSrcUin;		
		return *this;
	}

	int32_t CCSRequestSearchGameMoney::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;
		return success;
	}

	int32_t CCSRequestSearchGameMoney::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSRequestSearchGameMoney::dump()
	{
		return;
	}


	//查询相应
	CCSResponseSearchGameMoney::CCSResponseSearchGameMoney()
	{
		m_iSrcUin = 0;
		m_iCoin = 0;
	}

	CCSResponseSearchGameMoney::~CCSResponseSearchGameMoney()
	{

	}

	int32_t CCSResponseSearchGameMoney::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iCoin);
		iOutLength += coded_length;

		return success;
	}

	int32_t CCSResponseSearchGameMoney::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCoin);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponseSearchGameMoney::dump()
	{
		return;
	}

	//查询用户是否为VIP
	CCSRequestSearchVipInfo::CCSRequestSearchVipInfo()
	{
		m_iSrcUin = 0;
		//m_szSrcAccount[0] = '\0';
	}

	CCSRequestSearchVipInfo::~CCSRequestSearchVipInfo()
	{

	}

	int32_t CCSRequestSearchVipInfo::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}
		char *ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		//coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		//iOutLength += coded_length;

		return success;
	}

	int32_t CCSRequestSearchVipInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decoded_length;

		//decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		//remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}
		return success;
	}

	void CCSRequestSearchVipInfo::dump()
	{
		return;
	}

	//查询VIP信息回复
	CCSResponseSearchVipInfo::CCSResponseSearchVipInfo()
	{
		m_iSrcUin = 0;
		//m_szSrcAccount[0] = '\0';
		m_szVipType[0] = '\0';

		m_iEndTime = 0;
	}
	CCSResponseSearchVipInfo::~CCSResponseSearchVipInfo()
	{

	}

	int32_t CCSResponseSearchVipInfo::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}
		char *ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += coded_length;

		//coded_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		//iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szVipType, sizeof(m_szVipType));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iEndTime);
		iOutLength += coded_length;

		return success;
	}



	int32_t CCSResponseSearchVipInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char *ptmp = const_cast<char*>(pszIn);
		int16_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decoded_length;

		//decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, (const int16_t)sizeof(m_szSrcAccount));
		//remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_shResultID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szVipType, sizeof(m_szVipType));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iEndTime);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CCSResponseSearchVipInfo::dump()
	{
		return;
	}

	//给用户加蓝钻请求
	CCSRequestPayToVip::CCSRequestPayToVip()
	{
		m_szOrderID[0] = '\0';
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';
		m_iRequestTime = 0;
		m_iOperUin = 0;
		m_szOperAccount[0] = '\0';
		m_iAmount = 0;
		m_iChannelID = -1;
		m_iComboID = -1;
		m_iGoodsID = -1;
		m_iAddTime = 0;
		m_iVipType = -1;
		m_iAddVipInt = -1;
		m_szAddVipStr[0] = '\0';

	}

	CCSRequestPayToVip::~CCSRequestPayToVip()
	{

	}

	int32_t CCSRequestPayToVip::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t encode_length = 0;
		
		iOutLength = 0;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTime);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iOperUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOperAccount, sizeof(m_szOperAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iAmount);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iChannelID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iComboID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iGoodsID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iAddTime);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iVipType);
		iOutLength += encode_length;

		if (-1 != m_iAddVipInt)
		{
			encode_length = CCodeEngine::encode_int32(&ptmp, m_iAddVipInt);
			iOutLength += encode_length;
		}
		if (0 < strlen(m_szAddVipStr))
		{
			encode_length = CCodeEngine::encode_string(&ptmp, m_szAddVipStr, sizeof(m_szAddVipStr));
			iOutLength += encode_length;
		}

		return success;
	}

	int32_t CCSRequestPayToVip::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}

		char *ptmp = const_cast<char*>(pszIn);
		int16_t remained_length = iInLength;
		int32_t decoded_length = 0;
		
		decoded_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iRequestTime);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iOperUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szOperAccount, sizeof(m_szOperAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iAmount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iChannelID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iComboID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iGoodsID);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iAddTime);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iVipType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iAddVipInt);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAddVipStr, sizeof(m_szAddVipStr));
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}
		return success;
	}

	void CCSRequestPayToVip::dump()
	{
		return;
	}

	//充值vip回复
	CCSResponsePayToVip::CCSResponsePayToVip()
	{
		m_szOrderID[0] = '\0';
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';
		m_iOperUin = 0;
		m_szOperAccount[0] = '\0';
		m_shResultID = -1;
		m_szTransTag[0] = '\0';
		m_shActResultID = -1;
		m_iVipEndTime = 0;
	}
	CCSResponsePayToVip::~CCSResponsePayToVip()
	{

	}

	int32_t CCSResponsePayToVip::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t encode_length = 0;
		
		iOutLength = 0;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iOperUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOperAccount, sizeof(m_szOperAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag, sizeof(m_szTransTag));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, m_shActResultID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iVipEndTime);
		iOutLength += encode_length;

		return success;
	}

	int32_t CCSResponsePayToVip::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char * ptmp = const_cast<char *>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iOperUin);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szOperAccount, sizeof(m_szOperAccount));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_shResultID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag, sizeof(m_szTransTag));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_shActResultID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iVipEndTime);
		remained_length -= decode_length;

		if (remained_length < 0)
		{
			return fail;
		}
		return success;

	}

	void CCSResponsePayToVip::dump()
	{
		return;
	}

	//手机包月vip请求
	CCSRequestPayMobileToVip::CCSRequestPayMobileToVip()
	{
		m_szOrderID[0] = '\0';
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';
		m_iRequestTime = 0;
		m_iChannelID = -1;
		m_iComboID = -1;
		m_iGoodsID = -1;
		m_szMobile[0] = '\0';
		m_iAddVipInt = -1;
		m_szAddVipStr[0] = '\0';

	}
	CCSRequestPayMobileToVip::~CCSRequestPayMobileToVip()
	{

	}

	void CCSRequestPayMobileToVip::dump()
	{

	}

	int32_t CCSRequestPayMobileToVip::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}
		char *ptmp = pszOut;
		int32_t encode_length = 0;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTime);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iChannelID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iComboID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iGoodsID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iAddVipInt);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szAddVipStr, sizeof(m_szAddVipStr));
		iOutLength += encode_length;

		return success;
	}

	int32_t CCSRequestPayMobileToVip::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char* ptmp = const_cast<char *> (pszIn);
		int32_t remained_length = iInLength;

		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iRequestTime);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iChannelID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iComboID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iGoodsID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szMobile, sizeof(m_szMobile));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iAddVipInt);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szAddVipStr, sizeof(m_szAddVipStr));
		remained_length -= decode_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	/************************************************************************/
	/* 手机包月蓝钻回复                                                     */
	/************************************************************************/
	CCSResponsePayMobileToVip::CCSResponsePayMobileToVip()
	{
		m_szOrderID[0] = '\0';
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';
		m_shResultID = -1;
		m_szTransTag[0] = '\0';
		//m_iGoodsID = 0;
		m_shActResultID = -1;
	}

	CCSResponsePayMobileToVip::~CCSResponsePayMobileToVip()
	{

	}

	void CCSResponsePayMobileToVip::dump()
	{
		return ;
	}

	int32_t CCSResponsePayMobileToVip::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}
		char *ptmp = pszOut;
		int32_t encode_length = 0;

		iOutLength = 0;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag, sizeof(m_szTransTag));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, m_shActResultID);
		iOutLength += encode_length;

		return success;
	}

	int32_t CCSResponsePayMobileToVip::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}
		char *ptmp = const_cast<char *>(pszIn);
		int32_t remained_length = iInLength;

		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_shResultID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag, sizeof(m_szTransTag));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_shActResultID);
		remained_length -= decode_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	//手机取消包月vip请求
	CCSRequestPayUnsubscriptVip::CCSRequestPayUnsubscriptVip()
	{
		m_szOrderID[0] = '\0';
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';
		m_iRequestTime = 0;
		m_szMobile[0] = '\0';
	}

	CCSRequestPayUnsubscriptVip::~CCSRequestPayUnsubscriptVip()
	{

	}

	void CCSRequestPayUnsubscriptVip::dump()
	{
		return ;
	}

	int32_t CCSRequestPayUnsubscriptVip::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char *ptmp = pszOut;
		int32_t encode_length = 0;
		iOutLength = 0;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iRequestTime);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szMobile, sizeof(m_szMobile));
		iOutLength += encode_length;

		return success;
	}

	int32_t CCSRequestPayUnsubscriptVip::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char *>(pszIn);
		int32_t decode_length = 0;
		int32_t remained_length = iInLength;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iRequestTime);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szMobile, sizeof(m_szMobile));
		remained_length -= decode_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	//手机退订vip回复
	CCSResponsePayUnsubscriptVip::CCSResponsePayUnsubscriptVip()
	{
		m_szOrderID[0] = '\0';
		m_iSrcUin = 0;
		m_szSrcAccount[0] = '\0';
		m_shResultID = -1;
		m_szTransTag[0] = '\0';
	}

	CCSResponsePayUnsubscriptVip::~CCSResponsePayUnsubscriptVip()
	{

	}

	void CCSResponsePayUnsubscriptVip::dump()
	{
		return;
	}

	int32_t CCSResponsePayUnsubscriptVip::encode(char* pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char * ptmp = pszOut;
		int32_t encode_length = 0;
		iOutLength = 0;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_iSrcUin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, m_shResultID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag, sizeof(m_szTransTag));
		iOutLength += encode_length;

		return success;
	}

	int32_t CCSResponsePayUnsubscriptVip::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength < 0)
		{
			return fail;
		}

		char *ptmp = const_cast<char *> (pszIn);
		int32_t remained_length = iInLength;

		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID, sizeof(m_szOrderID));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_iSrcUin);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szSrcAccount, sizeof(m_szSrcAccount));
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_shResultID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag, sizeof(m_szTransTag));
		remained_length -= decode_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}
}}//namespace KingNet { namespace Server {


