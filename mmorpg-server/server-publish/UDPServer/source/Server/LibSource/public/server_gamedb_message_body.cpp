#include "public/server_gamedb_message_body.h"
#include "Common/common_codeengine.h"
#include "Common/common_string.h"

namespace KingNet { namespace Server {  

int32_t decode_TDBExtGameInfo(char** pszIn, TDBExtGameInfo& data);
int32_t  encode_TDBExtGameInfo(char** pszIn, TDBExtGameInfo& data);
CRequestGetGameData::CRequestGetGameData()
{
	m_iUin = 0;
	m_nGameID = -1;
	m_cRequireBaseInfo = 0;
	m_cAddRefrence = 0;
	m_szAccount[0] = '\0';
}

CRequestGetGameData::~CRequestGetGameData()
{

}

int32_t CRequestGetGameData::encode( char *pszOut, int32_t& iOutLength )
{
	if(NULL == pszOut)
	{
		return fail;
	}

	char * ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cRequireBaseInfo);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cAddRefrence);
	iOutLength += coded_length;
	return success;
}

int32_t CRequestGetGameData::decode( const char *pszIn, const int32_t iInLength )
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int16_t remained_length  = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t) sizeof(m_szAccount));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cRequireBaseInfo);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cAddRefrence);
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CRequestGetGameData::dump()
{
	
}

////
CResponseGetGameData::CResponseGetGameData()
{
	m_nResultID = result_id_success;
	m_iUin = 0;
	m_cHaveBaseInfo = 0;
	m_cIsFirstEnterGame = 0;
	memset(&m_stBaseInfo, 0, sizeof(m_stBaseInfo));
	memset(&m_stGameData, 0, sizeof(m_stGameData));
	memset(&m_stServiceData, 0, sizeof(m_stServiceData));
}

CResponseGetGameData::~CResponseGetGameData()
{
	
}

int32_t CResponseGetGameData::encode( char *pszOut, int32_t& iOutLength )
{
	//TODO: 修改，以增加版本兼容性
	if(NULL == pszOut)
	{
		return fail;
	}

	char * ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += coded_length;

	if(m_nResultID != result_id_success )
	{
		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stGameData.m_nGameID);
		iOutLength += coded_length;
		return success;	
	}

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cHaveBaseInfo);
	iOutLength += coded_length;

	if (m_cHaveBaseInfo > 0 ) //若有基本信息
	{
		coded_length = encode_player_common_info(&ptmp, m_stBaseInfo);
		iOutLength += coded_length;

	}//if (m_cHaveBaseInfo > 0 )

	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cIsFirstEnterGame);
	iOutLength += coded_length;


	//编码游戏数据
	coded_length = encode_player_db_gamedata(&ptmp, m_stGameData);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stServiceData.m_nCount);
	iOutLength += coded_length;

	for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
	{
		coded_length = encode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
		iOutLength += coded_length;
	}

	 

	//TODO .. 其他信息，比如具体游戏的特有数据

	return success;
}

int32_t CResponseGetGameData::decode( const char *pszIn, const int32_t iInLength )
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int16_t remained_length  = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decoded_length;

	if(m_nResultID != result_id_success )
	{
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stGameData.m_nGameID);
		remained_length -= decoded_length;
		return success;	
	}
	
	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cHaveBaseInfo);
	remained_length -= decoded_length;

	if (m_cHaveBaseInfo > 0)
	{
		decoded_length = decode_player_common_info(&ptmp, m_stBaseInfo);
		remained_length -= decoded_length;
	}

	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cIsFirstEnterGame);
	remained_length -= decoded_length;

	decoded_length = decode_player_db_gamedata(&ptmp, m_stGameData);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stServiceData.m_nCount);
	remained_length -= decoded_length;

	for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
	{
		decoded_length = decode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
		remained_length -= decoded_length;
	}

	 

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CResponseGetGameData::dump()
{

}

//////////////////////////////////////////////////////////////////////////

CRequestLockMoney::CRequestLockMoney()
{
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_iRoomID = invalid_object_id;
	m_iTableID = invalid_object_id;
	m_nGameID = -1;
	m_nLockType = -1;
	m_nLockMoney = 0;
	m_iACT = -1;
	m_iLockStrategy = lock_type_strategy_must;
}

CRequestLockMoney::~CRequestLockMoney()
{

}

int32_t CRequestLockMoney::encode( char *pszOut, int32_t& iOutLength )
{
/*
	int32_t m_iUin;
	char m_szAccount[max_account_string_length];
	int32_t m_iRoomID;
	int32_t m_iTableID;
	int16_t m_nGameID;
	int16_t m_nLockType;
	int32_t m_nLockMoney;
	*/
	if(NULL == pszOut)
	{
		return fail;
	}
	char * ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iRoomID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iTableID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp,m_nGameID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp,m_nLockType);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_nLockMoney);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iACT);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, m_iLockStrategy);
	iOutLength += coded_length;
	return success;
}

int32_t CRequestLockMoney::decode( const char *pszIn, const int32_t iInLength )
{
	/*
	int32_t m_iUin;
	char m_szAccount[max_account_string_length];
	int32_t m_iRoomID;
	int32_t m_iTableID;
	int16_t m_nGameID;
	int16_t m_nLockType;
	*/
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t remained_length  = iInLength;
	int32_t decoded_length = 0;
	decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iTableID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nGameID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nLockType);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nLockMoney);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iACT);
	remained_length -= decoded_length;
	
	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_iLockStrategy);
	remained_length -= decoded_length;

	if(remained_length < 0)
	{
		return fail;
	}
	return success;
}

void CRequestLockMoney::dump()
{

}

/*
int16_t m_nResultID;

	int32_t m_iUin;
	char m_szAccount[max_account_string_length];

	int16_t m_nGameID;
	int16_t m_nLockType;

	int32_t m_iRoomID;
	int32_t m_iTableID;

	int32_t m_iCurrentMoney;			//当前游戏币
	int16_t m_iLockedServerType;			//当前占有游戏币锁的服务器type
	int32_t m_iLockedServerID;			//当前占有游戏币锁的服务器id
	int32_t m_iLockedRoomID;			//当前占有游戏币锁的房间id
	int32_t m_iLockedTableID;			//当前占有游戏币锁的游戏桌id
	int32_t m_nLockMoney;				//当前占有游戏币锁的锁定金额 add by lz,2008.07.09

*/
CResponseLockMoney::CResponseLockMoney()
{
	m_nResultID = result_id_success;

	m_iUin = 0;
	m_szAccount[0] = '\0';

	m_nGameID = -1;
	m_nLockType = -1;

	m_iRoomID = invalid_object_id;
	m_iTableID = invalid_object_id;

	m_iCurrentMoney = 0;			//当前游戏币

	m_iLockedServerType = -1;
	m_iLockedServerID = -1;			//当前占有游戏币锁的服务器id
	m_iLockedRoomID = invalid_object_id;			//当前占有游戏币锁的房间id
	m_iLockedTableID = invalid_object_id;			//当前占有游戏币锁的游戏桌id
	m_nLockMoney = 0 ;
	m_iACT = -1;
	m_iLockStrategy = lock_type_strategy_must;
}

CResponseLockMoney::~CResponseLockMoney()
{

}

int32_t CResponseLockMoney::encode( char *pszOut, int32_t& iOutLength )
{
	 
	if(NULL == pszOut)
	{
		return fail;
	}
	int32_t coded_length = 0;
	char * ptmp = pszOut;

	iOutLength = 0;
	
	coded_length = CCodeEngine::encode_int16(&ptmp,m_nResultID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp,m_nGameID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp,m_nLockType);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iRoomID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iTableID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iCurrentMoney);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp,m_iLockedServerType);
	iOutLength += coded_length;
		
	coded_length = CCodeEngine::encode_int32(&ptmp,m_iLockedServerID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iLockedRoomID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp,m_iLockedTableID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_nLockMoney);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iACT);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int8(&ptmp, m_iLockStrategy);
	iOutLength += coded_length;

	return success;
}

int32_t CResponseLockMoney::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t remain_length = iInLength;
	char *ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nLockType);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iTableID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iCurrentMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iLockedServerType);
	remain_length -= decode_length;
		
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLockedServerID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLockedRoomID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLockedTableID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nLockMoney);
	remain_length -= decode_length;
		
	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iACT);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_iLockStrategy);
	remain_length -= decode_length;

	return success;
}

void CResponseLockMoney::dump()
{

}
CRequestUpdateGamePoint::CRequestUpdateGamePoint()
{
	m_iUin = 0;
	m_szAccount[0] = '\0';

	m_iSourceIP = 0;
	m_nPort = 0;

	m_nGameID = -1;
	m_iRoomID = -1;
	m_iTableID = -1;

	m_iDeltaGamePoint = 0;
	m_iDeltaWinRound = 0;
	m_iDeltaLossRound = 0;
	m_iDeltaDrawRound = 0;
	m_iDeltaEscapeRound = 0;
	m_iDeltaMoney = 0;
	m_lDeltaHappyBean = 0;

	//m_iDeltaCharming = 0;
	//m_iDeltaAchievement = 0;
	m_iDeltaServiceFare = 0;
	m_lDeltaServiceFareHappyBean = 0;

	m_iGameSeconds = 0;					//游戏耗时

	m_iACT = -1;
	m_szGameTag[0] = '\0';	//游戏局标签， 唯一表示一局游戏
	m_szGamePara[0] = '\0';	//游戏参数

	m_iRoomBillFlag = 0;		//账单类型: 普通房间账单，VIP房间账单等
	m_iRoomActiveID = -1;		//房间内活动ID。默认是0，无活动
}

CRequestUpdateGamePoint::~CRequestUpdateGamePoint()
{

}

int32_t CRequestUpdateGamePoint::encode( char *pszOut, int32_t& iOutLength )
{
	/*int32_t m_iUin;
	char m_szAccount[max_account_string_length];

	int32_t m_iSourceIP;
	int16_t m_nPort;

	int16_t m_nGameID;

	int32_t m_iDeltaGamePoint;
	int32_t m_iDeltaWinRound;
	int32_t m_iDeltaLossRound;
	int32_t m_iDeltaDrawRound;
	int32_t m_iDeltaEscapeRound;
	int32_t m_iDeltaMoney;

	int32_t m_iGameSeconds;					//游戏耗时


	char m_szGameTag[max_game_tag_length];	//游戏局标签， 唯一表示一局游戏
	char m_szGamePara[max_game_para_length];	//游戏参数

	int32_t m_iRoomBillFlag;		//账单类型: 普通房间账单，VIP房间账单等
	int32_t m_iRoomActiveID;		//房间内活动ID。默认是0，无活动
	*/
	if(NULL == pszOut)
	{
		return fail;
	}
	int32_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iSourceIP);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp,m_nPort);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp,m_nGameID);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iRoomID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iTableID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaGamePoint);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaWinRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaLossRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaDrawRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaEscapeRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int64(&ptmp,(uint64_t)m_lDeltaHappyBean);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaServiceFare);
	iOutLength += encode_length;

	 
	encode_length = CCodeEngine::encode_int64(&ptmp,(uint64_t)m_lDeltaServiceFareHappyBean);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp,m_iGameSeconds);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iACT);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szGameTag,sizeof(m_szGameTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szGamePara,sizeof(m_szGamePara));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iRoomBillFlag);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iRoomActiveID);
	iOutLength += encode_length;

	return success;
}

int32_t CRequestUpdateGamePoint::decode( const char *pszIn, const int32_t iInLength )
{
	/*int32_t m_iUin;
	char m_szAccount[max_account_string_length];

	int32_t m_iSourceIP;
	int16_t m_nPort;

	int16_t m_nGameID;

	int32_t m_iDeltaGamePoint;
	int32_t m_iDeltaWinRound;
	int32_t m_iDeltaLossRound;
	int32_t m_iDeltaDrawRound;
	int32_t m_iDeltaEscapeRound;
	int32_t m_iDeltaMoney;

	int32_t m_iGameSeconds;					//游戏耗时


	char m_szGameTag[max_game_tag_length];	//游戏局标签， 唯一表示一局游戏
	char m_szGamePara[max_game_para_length];	//游戏参数

	int32_t m_iRoomBillFlag;		//账单类型: 普通房间账单，VIP房间账单等
	int32_t m_iRoomActiveID;		//房间内活动ID。默认是0，无活动
	*/
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t remain_length = iInLength;
	char* ptmp  = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iSourceIP);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nPort);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomID);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iTableID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaGamePoint);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaWinRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaLossRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaDrawRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaEscapeRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lDeltaHappyBean);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaServiceFare);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lDeltaServiceFareHappyBean);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iGameSeconds);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iACT);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szGameTag,sizeof(m_szGameTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szGamePara,sizeof(m_szGamePara));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomBillFlag);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomActiveID);
	remain_length -= decode_length;
	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}

void CRequestUpdateGamePoint::dump()
{

}

//////////////////////////////////////////////////////////////////////////
CResponseUpdateGamePoint::CResponseUpdateGamePoint()
{
	m_nResultID = 0;
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_szGameTag[0]='\0';
	m_nGameID = -1;
	m_iRoomID = -1;
	m_iTableID = -1;

	//更新后的(全量)数据
	m_iGamePoint = 0;
	m_iWinRound = 0;
	m_iLossRound = 0;
	m_iDrawRound = 0;
	m_iEscapeRound = 0;
	m_iMoney = 0;
	m_iMoneyChg =0;
	m_lHappyBean = 0;
	m_lHappyBeanChg = 0;

	//数据变化量
	m_iDeltaGamePoint = 0;
	m_iDeltaWinRound = 0;
	m_iDeltaLossRound = 0;
	m_iDeltaDrawRound = 0;
	m_iDeltaEscapeRound = 0;
	m_iDeltaMoney = 0;
	m_iDeltaHappyBean = 0;
	m_iACT = -1;
	 
}

CResponseUpdateGamePoint::~CResponseUpdateGamePoint()
{

}

int32_t CResponseUpdateGamePoint::encode( char *pszOut, int32_t& iOutLength )
{
 
	if(NULL == pszOut)
	{
		return fail;
	}
	char* ptmp = pszOut;
	int32_t encode_length = 0;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp,m_nResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount, sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szGameTag, sizeof(m_szGameTag));
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int16(&ptmp,m_nGameID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iRoomID);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iTableID);
	iOutLength += encode_length;
	
	if(result_id_success == m_nResultID)
	{

		encode_length = CCodeEngine::encode_int32(&ptmp,m_iGamePoint);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_iWinRound);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_iLossRound);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_iDrawRound);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_iEscapeRound);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_iMoney);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_iMoneyChg);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int64(&ptmp,m_lHappyBean);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int64(&ptmp,m_lHappyBeanChg);
		iOutLength += encode_length;

		
	}
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaGamePoint);
	iOutLength += encode_length;
		
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaWinRound);
	iOutLength += encode_length;
		
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaLossRound);
	iOutLength += encode_length;
		
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaDrawRound);
	iOutLength += encode_length;
		
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaEscapeRound);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaMoney);
	iOutLength += encode_length;
 
	encode_length = CCodeEngine::encode_int64(&ptmp,m_iDeltaHappyBean);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iACT);
	iOutLength += encode_length;

	return success;
}

int32_t CResponseUpdateGamePoint::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t remain_length = iInLength;
	int32_t decode_length = 0;
	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_szGameTag, sizeof(m_szGameTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomID);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iTableID);
	remain_length -= decode_length;
		
	if(result_id_success == m_nResultID)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iGamePoint);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iWinRound);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLossRound);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDrawRound);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iEscapeRound);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iMoney);
		remain_length -= decode_length;
		
		decode_length  = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iMoneyChg);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lHappyBean);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lHappyBeanChg);
		remain_length -= decode_length;
	}
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaGamePoint);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaWinRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaLossRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaDrawRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaEscapeRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_iDeltaHappyBean);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iACT);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CResponseUpdateGamePoint::dump()
{

} 
 
//////////////////////////////////////////////////////////////////////////
CNotifyRefreshGamePoint::CNotifyRefreshGamePoint()
{
	m_iUin = 0;
	m_nGameID = -1;

	//更新后的(全量)数据
	m_iGamePoint = 0;
	m_iWinRound = 0;
	m_iLossRound = 0;
	m_iDrawRound = 0;
	m_iEscapeRound = 0;
	m_iMoney = 0;
	m_lHappyBean =0 ;
	m_iCharming = 0;
	m_iAchievement = 0;

	m_nNofifyTransparentDataSize = 0;
}

CNotifyRefreshGamePoint::~CNotifyRefreshGamePoint()
{

}

int32_t CNotifyRefreshGamePoint::encode( char *pszOut, int32_t& iOutLength )
{
	/*
	int32_t m_iUin;
	int16_t m_nGameID;

	//更新后的(全量)数据
	int32_t m_iGamePoint;
	int32_t m_iWinRound;
	int32_t m_iLossRound;
	int32_t m_iDrawRound;
	int32_t m_iEscapeRound;
	int32_t m_iMoney;
	*/
	if((NULL == pszOut) || (m_nNofifyTransparentDataSize > max_sub_message_size))
	{
		return fail;
	}
	char* ptmp = pszOut;
	int32_t encode_length = 0;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp,m_nGameID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iGamePoint);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iWinRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iLossRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDrawRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iEscapeRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int64(&ptmp,m_lHappyBean);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp, m_iCharming);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_iAchievement);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += encode_length;
 
	encode_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData, m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	return success;	
}

int32_t CNotifyRefreshGamePoint::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;


	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iGamePoint);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iWinRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLossRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDrawRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iEscapeRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lHappyBean);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCharming);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iAchievement);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	if(m_nNofifyTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData, m_nNofifyTransparentDataSize);
		remain_length -= decode_length;
	}

	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}

void CNotifyRefreshGamePoint::dump()
{

}
///////////////////////////////////////////////////////////////////////////
CNotifyGameResult::CNotifyGameResult()
{
	m_iUin = 0 ;
	m_szAccount[0] = '\0';
	m_nGameID = 0;
	m_iRoomID = -1;
	m_iTableID = -1;

	//更新后的(全量)数据
	m_iGamePoint = 0;
	m_iWinRound = 0;
	m_iLossRound = 0;
	m_iDrawRound = 0;
	m_iEscapeRound = 0;
	m_iMoney = 0;
	m_iMoneyChg = 0;
	m_lHappyBean = 0;    
	m_lHappyBeanChg = 0; 

	//请求数据变化量
	m_iDeltaGamePoint = 0;
	m_iDeltaWinRound = 0;
	m_iDeltaLossRound = 0;
	m_iDeltaDrawRound = 0;
	m_iDeltaEscapeRound = 0;
	m_iDeltaMoney = 0;
	m_iDeltaHappyBean = 0;//请求变更值

	//附加数据
	m_iDeltaGameSeconds = 0;	//本局游戏耗时
	m_iTotalGameSeconds = 0;    //本游戏总共耗时

	//HallServer
	m_iHallServerId = -1;
	m_iHallServerPlayerId = -1;

}
CNotifyGameResult::~CNotifyGameResult()
{


}
int32_t  CNotifyGameResult::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}
	char* ptmp = pszOut;
	int32_t encode_length = 0;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount, sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp,m_nGameID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iRoomID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iTableID);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp,m_iGamePoint);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iWinRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iLossRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDrawRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iEscapeRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iMoneyChg);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int64(&ptmp,m_lHappyBean);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int64(&ptmp,m_lHappyBeanChg);
	iOutLength += encode_length;
	 
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaGamePoint);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaWinRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaLossRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaDrawRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaEscapeRound);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int64(&ptmp,m_iDeltaHappyBean);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iDeltaGameSeconds);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp,m_iTotalGameSeconds);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp,m_iHallServerId);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iHallServerPlayerId);
	iOutLength += encode_length;

	return success;
}

int32_t  CNotifyGameResult::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t remain_length = iInLength;
	int32_t decode_length = 0;
	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iTableID);
	remain_length -= decode_length;

 	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iGamePoint);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iWinRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLossRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDrawRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iEscapeRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iMoney);
	remain_length -= decode_length;

	decode_length  = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iMoneyChg);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lHappyBean);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lHappyBeanChg);
	remain_length -= decode_length;
 

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaGamePoint);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaWinRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaLossRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaDrawRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaEscapeRound);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_iDeltaHappyBean);
	remain_length -= decode_length;
	 
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iDeltaGameSeconds);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iTotalGameSeconds);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iHallServerId);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iHallServerPlayerId);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}

void CNotifyGameResult::dump()
{


}



//////////////////////////////////////////////////////////////////////////


CNotifyRefreshLockMoney::CNotifyRefreshLockMoney()
{
	m_iUin = 0;
     	m_nLockType= lock_type_unlock;
	m_iServerID = -1;
	m_iRoomID = invalid_object_id;
	m_iTableID = invalid_object_id;
}

CNotifyRefreshLockMoney::~CNotifyRefreshLockMoney()
{

}

int32_t CNotifyRefreshLockMoney::encode( char *pszOut, int32_t& iOutLength )
{
	/*
	int32_t m_iUin;
	int32_t m_iServerID;
	int32_t m_iRoomID;
	int32_t m_iTableID;
	*/
	if(NULL == pszOut)
	{
		return fail;
	}
	int32_t encode_length = 0;
	char* ptmp = pszOut;

 	m_nLockType= lock_type_unlock;
 
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp,m_nLockType);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iServerID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iRoomID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_iTableID);
	iOutLength += encode_length;

	return success;
}

int32_t CNotifyRefreshLockMoney::decode( const char *pszIn, const int32_t iInLength )
{
	/*
	int32_t m_iUin;
	int32_t m_iServerID;
	int32_t m_iRoomID;
	int32_t m_iTableID;
	*/
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t remain_length = iInLength;
	int32_t decode_length = 0;
	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nLockType);
	remain_length += decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iServerID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iRoomID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTableID);
	remain_length -= decode_length;

	if(remain_length < 0 )
	{
		return fail;
	}
	return success;
}

void CNotifyRefreshLockMoney::dump()
{

}

//////////////////////////////////////////////////////////////////////////



CRequestUpdateGameConfig::CRequestUpdateGameConfig()
{
	m_iUin = 0;
	memset(&m_stGameConfig, 0 , sizeof(m_stGameConfig));
}

CRequestUpdateGameConfig::~CRequestUpdateGameConfig()
{

}

int32_t CRequestUpdateGameConfig::encode( char *pszOut, int32_t& iOutLength )
{
	/*
	int32_t m_iUin;
	LogicServer::stPlayerGameConfig m_stGameConfig;
	int32_t m_iMinPoint;					//最小积分
	int32_t m_iMaxPointGap;					//最大积分差距

	uint8_t m_byMaxOfflineRate;				//最大掉线率
	uint8_t m_byIPBit;						//
	uint32_t m_uiBitmap;					//


	uint8_t m_byOtherSize;					//个数
	int32_t m_aiOthers[8];
	*/
	if(NULL ==  pszOut)
	{
		return fail;
	}
	int32_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;
		
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;
/*
	encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameConfig.m_iMinPoint);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameConfig.m_iMaxPointGap);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,m_stGameConfig.m_byMaxOfflineRate);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,m_stGameConfig.m_byIPBit);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameConfig.m_uiBitMap);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,m_stGameConfig.m_byOtherSize);
	iOutLength += encode_length;

	for(int32_t i = 0; i < (int32_t)(sizeof(m_stGameConfig.m_aiOthers)/sizeof(m_stGameConfig.m_aiOthers[0])); ++i)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameConfig.m_aiOthers[i]);
		iOutLength += encode_length;
	}
	*/
	encode_length = encode_gameconfig(&ptmp,m_stGameConfig);
	iOutLength += encode_length;
	return success;
}

int32_t CRequestUpdateGameConfig::decode( const char *pszIn, const int32_t iInLength )
{
	/*
	int32_t m_iUin;
	LogicServer::stPlayerGameConfig m_stGameConfig;
	int32_t m_iMinPoint;					//最小积分
	int32_t m_iMaxPointGap;					//最大积分差距

	uint8_t m_byMaxOfflineRate;				//最大掉线率
	uint8_t m_byIPBit;						//
	uint32_t m_uiBitmap;					//


	uint8_t m_byOtherSize;					//个数
	int32_t m_aiOthers[8];
	*/
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;
	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;
/*
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stGameConfig.m_iMinPoint);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stGameConfig.m_iMaxPointGap);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_stGameConfig.m_byMaxOfflineRate);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_stGameConfig.m_byIPBit);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stGameConfig.m_uiBitMap);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_stGameConfig.m_byOtherSize);
	remain_length -= decode_length;

	for(int32_t i = 0; i < 8; ++i)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_stGameConfig.m_aiOthers[i]);
		remain_length -= decode_length;
	}
	*/
	decode_length = decode_gameconfig(&ptmp,m_stGameConfig);
	remain_length -= decode_length;

	return success;
}

void CRequestUpdateGameConfig::dump()
{

}

//////////////////////////////////////////////////////////////////////////
CRequestChangeMoney::CRequestChangeMoney()
{
	m_iUin = 0;
	m_szAccount[0] = '\0'; 
	m_iUnitMoneyChg = 0;
	m_iUnitCount =0 ;
	m_iStrategy =0;
	m_nServiceTag = -1;
	m_szDescription[0] = '\0';
	m_cFlag = 0;
	m_nTransparentDataSize = 0;
	m_szTransTag[0]='\0';

	m_nNofifyTransparentDataSize = 0;
}
CRequestChangeMoney::~CRequestChangeMoney()
{
	
}
int32_t CRequestChangeMoney::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if ((m_nTransparentDataSize > max_transparent_data_size) || (m_nNofifyTransparentDataSize > max_sub_message_size))
	{
		return fail;
	}


	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szAccount,(const int16_t)sizeof(m_szAccount));
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp, m_iUnitMoneyChg);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_iUnitCount);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int8(&ptmp, m_iStrategy);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp, m_nServiceTag);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp, m_cFlag);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szDescription, (const int16_t)sizeof(m_szDescription));
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szTransTag, (const int16_t)sizeof(m_szTransTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, (char*)m_szTransparentData, (const int32_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, (char*)m_szNofifyTransparentData, (const int32_t)m_nNofifyTransparentDataSize);
	iOutLength += encode_length;

	return success;
 
}
int32_t CRequestChangeMoney::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	int16_t remain_length = iInLength;
	int16_t decode_length = 0;

	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,  m_szAccount,(const int16_t)sizeof(m_szAccount));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUnitMoneyChg);
	remain_length -= decode_length;
	
	
	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_iUnitCount);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_iStrategy);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nServiceTag);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_cFlag);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szDescription, (const int16_t) sizeof(m_szDescription));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag, (const int16_t) sizeof(m_szTransTag));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	if (m_nTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, &m_szTransparentData[0], m_nTransparentDataSize);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	if (m_nNofifyTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, (char*)m_szNofifyTransparentData, (const int32_t)m_nNofifyTransparentDataSize);
		remain_length -= decode_length;
	}

	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}
void CRequestChangeMoney::dump()
{

}
//////////////////////////////////////////////////////////////////////////
CResponseChangeMoney::CResponseChangeMoney()
{
	m_nResultID = result_id_success;
	m_iUin = 0;
	m_iUnitMoneyChg = 0;
	m_iUnitCount =0;
	m_iStrategy =0;
	m_iCurrentMoney = 0;
	m_iMoneyChg= 0;
	m_szResultStr[0] = '\0';
	m_nTransparentDataSize = 0;
	m_szTransTag[0]='\0';
}
CResponseChangeMoney::~CResponseChangeMoney()
{

}
int32_t CResponseChangeMoney::encode(char *pszOut, int32_t& iOutLength)
{
	if((NULL == pszOut) || (m_nTransparentDataSize > max_transparent_data_size))
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_iCurrentMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, &m_szTransparentData[0], (const int32_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, m_iUnitMoneyChg);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int16(&ptmp, m_iUnitCount);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int8(&ptmp, m_iStrategy);
	iOutLength += encode_length;
	
	if (result_id_success == m_nResultID)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, m_iMoneyChg);
		iOutLength += encode_length;
	}
	else
	{
		encode_length = CCodeEngine::encode_string(&ptmp,m_szResultStr,sizeof(m_szResultStr));
		iOutLength += encode_length;
	}
	encode_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szTransTag, (const int16_t)sizeof(m_szTransTag));
	iOutLength += encode_length;
	return success;
}
int32_t CResponseChangeMoney::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	int16_t remain_length = iInLength;
	int16_t decode_length = 0;
	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iCurrentMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	if (m_nTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, &m_szTransparentData[0], (const int32_t)m_nTransparentDataSize);
		remain_length -= decode_length;
	}
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUnitMoneyChg);
	remain_length -= decode_length;
	

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_iUnitCount);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int8(&ptmp,(uint8_t*)&m_iStrategy);
	remain_length -= decode_length;
	
	if(result_id_success == m_nResultID)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iMoneyChg);
		remain_length -= decode_length;
	}
	else
	{
		decode_length = CCodeEngine::decode_string(&ptmp, m_szResultStr, (const int16_t)sizeof(m_szResultStr));
		remain_length -= decode_length;

	}
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag, (const int16_t)sizeof(m_szTransTag));
	remain_length -= decode_length;
	
	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}
void CResponseChangeMoney::dump()
{

}
//////////////////////////////////////////////////////////////////////////
CNotifyRefreshGameConfig::CNotifyRefreshGameConfig()
{
	m_unUin = 0;
	m_unTime = 0;

	m_nGameId = invalid_object_id;
	memset(&m_stGameCfg , 0 , sizeof(m_stGameCfg));
}
CNotifyRefreshGameConfig::~CNotifyRefreshGameConfig()
{

}
int32_t CNotifyRefreshGameConfig::encode(char *pszOut, int32_t& iOutLength)
{
	/*
	int32_t m_unUin;
	int32_t m_unTime;

	int32_t m_nGameId;
	LogicServer::stPlayerGameConfig  m_stGameCfg;
	struct 
	int32_t m_iMinPoint;					//最小积分
	int32_t m_iMaxPointGap;					//最大积分差距

	uint8_t m_byMaxOfflineRate;				//最大掉线率
	uint8_t m_byIPBit;						//
	uint32_t m_uiBitmap;					//

	uint8_t m_byOtherSize;					//个数
	int32_t m_aiOthers[8];
	end struct 
	*/
	if( NULL == pszOut || iOutLength < 0 )
	{
		return fail;
	}
	int32_t encode_length = 0;
	char* ptmp = const_cast<char*>(pszOut);

	iOutLength = 0;
		
	encode_length = CCodeEngine::encode_int32(&ptmp,m_unUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_unTime);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_nGameId);
	iOutLength += encode_length;
/*
	encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameCfg.m_iMinPoint);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameCfg.m_iMaxPointGap);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,m_stGameCfg.m_byMaxOfflineRate);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,m_stGameCfg.m_byIPBit);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameCfg.m_uiBitmap);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp,m_stGameCfg.m_byOtherSize);
	iOutLength += encode_length;

	for(int32_t i = 0; i < 8; ++i)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp,m_stGameCfg.m_aiOthers[i]);
		iOutLength += encode_length;
	}
	*/
	encode_length = encode_gameconfig(&ptmp,m_stGameCfg);
	iOutLength += encode_length;
	return success;
}
int32_t CNotifyRefreshGameConfig::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;
	char* ptmp = const_cast<char*>(pszIn);

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_unUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_unTime);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nGameId);
	remain_length -= decode_length;
 
	decode_length = decode_gameconfig(&ptmp,m_stGameCfg);
	remain_length -= decode_length;
	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}

void CNotifyRefreshGameConfig::dump()
{

}
//////////////////////////////////////////////////////////////////////////
CNotifyDeletePlayerRefrence::CNotifyDeletePlayerRefrence()
{
	m_iUin = 0;
	memset(&m_stDeltaCommnonData, 0 ,sizeof(m_stDeltaCommnonData));
}

CNotifyDeletePlayerRefrence::~CNotifyDeletePlayerRefrence()
{

}

int32_t CNotifyDeletePlayerRefrence::encode( char *pszOut, int32_t& iOutLength )
{

	if(NULL == pszOut)
	{
		return fail;
	}
	int32_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;


	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	encode_length = encode_player_common_info(&ptmp, m_stDeltaCommnonData);
	iOutLength += encode_length;

	return success;
}

int32_t CNotifyDeletePlayerRefrence::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	int32_t remain_length = iInLength;
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;
 

	decode_length = decode_player_common_info(&ptmp, m_stDeltaCommnonData);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CNotifyDeletePlayerRefrence::dump()
{

}

/////////////////////////////////////////////////////////////////////////////////////
CRequestGetUserInfo::CRequestGetUserInfo()
{
	m_unUin = 0;
}
CRequestGetUserInfo::~CRequestGetUserInfo()
{

}

CRequestGetUserInfo& CRequestGetUserInfo::operator =(const CRequestGetUserInfo& original)
{
	if (this == &original)
	{
		return *this;
	}

	m_unUin = original.m_unUin;
	
	return *this;
}
int32_t CRequestGetUserInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}
	
	iOutLength = 0;
	
	char* ptmp = pszOut;
	int32_t encode_length = 0;
	encode_length = CCodeEngine::encode_int32(&ptmp,m_unUin);
	iOutLength += encode_length;
	return success;
}

int32_t CRequestGetUserInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_unUin);
	remain_length -= decode_length;
	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}
void CRequestGetUserInfo::dump()
{

}
///////////////////////////////////////////////////////////////////////////////////////
CResponseGetUserInfo::CResponseGetUserInfo()
{
	m_nResultID = result_id_success;
	m_iUin = 0;

	m_nMoney = 0;
	m_nCharming = 0;
	m_nAchievement = 0;
	m_lHappyBean  = 0;
	//m_szResultStr[0] = '\0';
	m_lLastOfflineCharming = 0;
	memset(&m_stServiceData,0,sizeof(m_stServiceData));
	m_szIDCard[0] = '\0';
	m_szUserName[0] ='\0';

	memset(&m_stVIP,0,sizeof(m_stVIP));
}
CResponseGetUserInfo::~CResponseGetUserInfo()
{

}

CResponseGetUserInfo& CResponseGetUserInfo::operator =(const CResponseGetUserInfo& original)
{
	if (this == &original)
	{
		return *this;
	}

	m_nResultID = original.m_nResultID;
	m_iUin = original.m_iUin;

	m_nMoney = original.m_nMoney;
	m_nCharming = original.m_nCharming;
	m_nAchievement = original.m_nAchievement;
	m_lHappyBean = original.m_lHappyBean;
	m_lLastOfflineCharming = original.m_lLastOfflineCharming;
	
	//STRNCPY(m_szResultStr, original.m_szResultStr, sizeof(m_szResultStr));
 	memcpy(&m_stServiceData,&original.m_stServiceData,sizeof(m_stServiceData));
	STRNCPY(m_szIDCard,original.m_szIDCard,sizeof(m_szIDCard));
	STRNCPY(m_szUserName,&original.m_szUserName[0],sizeof(m_szUserName));

	memcpy((void *)&original.m_stVIP,(const void *)&m_stVIP,sizeof(original.m_stVIP));
	
	return *this;
}

int32_t CResponseGetUserInfo::encode(char *pszOut, int32_t& iOutLength)
{
	 
	
	if(NULL == pszOut)
	{
		return fail;
	}
	char* ptmp = pszOut;
	int32_t encode_length = 0;
	
	iOutLength = 0;
	
	encode_length = CCodeEngine::encode_int16(&ptmp,m_nResultID);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	if(result_id_success == m_nResultID)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp,m_nMoney);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_nCharming);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_nAchievement);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int64(&ptmp,m_lHappyBean);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_lLastOfflineCharming);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp,m_stServiceData.m_nCount);
		iOutLength += encode_length;


		for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
		{
			encode_length = encode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
			iOutLength += encode_length;
		}

		encode_length = CCodeEngine::encode_string(&ptmp,m_szIDCard,sizeof(m_szIDCard));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp,m_szUserName,sizeof(m_szUserName));
		iOutLength += encode_length;

		encode_length = encode_player_vipdata(&ptmp,m_stVIP);
		iOutLength += encode_length;


	}
	//else
	//{
	//	encode_length = CCodeEngine::encode_string(&ptmp,m_szResultStr,max_string_message_length);
	//	iOutLength += encode_length;
	//}
	return success;
}
int32_t CResponseGetUserInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	if(result_id_success == m_nResultID)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nMoney);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nCharming);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nAchievement);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lHappyBean);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_lLastOfflineCharming);
		remain_length -= decode_length;
		
		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_stServiceData.m_nCount);
		remain_length -= decode_length;

		if(m_stServiceData.m_nCount>max_service_type_count)
		{
			m_stServiceData.m_nCount=max_service_type_count;
		}

		for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
		{
			decode_length = decode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
			remain_length -= decode_length;
		}


		decode_length = CCodeEngine::decode_string(&ptmp,m_szIDCard,sizeof(m_szIDCard));
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp,m_szUserName,sizeof(m_szUserName));
		remain_length -= decode_length;

		decode_length = decode_player_vipdata(&ptmp,m_stVIP);
		remain_length -= decode_length;
	}
	 

	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}
void  CResponseGetUserInfo::dump()
{
	CResponseGetUserInfo::dump();
}


//////////////////////////////
CResponseGetUserProfile::CResponseGetUserProfile()
{
	m_iLastBecomeVIPTime = 0;
}

CResponseGetUserProfile::~CResponseGetUserProfile()
{
	
}

int32_t  CResponseGetUserProfile::encode(char *pszOut, int32_t& iOutLength)
{
		
	if(NULL == pszOut)
	{
		return fail;
	}
	char* ptmp = pszOut;
	int32_t encode_length = 0;
	
	iOutLength = 0;
	
	encode_length = CCodeEngine::encode_int16(&ptmp,m_nResultID);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	if(result_id_success == m_nResultID)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp,m_nMoney);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_nCharming);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_nAchievement);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int64(&ptmp,m_lHappyBean);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_lLastOfflineCharming);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp,m_stServiceData.m_nCount);
		iOutLength += encode_length;


		for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
		{
			encode_length = encode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
			iOutLength += encode_length;
		}

		encode_length = CCodeEngine::encode_string(&ptmp,m_szIDCard,sizeof(m_szIDCard));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_string(&ptmp,m_szUserName,sizeof(m_szUserName));
		iOutLength += encode_length;

		encode_length = encode_player_vipdata(&ptmp,m_stVIP,&m_iLastBecomeVIPTime);
		iOutLength += encode_length;

	}
	//else
	//{
	//	encode_length = CCodeEngine::encode_string(&ptmp,m_szResultStr,max_string_message_length);
	//	iOutLength += encode_length;
	//}
	return success;
		
}

int32_t  CResponseGetUserProfile::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	if(result_id_success == m_nResultID)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nMoney);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nCharming);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nAchievement);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int64(&ptmp,(uint64_t*)&m_lHappyBean);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_lLastOfflineCharming);
		remain_length -= decode_length;
		
		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_stServiceData.m_nCount);
		remain_length -= decode_length;

		if(m_stServiceData.m_nCount>max_service_type_count)
		{
			m_stServiceData.m_nCount=max_service_type_count;
		}

		for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
		{
			decode_length = decode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
			remain_length -= decode_length;
		}


		decode_length = CCodeEngine::decode_string(&ptmp,m_szIDCard,sizeof(m_szIDCard));
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_string(&ptmp,m_szUserName,sizeof(m_szUserName));
		remain_length -= decode_length;

		decode_length = decode_player_vipdata(&ptmp,m_stVIP,&m_iLastBecomeVIPTime);
		remain_length -= decode_length;
	}
	 

	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}

void CResponseGetUserProfile::dump()
{
		
}

//..........................
/////////////////////////////////////////////////////////////////////////////////////
CRequestGetOtherUserInfo::CRequestGetOtherUserInfo()
{
	m_unUin = 0;
}
CRequestGetOtherUserInfo::~CRequestGetOtherUserInfo()
{

}
int32_t CRequestGetOtherUserInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}
	
	iOutLength = 0;
	
	char* ptmp = pszOut;
	int32_t encode_length = 0;
	encode_length = CCodeEngine::encode_int32(&ptmp,m_unUin);
	iOutLength += encode_length;
	return success;
}

int32_t CRequestGetOtherUserInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_unUin);
	remain_length -= decode_length;
	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}
void CRequestGetOtherUserInfo::dump()
{

}
///////////////////////////////////////////////////////////////////////////////////////
CResponseGetOtherUserInfo::CResponseGetOtherUserInfo()
{
	m_nResultID = result_id_success;
	m_iUin = 0;
 
	m_nCharming = 0;
	m_nAchievement = 0;
	m_szResultStr[0] = '\0';
}
CResponseGetOtherUserInfo::~CResponseGetOtherUserInfo()
{

}
int32_t CResponseGetOtherUserInfo::encode(char *pszOut, int32_t& iOutLength)
{
	 
	
	if(NULL == pszOut)
	{
		return fail;
	}
	char* ptmp = pszOut;
	int32_t encode_length = 0;
	
	iOutLength = 0;
	
	encode_length = CCodeEngine::encode_int16(&ptmp,m_nResultID);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp,m_iUin);
	iOutLength += encode_length;

	if(result_id_success == m_nResultID)
	{
		 

		encode_length = CCodeEngine::encode_int32(&ptmp,m_nCharming);
		iOutLength += encode_length;
		
		encode_length = CCodeEngine::encode_int32(&ptmp,m_nAchievement);
		iOutLength += encode_length;
	}
	else
	{
		encode_length = CCodeEngine::encode_string(&ptmp,m_szResultStr,sizeof(m_szResultStr));
		iOutLength += encode_length;
	}
	return success;
}
		
int32_t CResponseGetOtherUserInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nResultID);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iUin);
	remain_length -= decode_length;

	if(result_id_success == m_nResultID)
	{
		 
		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nCharming);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_nAchievement);
		remain_length -= decode_length;
	}
	else
	{
		decode_length = CCodeEngine::decode_string(&ptmp,m_szResultStr,sizeof(m_szResultStr));
		remain_length -= decode_length;

	}

	if(remain_length < 0)
	{
		return fail;
	}
	return success;
}
void  CResponseGetOtherUserInfo::dump()
{

}
/////////////////

//CResponseUpdateGameConfig
CResponseUpdateGameConfig::CResponseUpdateGameConfig()
{
	m_nResultID = result_id_success;
}

CResponseUpdateGameConfig::~CResponseUpdateGameConfig()
{

}

int32_t CResponseUpdateGameConfig::encode( char *pszOut, int32_t& iOutLength )
{
	if (NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += coded_length;

	return success;
}

int32_t CResponseUpdateGameConfig::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	if (remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CResponseUpdateGameConfig::dump()
{

}

/////////////////////////////////////////////////////////////////
CRequestGetPlayCommonData::CRequestGetPlayCommonData()
{
	m_nUin = 0;
	m_nDataCount = 0;
	memset(m_anDataType,0,PCD_MAX_DATA_NUMBER);
	memset(m_anDataOtherPosi,0,PCD_MAX_DATA_NUMBER);
	return ;
}


CRequestGetPlayCommonData::~CRequestGetPlayCommonData()
{

	return ;
}

int32_t  CRequestGetPlayCommonData::encode(char *pszOut, int32_t& iOutLength)
{
	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_nUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataCount);
	iOutLength += coded_length;

	for(int16_t x = 0;x<m_nDataCount;++x)
	{
		coded_length = CCodeEngine::encode_int8(&ptmp, m_anDataType[x]);
		iOutLength += coded_length;
	}

	for(int16_t x = 0;x<m_nDataCount;++x)
	{
		coded_length = CCodeEngine::encode_int8(&ptmp, m_anDataOtherPosi[x]);
		iOutLength += coded_length;
	}
	return success;
}

int32_t  CRequestGetPlayCommonData::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDataCount);
	remain_length -= decode_length;

	if(m_nDataCount > PCD_MAX_DATA_NUMBER)
	{
		m_nDataCount = PCD_MAX_DATA_NUMBER;
	}

	for(int16_t x = 0;x<m_nDataCount;++x)
	{
		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_anDataType[x]);
		remain_length -= decode_length;
	}

	for(int16_t x = 0;x<m_nDataCount;++x)
	{
		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_anDataOtherPosi[x]);
		remain_length -= decode_length;
	}
	return success;
}

void CRequestGetPlayCommonData::dump()
{

	return ;
}

//////////////////////////////////////////////////////////////////////////////////
CResponseGetPlayCommonData::CResponseGetPlayCommonData()
{
	m_nUin = 0;
	m_nResultID  = 0;
	m_nDataCount = 0;
	memset(m_astData,0,sizeof(m_astData));
}

CResponseGetPlayCommonData::~CResponseGetPlayCommonData()
{

}

int32_t  CResponseGetPlayCommonData::encode(char *pszOut, int32_t& iOutLength)
{
	if (NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

 	coded_length = CCodeEngine::encode_int32(&ptmp, m_nUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += coded_length;

	if( m_nResultID  == result_id_success)
	{
		coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataCount);
		iOutLength += coded_length;

		for(int32_t i=0;i<m_nDataCount;++i)
		{
			coded_length = encode_player_common_data(&ptmp,m_astData[i]);	
			iOutLength += coded_length;
		}
	}
	return success;
}

int32_t  CResponseGetPlayCommonData::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	if( m_nResultID == result_id_success )
	{
		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDataCount);
		remain_length -= decode_length;

		for(int32_t i=0;i<m_nDataCount;++i)
		{
			decode_length = decode_player_common_data(&ptmp,m_astData[i]);	
			remain_length -= decode_length;
		}
	}

	return success;
}

void CResponseGetPlayCommonData::dump()
{


}


///////////////////////////////////////////////////////////////////////
CRequestUpdatePlayCommonData::CRequestUpdatePlayCommonData()
{	
	 m_srcUin = 0;
	 m_dstUin = 0;
	 m_srcAccount[0] = '\0';
	 m_dstAccount[0] = '\0';
	 memset(&m_DeltaData[0],0,sizeof(m_DeltaData));
	 m_nDataCount =0;
	 m_nTransparentDataSize=0;
	 memset(m_TransparentData,0,max_transparent_data_size);
	 m_szTransTag[0]='\0';


	 m_nNofifyTransparentDataSize = 0;
}

CRequestUpdatePlayCommonData::~CRequestUpdatePlayCommonData()
{
	
}

void CRequestUpdatePlayCommonData::dump()
{
	
}

 

int32_t CRequestUpdatePlayCommonData::encode( char *pszOut, int32_t& iOutLength )
{
	if (NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize >  max_transparent_data_size)
	{
		return fail;
	}

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_srcUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_srcAccount,(const int16_t)sizeof(m_srcAccount));
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_int32(&ptmp, m_dstUin);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, m_dstAccount,(const int16_t)sizeof(m_dstAccount));
	iOutLength += coded_length;
		
	coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataCount);
	iOutLength += coded_length; 
	
	for(int32_t i=0;i<m_nDataCount;++i)
	{
		coded_length = encode_player_common_data(&ptmp,m_DeltaData[i]);	
		iOutLength += coded_length; 
	}
 
	coded_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szTransTag, (const int16_t)sizeof(m_szTransTag));
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	iOutLength += coded_length;

	return success;
}

int32_t CRequestUpdatePlayCommonData::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_srcUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_srcAccount,(const int16_t)sizeof(m_srcAccount));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_dstUin);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_dstAccount,(const int16_t)sizeof(m_dstAccount));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDataCount);
	remain_length -= decode_length;
	
	for(int32_t i=0;i<m_nDataCount;++i)
	{
		decode_length = decode_player_common_data(&ptmp,m_DeltaData[i]);	
		remain_length -= decode_length;
	}
 
	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if (m_nTransparentDataSize >  max_transparent_data_size)
	{
		return fail;
	}
	
	if( m_nTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
		remain_length -= decode_length;
	}
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag, (const int16_t)sizeof(m_szTransTag));
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	if(m_nNofifyTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData, m_nNofifyTransparentDataSize);
		remain_length -= decode_length;
	}

	return success;
}


///////////////////////////////////////////////////////////////////////
CResponseUpdatePlayCommonData::CResponseUpdatePlayCommonData()
{	
	m_nResultID = result_id_success;
	m_srcUin = 0;
	m_dstUin = 0;
	m_nDataCount = 0;
	m_srcAccount[0] = '\0';
	m_dstAccount[0] = '\0';
	memset(&m_DeltaData[0],0,sizeof(m_DeltaData));
	memset(&m_CurrentData[0],0,sizeof(m_CurrentData));
	m_nTransparentDataSize = 0;
	memset(&m_TransparentData[0],0,sizeof(m_TransparentData));
	m_szTransTag[0]='\0';
}

CResponseUpdatePlayCommonData::~CResponseUpdatePlayCommonData()
{
	
}

void CResponseUpdatePlayCommonData::dump()
{
	
}

 

int32_t CResponseUpdatePlayCommonData::encode( char *pszOut, int32_t& iOutLength )
{
	if (NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;
	
	coded_length = CCodeEngine::encode_int16(&ptmp, m_nResultID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_srcUin);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, m_srcAccount,(const int16_t)sizeof(m_srcAccount));
	iOutLength += coded_length;
	
	
	coded_length = CCodeEngine::encode_int32(&ptmp, m_dstUin);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, m_dstAccount,(const int16_t)sizeof(m_dstAccount));
	iOutLength += coded_length;
	

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataCount);
	iOutLength += coded_length;
	
	for(int32_t i=0;i<m_nDataCount;++i)
	{
		coded_length = encode_player_common_data(&ptmp,m_DeltaData[i]);	
		iOutLength += coded_length;
	}
	
	if( m_nResultID == result_id_success )
	{
		for(int32_t i=0;i<m_nDataCount;++i)
		{
			coded_length = encode_player_common_data(&ptmp,m_CurrentData[i]);	
			iOutLength += coded_length;
		}
	}

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nTransparentDataSize);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, (const char*)m_szTransTag, max_game_tag_length);
	iOutLength += coded_length;
	
	return success;
}

int32_t CResponseUpdatePlayCommonData::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	
	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_srcUin);
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_srcAccount,(const int16_t)sizeof(m_srcAccount));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_dstUin);
	remain_length -= decode_length;
	
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_dstAccount,(const int16_t)sizeof(m_dstAccount));
	remain_length -= decode_length;
	
	
	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDataCount);
	remain_length -= decode_length;
	
	for(int32_t i=0;i<m_nDataCount;++i)
	{
		decode_length = decode_player_common_data(&ptmp,m_DeltaData[i]);	
		remain_length -= decode_length;
	}
	
	if( m_nResultID == result_id_success )
	{
		for(int32_t i=0;i<m_nDataCount;++i)
		{
			decode_length = decode_player_common_data(&ptmp,m_CurrentData[i]);	
			remain_length -= decode_length;
		}
	}

	decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)& m_nTransparentDataSize);
	remain_length -= decode_length;

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}
	
	if( m_nTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
		remain_length -= decode_length;
	}
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,max_game_tag_length);
	remain_length -= decode_length;
	
	return success;
}




///////////////////////////////////////////////////////////////////////
CNotifyRefreshPlayCommonData::CNotifyRefreshPlayCommonData()
{	
	m_iUin = 0;
	m_nDataCount = 0;
	m_szAccount[0] = '\0';
	memset(&m_CurrentData[0],0,sizeof(m_CurrentData));

	m_nNofifyTransparentDataSize = 0;
}

CNotifyRefreshPlayCommonData::~CNotifyRefreshPlayCommonData()
{
	
}

void CNotifyRefreshPlayCommonData::dump()
{
	
}

int32_t CNotifyRefreshPlayCommonData::encode( char *pszOut, int32_t& iOutLength )
{
	if (NULL == pszOut)
	{
		return fail;
	}

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,(const int16_t)sizeof(m_szAccount));
	iOutLength += coded_length;
		
	coded_length = CCodeEngine::encode_int16(&ptmp, m_nDataCount);
	iOutLength += coded_length;
	 	 
	for(int32_t i=0;i<m_nDataCount;++i)
	{
		coded_length = encode_player_common_data(&ptmp,m_CurrentData[i]);	
		iOutLength += coded_length;
	}

	coded_length = CCodeEngine::encode_int16(&ptmp, m_nNofifyTransparentDataSize);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	iOutLength += coded_length;

	return success;
}

int32_t CNotifyRefreshPlayCommonData::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;
	
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,(const int16_t)sizeof(m_szAccount));
	remain_length -= decode_length;
	
	
	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDataCount);
	remain_length -= decode_length;
	
	for(int32_t i=0;i<m_nDataCount;++i)
	{
		decode_length = decode_player_common_data(&ptmp,m_CurrentData[i]);	
		remain_length -= decode_length;
	}


	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	remain_length -= decode_length;

	if (m_nNofifyTransparentDataSize > max_sub_message_size)
	{
		return fail;
	}

	if(m_nNofifyTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
		remain_length -= decode_length;
	}
	return success;
}

/*
///////////////////////////////////////////////////////////////////////
CNotifyPlayerStatus::CNotifyPlayerStatus()
{	
	m_iUin = 0;
	memset(&m_szAccount[0],0,sizeof(m_szAccount));
	m_cState = player_logout ; 
}

CNotifyPlayerStatus::~CNotifyPlayerStatus()
{
	
}

void CNotifyPlayerStatus::dump()
{
	
}

 

int32_t CNotifyPlayerStatus::encode( char *pszOut, int32_t& iOutLength )
{
	if (NULL == pszOut)
	{
		return fail;
	}
	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, m_iUin);
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += coded_length;
	
	coded_length = CCodeEngine::encode_int8(&ptmp, m_cState);
	iOutLength += coded_length;
	
	return success;
}

int32_t CNotifyPlayerStatus::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;
	
	
	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;
	
	decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cState);
	remain_length -= decode_length;
	
	return success;
}

*/
///////////////////////////////////////////////////////////////////////////////////////

int32_t encode_gameconfig( char** pOut, stPlayerGameConfig& config )
{
	//预留出player game config 的长度
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	char* ptmp = *pOut;
	int32_t playergameconfig_size = 0;
	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间
	

	coded_length = CCodeEngine::encode_int32(pOut, config.m_uiBitMap);
	playergameconfig_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, config.m_iMinPoint);
	playergameconfig_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, config.m_iMaxPointGap);
	playergameconfig_size += coded_length;

	coded_length =	CCodeEngine::encode_int8(pOut, config.m_byMaxOfflineRate);
	playergameconfig_size += coded_length;

	coded_length =	CCodeEngine::encode_int8(pOut, config.m_byIPBit);
	playergameconfig_size += coded_length;

	coded_length =	CCodeEngine::encode_int8(pOut, config.m_byOtherSize);
	playergameconfig_size += coded_length;

	for(int32_t x=0;x<config.m_byOtherSize;++x)
	{
		coded_length = CCodeEngine::encode_int32(pOut, config.m_aiOthers[x]);
		playergameconfig_size += coded_length;
	}

	//编码player game config的实际长度
	*pOut -= (playergameconfig_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pOut, playergameconfig_size);

	iOutLength = (playergameconfig_size + coded_length);

	*pOut = ptmp + iOutLength; //	把指针指向下一个可用位置


	return iOutLength;

}

int32_t decode_gameconfig(char** pOut, stPlayerGameConfig& config)
{
	//预留出player game config 的长度
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	//char* ptmp = *pOut;
	int16_t playergameconfig_size = 0;

	int32_t iOutLength = 0;
	int32_t decode_length = 0;
	decode_length = CCodeEngine::decode_int16(pOut,(uint16_t*)&playergameconfig_size);
	iOutLength += decode_length;

	decode_length = CCodeEngine::decode_int32(pOut,(uint32_t*)&config.m_uiBitMap);
	iOutLength += decode_length;
	playergameconfig_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut,(uint32_t*)&config.m_iMinPoint);
	iOutLength += decode_length;
	playergameconfig_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut,(uint32_t*)&config.m_iMaxPointGap);
	iOutLength += decode_length;
	playergameconfig_size -= decode_length;

	decode_length = CCodeEngine::decode_int8(pOut,(uint8_t*)&config.m_byMaxOfflineRate);
	iOutLength += decode_length;
	playergameconfig_size -= decode_length;

	decode_length = CCodeEngine::decode_int8(pOut,(uint8_t*)&config.m_byIPBit);
	iOutLength += decode_length;
	playergameconfig_size -= decode_length;

	decode_length =	CCodeEngine::decode_int8(pOut,(uint8_t*)&config.m_byOtherSize);
	iOutLength += decode_length;
	playergameconfig_size -= decode_length;

	if(config.m_byOtherSize>max_game_other_data_count)
	{
		config.m_byOtherSize = max_game_other_data_count;
	}

	for(int32_t x=0;x<config.m_byOtherSize;++x)
	{
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&config.m_aiOthers[x]);
		iOutLength += decode_length;
		playergameconfig_size -= decode_length;
	}

	if(playergameconfig_size < 0)
	{
		return 0;
	}
	else
	{	
		//跳过剩余的字节
		skip_byte_of(pOut, playergameconfig_size);
		iOutLength += playergameconfig_size;
	}

	return iOutLength;

}
// void skip_byte_of(char** pOut, int32_t iByteCount)
// {
// 	if (pOut == NULL || NULL == *pOut || iByteCount <= 0)
// 	{
// 		return;
// 	}
// 
// 	(*pOut) += iByteCount;
// 
// 	return;
// }


int32_t encode_player_common_data(char** pOut, stPlayerCommonData& data)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}
	int32_t iOutLength = 0;
	int32_t coded_length = 0;
	
	coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_iDataType);
	iOutLength += coded_length;
	
	switch(data.m_iDataType)
	{
	case PCD_Charming:
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_iCharming);
		iOutLength += coded_length;
		break;
	case PCD_Achievement:
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_iAchievement);
		iOutLength += coded_length;
		break;
	 
	case PCD_PunishMethod :
		coded_length = CCodeEngine::encode_string(pOut, data.m_DataValue.m_stPunish.m_szValidDate,max_date_string_length);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_DataValue.m_stPunish.m_iBlackLevel);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)data.m_DataValue.m_stPunish.m_iPunishMethod);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(pOut, data.m_DataValue.m_stPunish.m_szDescString,max_operate_description_length);
		iOutLength += coded_length;
		break;
	case PCD_OtherData:
		coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_DataValue.m_stOtherData.m_nOtherDataIndex);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_DataValue.m_stOtherData.m_nUpdateMode);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stOtherData.m_iOtherDataValue);
		iOutLength += coded_length;
		break;
	case PCD_LoginCount:
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_iLoginCount);
		iOutLength += coded_length;
		break;
	case PCD_LastLoginTime:
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_iLastLoginTime);
		iOutLength += coded_length;
		break;
	case PCD_LastLoginIP:
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_iLastLoginIP);
		iOutLength += coded_length;
		break;
	case PCD_WebQunData:
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stWebQunData.m_nDataSize);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_memory(pOut, data.m_DataValue.m_stWebQunData.m_szWebQunInfo,data.m_DataValue.m_stWebQunData.m_nDataSize);
		iOutLength += coded_length;
		break;
	case PCD_VipData:
	        coded_length = encode_player_vipdata(pOut,data.m_DataValue.m_stVipData);
	        iOutLength += coded_length;
	        /*
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stVipData.m_iGameVipExpireTime);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stVipData.m_iGameVIPScore);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stVipData.m_iGameVIPLevel);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(pOut,data.m_DataValue.m_stVipData.m_szVIPTips,max_system_message_length);
		iOutLength += coded_length;
		*/
		
		break;
	case PCD_IdCard:
		coded_length = CCodeEngine::encode_string(pOut, data.m_DataValue.m_szIDCard,sizeof(data.m_DataValue.m_szIDCard));
		iOutLength += coded_length;
		break;
	case PCD_Sex:
		coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_DataValue.m_nSex);
		iOutLength += coded_length;
		break;
	case PCD_Birthday:
		coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)data.m_DataValue.m_stBirThday.m_iYear);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)data.m_DataValue.m_stBirThday.m_iMonth);
		iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)data.m_DataValue.m_stBirThday.m_iDay);
		iOutLength += coded_length;
		break;
	default:
		break;
	};

	return iOutLength;
}

int32_t decode_player_common_data(char** pOut, stPlayerCommonData& data)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}
	int32_t iOutLength = 0;
	int32_t decode_length = 0;
	
	decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data.m_iDataType);
	iOutLength += decode_length;
	
	switch(data.m_iDataType)
	{
	case PCD_Charming:
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_iCharming);
		iOutLength += decode_length;
		break;
	case PCD_Achievement:
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_iAchievement);
		iOutLength += decode_length;
		break;
	case PCD_PunishMethod :
		decode_length = CCodeEngine::decode_string(pOut, data.m_DataValue.m_stPunish.m_szValidDate,max_date_string_length);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data.m_DataValue.m_stPunish.m_iBlackLevel);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&data.m_DataValue.m_stPunish.m_iPunishMethod);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_string(pOut, data.m_DataValue.m_stPunish.m_szDescString,max_operate_description_length);
		iOutLength += decode_length;
		break;
	case PCD_OtherData:
		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data.m_DataValue.m_stOtherData.m_nOtherDataIndex);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data.m_DataValue.m_stOtherData.m_nUpdateMode);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stOtherData.m_iOtherDataValue);
		iOutLength += decode_length;
		break;
	case PCD_LoginCount:
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_iLoginCount);
		iOutLength += decode_length;
		break;
	case PCD_LastLoginTime:
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_iLastLoginTime);
		iOutLength += decode_length;
		break;
	case PCD_LastLoginIP:
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_iLastLoginIP);
		iOutLength += decode_length;
		break;
	case PCD_WebQunData:
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stWebQunData.m_nDataSize);
		iOutLength += decode_length;
		
		if(data.m_DataValue.m_stWebQunData.m_nDataSize>max_web_qun_data_size)
		{
			data.m_DataValue.m_stWebQunData.m_nDataSize = max_web_qun_data_size;
		}
		decode_length = CCodeEngine::decode_memory(pOut, data.m_DataValue.m_stWebQunData.m_szWebQunInfo,data.m_DataValue.m_stWebQunData.m_nDataSize);
		iOutLength += decode_length;
		break;
	case PCD_VipData:
	        decode_length = decode_player_vipdata(pOut,data.m_DataValue.m_stVipData);
	        iOutLength += decode_length;
		break;
	case PCD_IdCard:
		decode_length = CCodeEngine::decode_string(pOut, data.m_DataValue.m_szIDCard,sizeof(data.m_DataValue.m_szIDCard));
		iOutLength += decode_length;
		break;
	case PCD_Sex:
		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data.m_DataValue.m_nSex);
		iOutLength += decode_length;
		break;
	case PCD_Birthday:
		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&data.m_DataValue.m_stBirThday.m_iYear);
		iOutLength += decode_length;
		
		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&data.m_DataValue.m_stBirThday.m_iMonth);
		iOutLength += decode_length;
		
		decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&data.m_DataValue.m_stBirThday.m_iDay);
		iOutLength += decode_length;

		break;
	default:
		break;
	};
	
	return iOutLength;
}

//
int32_t encode_player_common_info(char** pOut, stPlayerCommonInfo& common)
{
	//前2Byte存储player common的实际大小

	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t playercommon_size = 0;
	char* ptmp = *pOut;

	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iCoin);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iLoginCount);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iOfflineCount);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iCharming);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iAchievement);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int64(pOut, (uint64_t)common.m_lHappyBean);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iBlackLevel);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_string(pOut, (const char*)common.m_szValidDate,max_date_string_length);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iPunishMethod);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_string(pOut, (const char*)common.m_szLastLoginDate, max_date_string_length);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iLastLoginIP);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_string(pOut, (const char*)common.m_szDescString, max_describe_string_length);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_string(pOut, (const char*)common.m_szLastConsumeDate, max_date_string_length);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iMiscFlag);
	playercommon_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iHappyBean_Daily_PresentedCount);
	playercommon_size += coded_length;
 
	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iHappyBean_Daily_LastPresentedTime);
	playercommon_size += coded_length;
	
	
	
	
	
	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)common.m_stWebQunData.m_nDataSize);
	playercommon_size += coded_length;
	
	coded_length = CCodeEngine::encode_memory(pOut,common.m_stWebQunData.m_szWebQunInfo ,common.m_stWebQunData.m_nDataSize);
	playercommon_size += coded_length;
	//

// 	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)common.m_nOtherCount);
// 	playercommon_size += coded_length;

	for (size_t i = 0; i < sizeof(common.m_aiOthers)/sizeof(common.m_aiOthers[0]); ++i)
	{
		coded_length = CCodeEngine::encode_int32(pOut, common.m_aiOthers[i]);
		playercommon_size += coded_length;
	}

	//
	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)common.m_iFirstLoginDatetime);
	playercommon_size += coded_length;
	
	coded_length = encode_player_vipdata(pOut, common.m_stVipData);
	playercommon_size += coded_length;
	
	
	coded_length = CCodeEngine::encode_string(pOut, (const char*)common.m_szIDCard, sizeof(common.m_szIDCard));
	playercommon_size += coded_length;
	
	coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)common.m_nSex);
	playercommon_size += coded_length;
	
	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)common.m_stBirThday.m_iYear);
	playercommon_size += coded_length;
	
	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)common.m_stBirThday.m_iMonth);
	playercommon_size += coded_length;
	
	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)common.m_stBirThday.m_iDay);
	playercommon_size += coded_length;
	
	
	*pOut -= (playercommon_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pOut, playercommon_size);

	iOutLength = playercommon_size + coded_length;

	*pOut = ptmp + iOutLength; // 把指针指向下一个可用位置


	return iOutLength;
}

int32_t decode_player_common_info(char** pOut, stPlayerCommonInfo& common)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t playercommon_size = 0;
	int32_t iOutLength = 0;
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int16(pOut,(uint16_t*)&playercommon_size);
	iOutLength += decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iCoin);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iLoginCount);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iOfflineCount);
	iOutLength += decode_length;
	playercommon_size -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iCharming);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iAchievement);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int64(pOut, (uint64_t*)&common.m_lHappyBean);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iBlackLevel);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_string(pOut, common.m_szValidDate, (const int16_t)sizeof(common.m_szValidDate));
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iPunishMethod);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_string(pOut, common.m_szLastLoginDate, (const int16_t)sizeof(common.m_szLastLoginDate));
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iLastLoginIP);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_string(pOut, common.m_szDescString, (const int16_t)sizeof(common.m_szDescString));
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_string(pOut, common.m_szLastConsumeDate, (const int16_t)sizeof(common.m_szLastConsumeDate));
	iOutLength += decode_length;
	playercommon_size -= decode_length;


	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iMiscFlag);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iHappyBean_Daily_PresentedCount);
	iOutLength += decode_length;
	playercommon_size -= decode_length;


	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iHappyBean_Daily_LastPresentedTime);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&common.m_stWebQunData.m_nDataSize);
	iOutLength += decode_length;
	playercommon_size -= decode_length;
	
	if(common.m_stWebQunData.m_nDataSize>max_web_qun_data_size)
	{
		common.m_stWebQunData.m_nDataSize = 0;
	}
	else
	{
		decode_length = CCodeEngine::decode_memory(pOut,common.m_stWebQunData.m_szWebQunInfo ,common.m_stWebQunData.m_nDataSize);
		iOutLength += decode_length;
		playercommon_size -= decode_length;
	}

// 	decode_length = CCodeEngine::decode_int16(pOut,(uint16_t*)&common.m_nOtherCount);
// 	iOutLength += decode_length;
// 	playercommon_size -= decode_length;

	for (size_t i = 0; i < sizeof(common.m_aiOthers)/sizeof(common.m_aiOthers[0]); ++i)
	{
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_aiOthers[i]);
		iOutLength += decode_length;
		playercommon_size -= decode_length;
		
	}
	
	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&common.m_iFirstLoginDatetime);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	decode_length = decode_player_vipdata(pOut, common.m_stVipData);
	iOutLength += decode_length;
	playercommon_size -= decode_length;
	
	decode_length = CCodeEngine::decode_string(pOut, common.m_szIDCard, (const int16_t)sizeof(common.m_szIDCard));
	iOutLength += decode_length;
	playercommon_size -= decode_length;
	
	decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&common.m_nSex);
	iOutLength += decode_length;
	playercommon_size -= decode_length;
	
	decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&common.m_stBirThday.m_iYear);
	iOutLength += decode_length;
	playercommon_size -= decode_length;
	
	decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&common.m_stBirThday.m_iMonth);
	iOutLength += decode_length;
	playercommon_size -= decode_length;
	
	decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&common.m_stBirThday.m_iDay);
	iOutLength += decode_length;
	playercommon_size -= decode_length;

	if (playercommon_size < 0)
	{
		return fail;
	}
	else
	{
		//
		skip_byte_of(pOut, playercommon_size);
		iOutLength += playercommon_size;
	}

	return iOutLength;
}


int32_t encode_player_db_gamedata(char** pOut, stDBGameData& gamedata)
{
	//前2Byte存储player game data的实际大小

	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t gamedata_size = 0; //不包含自身的2Byte
	char* ptmp = *pOut;

	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间
	
	coded_length = CCodeEngine::encode_int16(pOut, gamedata.m_nGameID);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int8(pOut, gamedata.m_byLevel);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, gamedata.m_iPoint);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, gamedata.m_iWinRound);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, gamedata.m_iLoseRound);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, gamedata.m_iDrawRound);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, gamedata.m_iEscapeRound);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, gamedata.m_iOrgID);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int16(pOut, gamedata.m_nPosition);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, gamedata.m_uiTotalSecs);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_string(pOut, gamedata.m_szLastDate,sizeof(gamedata.m_szLastDate));
	gamedata_size += coded_length;

	coded_length = encode_TDBExtGameInfo(pOut, gamedata.m_stExtGameInfo);
	gamedata_size += coded_length;

	coded_length = encode_gameconfig(pOut, gamedata.m_stGameConfig);
	gamedata_size += coded_length;

	*pOut -= (gamedata_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pOut, gamedata_size);

	iOutLength = coded_length + gamedata_size;

	*pOut = ptmp + iOutLength;

	return iOutLength;
}

int32_t decode_player_db_gamedata(char** pOut, stDBGameData& gamedata)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t data_size = 0;
	int32_t iOutLength = 0;
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int16(pOut,(uint16_t*)&data_size);
	iOutLength += decode_length;

	decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&gamedata.m_nGameID);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&gamedata.m_byLevel);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&gamedata.m_iPoint);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&gamedata.m_iWinRound);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&gamedata.m_iLoseRound);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&gamedata.m_iDrawRound);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&gamedata.m_iEscapeRound);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&gamedata.m_iOrgID);
	iOutLength += decode_length;
	data_size -= decode_length;
	
	decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&gamedata.m_nPosition);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&gamedata.m_uiTotalSecs);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_string(pOut, gamedata.m_szLastDate,sizeof(gamedata.m_szLastDate));
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = decode_TDBExtGameInfo(pOut, gamedata.m_stExtGameInfo);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = decode_gameconfig(pOut, gamedata.m_stGameConfig);
	iOutLength += decode_length;
	data_size -= decode_length;

	if (data_size < 0)
	{
		return 0;
	}
	else
	{
		skip_byte_of(pOut, data_size);
		iOutLength += data_size;
	}

	return iOutLength;
}

int32_t encode_player_servicedata(char** pOut, stService& service)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t service_size = 0; //不包含自身的2Byte
	char* ptmp = *pOut;

	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

	coded_length = CCodeEngine::encode_int32(pOut, service.m_iServiceID);
	service_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, service.m_iExpireTime);
	service_size += coded_length;

	*pOut -= (service_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pOut, service_size);

	iOutLength = coded_length + service_size;

	*pOut = ptmp + iOutLength;

	return iOutLength;
}

int32_t decode_player_servicedata(char** pOut, stService& service)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t data_size = 0;
	int32_t iOutLength = 0;
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int16(pOut,(uint16_t*)&data_size);
	iOutLength += decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&service.m_iServiceID);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&service.m_iExpireTime);
	iOutLength += decode_length;
	data_size -= decode_length;

	if (data_size < 0)
	{
		return 0;
	}
	else
	{
		//
		skip_byte_of(pOut, data_size);
		iOutLength += data_size;
	}

	return iOutLength;
}
 

int32_t encode_player_vipdata(char** pOut, stVipData& vipData,int32_t *pBecomeVIPTime)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t vip_size = 0; //不包含自身的2Byte
	char* ptmp = *pOut;

	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

	coded_length = CCodeEngine::encode_int32(pOut, vipData.m_iGameVipExpireTime);
	vip_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, vipData.m_iGameVIPScore);
	vip_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, vipData.m_iGameVIPLevel);
	vip_size += coded_length;

	coded_length = CCodeEngine::encode_int32(pOut, vipData.m_iNextUpdateVIPLevelTime);
	vip_size += coded_length;

	coded_length = CCodeEngine::encode_string(pOut, vipData.m_szVIPTips,max_system_message_length);
	vip_size += coded_length;
	
	
	coded_length = CCodeEngine::encode_int32(pOut, vipData.m_iGameVIPType);
	vip_size += coded_length;

	if(pBecomeVIPTime != NULL)
	{
		coded_length = CCodeEngine::encode_int32(pOut, *pBecomeVIPTime);
		vip_size += coded_length;	
	}
	*pOut -= (vip_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pOut, vip_size);

	iOutLength = coded_length + vip_size;

	*pOut = ptmp + iOutLength;

	return iOutLength;
}


int32_t decode_player_vipdata(char** pOut, stVipData& vipData,int32_t *pBecomeVIPTime)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t vip_size = 0;
	int32_t iOutLength = 0;
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int16(pOut,(uint16_t*)&vip_size);
	iOutLength += decode_length;

	

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&vipData.m_iGameVipExpireTime);
	iOutLength += decode_length;
	vip_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&vipData.m_iGameVIPScore);
	iOutLength += decode_length;
	vip_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&vipData.m_iGameVIPLevel);
	iOutLength += decode_length;
	vip_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&vipData.m_iNextUpdateVIPLevelTime);
	iOutLength += decode_length;
	vip_size -= decode_length;


	decode_length = CCodeEngine::decode_string(pOut,vipData.m_szVIPTips,max_system_message_length);
	iOutLength += decode_length;
	vip_size -= decode_length;
	
	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&vipData.m_iGameVIPType);
	iOutLength += decode_length;
	vip_size -= decode_length;

	if(pBecomeVIPTime != NULL)
	{
		decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)pBecomeVIPTime);
		iOutLength += decode_length;
		vip_size -= decode_length;
	}
	if (vip_size < 0)
	{
		return 0;
	}
	else
	{
		//
		skip_byte_of(pOut, vip_size);
		iOutLength += vip_size;
	}

	return iOutLength;
}


//SS_MSG_RECORD_SHARED_MONEY
CRequestRecordSharedMoney::CRequestRecordSharedMoney()
{
	m_szGameTag[0] = '\0';
	//m_nEntryCount = 0;
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_iSelfShared = 0;
	m_iTotalShared = 0;
}

CRequestRecordSharedMoney::~CRequestRecordSharedMoney()
{

}

int32_t CRequestRecordSharedMoney::encode( char *pszOut, int32_t& iOutLength )
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;


	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szGameTag, (const int16_t)sizeof(m_szGameTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iSelfShared);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iTotalShared);
	iOutLength += encode_length;

// 	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nEntryCount);
// 	iOutLength += encode_length;
// 
// 	for (int32_t i = 0; i < m_nEntryCount; ++i)
// 	{
// 		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_astSharedMoney[i].m_iUin);
// 		iOutLength += encode_length;
// 
// 		encode_length = CCodeEngine::encode_string(&ptmp, m_astSharedMoney[i].m_szAccount, 
// 			(const int16_t)sizeof(m_astSharedMoney[i].m_szAccount));
// 		iOutLength += encode_length;
// 
// 		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_astSharedMoney[i].m_iSharedMoney);
// 		iOutLength += encode_length;
// 	}

	


	return success;
}

int32_t CRequestRecordSharedMoney::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szGameTag, (const int16_t)sizeof(m_szGameTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSelfShared);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTotalShared);
	remain_length -= decode_length;

// 	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nEntryCount);
// 	remain_length -= decode_length;
// 
// 	for (int32_t i = 0; i < m_nEntryCount; ++i)
// 	{
// 		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_astSharedMoney[i].m_iUin);
// 		remain_length -= decode_length;
// 
// 		decode_length = CCodeEngine::decode_string(&ptmp, m_astSharedMoney[i].m_szAccount, 
// 			(const int16_t)sizeof(m_astSharedMoney[i].m_szAccount));
// 		remain_length -= decode_length;
// 
// 		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_astSharedMoney[i].m_iSharedMoney);
// 		remain_length -= decode_length;
// 
// 	}
	
	if (remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CRequestRecordSharedMoney::dump()
{

}


//SS_MSG_GET_PLAYER_ALL_GAME_DATA
CRequestGetPlayerAllGameData::CRequestGetPlayerAllGameData()
{
	m_iUin = 0;
}

CRequestGetPlayerAllGameData::~CRequestGetPlayerAllGameData()
{

}

int32_t CRequestGetPlayerAllGameData::encode( char *pszOut, int32_t& iOutLength )
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;


	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	return success;
}

int32_t CRequestGetPlayerAllGameData::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	if (remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CRequestGetPlayerAllGameData::dump()
{

}

CResponseGetPlayerAllGameData::CResponseGetPlayerAllGameData()
{
	m_iResultId = result_id_success;
	m_iUin = 0;
	m_nGameDataCount  =0 ;
	memset(m_stGameData,0,sizeof(m_stGameData));
}

CResponseGetPlayerAllGameData::~CResponseGetPlayerAllGameData()
{

}

int32_t CResponseGetPlayerAllGameData::encode( char *pszOut, int32_t& iOutLength )
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;


	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iResultId);
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameDataCount);
	iOutLength += encode_length;

	if(m_iResultId == result_id_success )
	{
		for(int32_t i=0;i<m_nGameDataCount ;++i)
		{
			encode_length =encode_player_db_gamedata(&ptmp,m_stGameData[i]);
			iOutLength += encode_length;
		}
	}
	return success;
}

int32_t CResponseGetPlayerAllGameData::decode( const char *pszIn, const int32_t iInLength )
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iResultId);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameDataCount);
	remain_length -= decode_length;

	if(m_iResultId == result_id_success )
	{
		for(int32_t i=0;i<m_nGameDataCount ;++i)
		{
			decode_length =decode_player_db_gamedata(&ptmp,m_stGameData[i]);
			remain_length -= decode_length;
		}
	}

	if (remain_length < 0)
	{
		return fail;
	}
	return success;
}

void CResponseGetPlayerAllGameData::dump()
{

}

//////////////////////////////////////////////////////////////////////////


int32_t encode_ExtGameDataUpdate(char** pOut, ExtGameDataUpdate& data)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t gamedata_size = 0; //不包含自身的2Byte
	char* ptmp = *pOut;

	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)data.m_nExtIndex);
	gamedata_size += coded_length;

	coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_nUpdateMode);
	gamedata_size += coded_length;
 

	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_ExtVal);
	gamedata_size += coded_length;

 
	*pOut -= (gamedata_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pOut, gamedata_size);

	iOutLength = coded_length + gamedata_size;

	*pOut = ptmp + iOutLength;

	return iOutLength;
}

int32_t decode_ExtGameDataUpdate(char** pszIn, ExtGameDataUpdate& data)
{
	if (*pszIn == NULL || pszIn == NULL)
	{
		return 0;
	}

	int16_t data_size = 0;
	int32_t iOutLength = 0;
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int16(pszIn,(uint16_t*)&data_size);
	iOutLength += decode_length;

	decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&data.m_nExtIndex);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int8(pszIn, (uint8_t*)&data.m_nUpdateMode);
	iOutLength += decode_length;
	data_size -= decode_length;

	decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&data.m_ExtVal);
	iOutLength += decode_length;
	data_size -= decode_length;


	if (data_size < 0)
	{
		return 0;
	}
	else
	{
		//
		skip_byte_of(pszIn, data_size);
		iOutLength += data_size;
	}
	return iOutLength;
}

int32_t encode_TDBExtGameInfo(char** pOut, TDBExtGameInfo& data)
{
	if (*pOut == NULL || pOut == NULL)
	{
		return 0;
	}

	int16_t gamedata_size = 0; //不包含自身的2Byte
	char* ptmp = *pOut;

	int32_t iOutLength = 0;
	int32_t coded_length = 0;

	*pOut += sizeof(int16_t); //为元素的长度预留2Byte的空间
	for(int16_t i=0;i<max_game_ext_int_count;++i)
	{
		coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_aryExtInt[i]);
		gamedata_size += coded_length;
	}
	
	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)data.m_nExtDataSize);
	gamedata_size += coded_length;

	if(data.m_nExtDataSize > 0)
	{
		coded_length = CCodeEngine::encode_memory(pOut,data.m_szExtData,data.m_nExtDataSize);
		gamedata_size += coded_length;
	}

	*pOut -= (gamedata_size + sizeof(int16_t));
	coded_length = CCodeEngine::encode_int16(pOut, gamedata_size);

	iOutLength = coded_length + gamedata_size;

	*pOut = ptmp + iOutLength;

	return iOutLength;
}

int32_t decode_TDBExtGameInfo(char** pszIn, TDBExtGameInfo& data)
{
	if (*pszIn == NULL || pszIn == NULL)
	{
		return 0;
	}

	int16_t data_size = 0;
	int32_t iOutLength = 0;
	int32_t decode_length = 0;

	decode_length = CCodeEngine::decode_int16(pszIn,(uint16_t*)&data_size);
	iOutLength += decode_length;
	for(int32_t i=0;i<max_game_ext_int_count && data_size>0;++i)
	{
		decode_length = CCodeEngine::decode_int32(pszIn, (uint32_t*)&data.m_aryExtInt[i]);
		iOutLength += decode_length;
		data_size -= decode_length;
	}
	
	if(data_size<0)
	{
		return 0;
	}

	decode_length = CCodeEngine::decode_int16(pszIn, (uint16_t*)&data.m_nExtDataSize);
	iOutLength += decode_length;
	data_size -= decode_length;


	if(data.m_nExtDataSize>0)
	{
		decode_length = CCodeEngine::decode_memory(pszIn, data.m_szExtData,data.m_nExtDataSize);
		iOutLength += decode_length;
		data_size -= decode_length;
	}
	

	if (data_size < 0)
	{
		return fail;
	}
	else
	{
		//
		skip_byte_of(pszIn, data_size);
		iOutLength += data_size;
	}
	return iOutLength;
	 
}


CRequestUpdateGameExtInfo::CRequestUpdateGameExtInfo()
{
	m_iUin = 0;
	m_nGameID = 0;
	m_nUpdateIntCount = 0;
	memset(m_astUpdateGameData,0,sizeof(m_astUpdateGameData));
	m_nExtDataSize  = 0;
	//memset(m_szExtData,0,sizeof(m_szExtData)); //deleted by aprilliu, 2008-11-02
	m_nTransparentDataSize = 0;
	//memset(m_TransparentData,0,sizeof(m_TransparentData));  //deleted by aprilliu, 2008-11-02
}
CRequestUpdateGameExtInfo::~CRequestUpdateGameExtInfo()
{

}


int32_t  CRequestUpdateGameExtInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nExtDataSize > max_game_ext_data_size)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nUpdateIntCount);
	iOutLength += encode_length;

	for(int32_t i=0;i<m_nUpdateIntCount && i<max_game_ext_int_count;++i)
	{
		encode_length =encode_ExtGameDataUpdate(&ptmp,m_astUpdateGameData[i]);
		iOutLength += encode_length;
	}

	encode_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)this->m_nExtDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_szExtData,m_nExtDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_TransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CRequestUpdateGameExtInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nUpdateIntCount);
	remain_length -= decode_length;

	for(int32_t i=0;i<m_nUpdateIntCount && i<max_game_ext_int_count;++i)
	{
		decode_length =decode_ExtGameDataUpdate(&ptmp,m_astUpdateGameData[i]);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nExtDataSize);
	remain_length -= decode_length;

	if (m_nExtDataSize > max_game_ext_data_size)
	{
		return fail;
	}

	if(m_nExtDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp,m_szExtData,m_nExtDataSize);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	if(m_nTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp,m_TransparentData,m_nTransparentDataSize);
		remain_length -= decode_length;
	}

	if (remain_length < 0)
	{
		return fail;
	}
	return success;

}

void CRequestUpdateGameExtInfo::dump()
{

}

CResponseUpdateGameExtInfo::CResponseUpdateGameExtInfo()
{
	m_iUin = 0;
	m_nGameID = 0;
	m_nResultID = 0;
	memset(&m_stCurrentExtGameInfo,0,sizeof(m_stCurrentExtGameInfo));
	m_nTransparentDataSize = 0;
}

CResponseUpdateGameExtInfo::~CResponseUpdateGameExtInfo()
{

}

int32_t  CResponseUpdateGameExtInfo::encode(char *pszOut, int32_t& iOutLength)  
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	if(success == m_nResultID)
	{
		encode_length = encode_TDBExtGameInfo(&ptmp,this->m_stCurrentExtGameInfo);
		iOutLength += encode_length;
	}

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp,m_TransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CResponseUpdateGameExtInfo::decode(const char *pszIn, const int32_t iInLength) 
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;


	if(m_nResultID==success)
	{
		decode_length = decode_TDBExtGameInfo(&ptmp,this->m_stCurrentExtGameInfo);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	if(m_nTransparentDataSize > 0)
	{
		decode_length = CCodeEngine::decode_memory(&ptmp,m_TransparentData,m_nTransparentDataSize);
		remain_length -= decode_length;
	}

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CResponseUpdateGameExtInfo::dump()  
{

}

CNotifyRefreshGameExtInfo::CNotifyRefreshGameExtInfo()
{
	m_iUin = 0;
	m_nGameID = 0;
	memset(&m_stCurrentExtGameInfo,0,sizeof(m_stCurrentExtGameInfo));

}

CNotifyRefreshGameExtInfo::~CNotifyRefreshGameExtInfo()
{


}

int32_t  CNotifyRefreshGameExtInfo::encode(char *pszOut, int32_t& iOutLength) 
{

	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	iOutLength += encode_length;

	encode_length = encode_TDBExtGameInfo(&ptmp,this->m_stCurrentExtGameInfo);
	iOutLength += encode_length;
	 
	return success;
}

int32_t  CNotifyRefreshGameExtInfo::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	remain_length -= decode_length;

	decode_length = decode_TDBExtGameInfo(&ptmp,this->m_stCurrentExtGameInfo);
	remain_length -= decode_length;
	 
	if(remain_length<0)
	{
		return fail;
	}

	return success;

}

void CNotifyRefreshGameExtInfo::dump()
{


}

CRequestUpdatePlayerCardID::CRequestUpdatePlayerCardID()
{
		m_iUin = 0;
		m_szAccount[0]='\0';
		m_szIDCard[0]='\0';
		m_szUserName[0]='\0';
}

CRequestUpdatePlayerCardID::~CRequestUpdatePlayerCardID()
{


}

CRequestUpdatePlayerCardID& CRequestUpdatePlayerCardID::operator =(const CRequestUpdatePlayerCardID& original)
{
	if (this == &original)
	{
		return *this;
	}

	m_iUin = original.m_iUin;
	
	STRNCPY(m_szAccount, original.m_szAccount, sizeof(m_szAccount));
	
	STRNCPY(m_szIDCard, original.m_szIDCard, sizeof(m_szIDCard));

	STRNCPY(m_szUserName, original.m_szUserName, sizeof(m_szUserName));
	
	return *this;
}

int32_t  CRequestUpdatePlayerCardID::encode(char *pszOut, int32_t& iOutLength) 
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szIDCard,sizeof(m_szIDCard));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szUserName,sizeof(m_szUserName));
	iOutLength += encode_length;
	return success;
}

int32_t  CRequestUpdatePlayerCardID::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szIDCard,sizeof(m_szIDCard));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szUserName,sizeof(m_szUserName));
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CRequestUpdatePlayerCardID::dump()
{


}

CResponseUpdatePlayerCardID::CResponseUpdatePlayerCardID()
{
	m_iUin = 0;
	memset(m_szAccount,0,sizeof(m_szAccount));
	m_nResultID =success;
}

CResponseUpdatePlayerCardID::~CResponseUpdatePlayerCardID()
{
	
}

CResponseUpdatePlayerCardID& CResponseUpdatePlayerCardID::operator =(const CResponseUpdatePlayerCardID& original)
{
	if (this == &original)
	{
		return *this;
	}

	m_iUin = original.m_iUin;

	STRNCPY(m_szAccount, original.m_szAccount, sizeof(m_szAccount));

	m_nResultID = original.m_nResultID;

	return *this;
}

int32_t  CResponseUpdatePlayerCardID::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	return success;
}

int32_t  CResponseUpdatePlayerCardID::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CResponseUpdatePlayerCardID::dump()
{


}

CRequestUpdatePlayerEnthralmentData::CRequestUpdatePlayerEnthralmentData()
{
	m_iUin = 0;
	m_iCumulativeOnLine = 0 ;
	m_iCumulativeOffLine = 0 ;	
}

CRequestUpdatePlayerEnthralmentData::~CRequestUpdatePlayerEnthralmentData()
{


}

int32_t  CRequestUpdatePlayerEnthralmentData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iCumulativeOnLine);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iCumulativeOffLine);
	iOutLength += encode_length;

	return success;
}

int32_t  CRequestUpdatePlayerEnthralmentData::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCumulativeOnLine);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCumulativeOffLine);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CRequestUpdatePlayerEnthralmentData::dump()
{

}



CResponseUpdatePlayerEnthralmentData::CResponseUpdatePlayerEnthralmentData()
{

	  m_iUin = 0;
	  m_nResultID = 0;      

}

CResponseUpdatePlayerEnthralmentData::~CResponseUpdatePlayerEnthralmentData()
{


}

int32_t  CResponseUpdatePlayerEnthralmentData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;


	return success;

}

int32_t  CResponseUpdatePlayerEnthralmentData::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	if(remain_length<0)
	{
		return fail;
	}
	return success;

}

void CResponseUpdatePlayerEnthralmentData::dump()
{

}



CRequestGetPlayerEnthralmentData::CRequestGetPlayerEnthralmentData()
{
		m_iUin = 0;

}

CRequestGetPlayerEnthralmentData::~CRequestGetPlayerEnthralmentData()
{


}

int32_t  CRequestGetPlayerEnthralmentData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;


	return success;

}

int32_t  CRequestGetPlayerEnthralmentData::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;
	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CRequestGetPlayerEnthralmentData::dump()
{



}

CResponseGetPlayerEnthralmentData::CResponseGetPlayerEnthralmentData()
{
	m_iUin = 0;
	m_nResultID = 0;  
	memset(m_szIDCard,0,sizeof(m_szIDCard));				//身份证号
	m_iCumulativeOnLine = 0;	//累计在线时长
	m_iCumulativeOffLine = 0;	//累计下线时长
	m_tmLastOffline = 0;		//上一次下线时间
	 
}

CResponseGetPlayerEnthralmentData::~CResponseGetPlayerEnthralmentData()
{
	 
}

int32_t  CResponseGetPlayerEnthralmentData::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	if(m_nResultID == success)
	{
		encode_length = CCodeEngine::encode_string(&ptmp, m_szIDCard,sizeof(m_szIDCard));
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iCumulativeOnLine);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iCumulativeOffLine);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_tmLastOffline);
		iOutLength += encode_length;
	}

	return success;
}

int32_t  CResponseGetPlayerEnthralmentData::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;


	if(m_nResultID == success)
	{
		decode_length = CCodeEngine::decode_string(&ptmp, m_szIDCard,sizeof(m_szIDCard));
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCumulativeOnLine);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iCumulativeOffLine);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_tmLastOffline);
		remain_length -= decode_length;

	}

	if(remain_length<0)
	{
		return fail;
	}
	return success;
}

void CResponseGetPlayerEnthralmentData::dump()
{


}

CNotifyPlayerAccountChannge::CNotifyPlayerAccountChannge()
{
	//m_iUin = 0;
	//memset(m_szAccount,0,sizeof(m_szAccount));
	//m_iLastHour_Consume = 0; 
	//m_iLastDay_Consume = 0; 
	//m_iLastConsumeTime = 0; 
	//m_iLastConsumeTime ＝ 0;//最近的一次消费时间
	//m_iLastConsume ＝ 0;//最近的一次消费金额
	//m_iConsumeType ＝ 0;//消费(服务)类型


	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_iChangeTime = 0 ;
	m_iChangeMoney = 0;
	m_iServiceType = 0;
	m_szTransTag[0] = '\0';
	m_iSelfShared = 0;

	m_stStatDataCount = 0;
	memset(&m_aryStatData[0],0,sizeof(m_aryStatData));
}

CNotifyPlayerAccountChannge::~CNotifyPlayerAccountChannge()
{

}

 
int32_t  CNotifyPlayerAccountChannge::  encode(char *pszOut, int32_t& iOutLength) 
{

	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChangeTime);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChangeMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iServiceType);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iSelfShared);
	iOutLength += encode_length;

	
	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stStatDataCount);
	iOutLength += encode_length;
	 
	for(int32_t x=0;x<m_stStatDataCount;++x)
	{
		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_aryStatData[x].nStatIndex);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_aryStatData[x].nStatValue);
		iOutLength += encode_length;
	}
    return success;
}

int32_t  CNotifyPlayerAccountChannge::decode(const char *pszIn, const int32_t iInLength) 
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChangeTime);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChangeMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServiceType);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iSelfShared);
	remain_length += decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stStatDataCount);
	remain_length -= decode_length;

    if(m_stStatDataCount>enum_StatMaxSize)
		 m_stStatDataCount=enum_StatMaxSize;

	for(int32_t x=0;x<m_stStatDataCount;++x)
	{
		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_aryStatData[x].nStatIndex);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aryStatData[x].nStatValue);
		remain_length -= decode_length;
	}

	if(remain_length<0)
	{
		return fail;
	}
	return success;

}
void CNotifyPlayerAccountChannge::dump()
{

}


/////////////////////////////
CRequestPlayerCharge::CRequestPlayerCharge()
{
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_iOperUin = 0;
	m_szOperAccount[0] = '\0';
	m_szOrderID[0] = '\0';//
	m_iChannelId= 0;
	m_iChannelAmout = 0;
	m_iPayAmount = 0;
	m_iReqTimestamp = 0;
	m_szTransTag[0] = '\0';
	m_nTransparentDataSize = 0 ;
	memset(m_TransparentData,0,sizeof(m_TransparentData));
}
CRequestPlayerCharge::~CRequestPlayerCharge()
{


}

int32_t  CRequestPlayerCharge::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iOperUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChannelId);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChannelAmout);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPayAmount);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iReqTimestamp);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;
	 	
	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CRequestPlayerCharge::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iOperUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChannelId);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChannelAmout);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayAmount);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iReqTimestamp);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	remain_length -= decode_length;

   if(remain_length < 0)
   {
		return fail;
   }
   return success;
}

void CRequestPlayerCharge::dump()
{

}


CResponsePlayerCharge::CResponsePlayerCharge()
{
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_iOperUin = 0;
	m_szOperAccount[0] = '\0';
	m_szOrderID[0] = '\0';
	m_iChannelId = 0;
	m_iChannelAmout = 0;
	m_iPayAmount = 0;
	m_iReqTimestamp = 0;
	m_szTransTag[0] = '\0';
	m_iResTimestamp = 0;
	m_iResultID = 0;
	m_iMoney = 0;
	m_nTransparentDataSize = 0 ;
	memset(m_TransparentData,0,sizeof(m_TransparentData));

}

CResponsePlayerCharge::~CResponsePlayerCharge()
{


}

int32_t  CResponsePlayerCharge::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iOperUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChannelId);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iChannelAmout);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPayAmount);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iReqTimestamp);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iResTimestamp);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iResultID);
	iOutLength += encode_length;
	
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;
	return success;
}

int32_t  CResponsePlayerCharge::decode(const char *pszIn, const int32_t iInLength) 
{

	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iOperUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChannelId);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iChannelAmout);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayAmount);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iReqTimestamp);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iResTimestamp);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if(m_nTransparentDataSize> max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}
void CResponsePlayerCharge::dump()
{


}


/////////////////// CRequestPlayerExchange 
CRequestPlayerExchange::CRequestPlayerExchange()
{
	m_iUin = 0;
	m_szAccount[0]='\0';
	m_iOperUin = 0;
	m_szOperAccount[0] = '\0';
	m_szOrderID[0] = '\0';
	m_iExcid = 0;
	m_iPayAmount = 0;
	m_iReqTimestamp = 0;
	m_szTransTag[0] = '\0';
	m_nTransparentDataSize = 0 ;
	memset(m_TransparentData,0,sizeof(m_TransparentData));
}


CRequestPlayerExchange::~CRequestPlayerExchange()
{


}
int32_t  CRequestPlayerExchange::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if(m_nTransparentDataSize> max_transparent_data_size)
	{
		return  fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;

	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iOperUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	iOutLength += encode_length;


	encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iExcid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPayAmount);
	iOutLength += encode_length;

	 
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iReqTimestamp);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;
	


	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;
	
	return success;
}

int32_t  CRequestPlayerExchange::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iOperUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iExcid);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayAmount);
	remain_length -= decode_length;

 

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iReqTimestamp);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remain_length -= decode_length;

	

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CRequestPlayerExchange::dump()
{


}


//CResponsePlayerExchange
CResponsePlayerExchange::CResponsePlayerExchange()
{
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_iOperUin = 0;
	m_szOperAccount[0] = '\0';
	m_szOrderID[0] = '\0';
	m_iExcid = 0;
	m_iPayAmount = 0;
	m_iReqTimestamp = 0;
	m_szTransTag[0] = '\0';
	m_iResTimestamp = 0;
	m_iResultID = 0;
	m_iMoney  = 0;
	m_nTransparentDataSize = 0 ;
	memset(m_TransparentData,0,sizeof(m_TransparentData));
}

CResponsePlayerExchange::~CResponsePlayerExchange()
{


}

int32_t  CResponsePlayerExchange::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}


	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iOperUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iExcid);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPayAmount);
	iOutLength += encode_length;
 
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iReqTimestamp);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;
	 
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iResTimestamp);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMoney);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CResponsePlayerExchange::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iOperUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szOperAccount,sizeof(m_szOperAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szOrderID,sizeof(m_szOrderID));
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iExcid);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPayAmount);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iReqTimestamp);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iResTimestamp);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoney);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if(m_nTransparentDataSize>max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	remain_length -= decode_length;

	if(remain_length < 0)
	{
		return fail;
	}
	return success;

}

void CResponsePlayerExchange::dump()
{


}

CResponseQueryPlatformMoneyLockInfo::CResponseQueryPlatformMoneyLockInfo()
{
		m_iLockType=0;
}

CResponseQueryPlatformMoneyLockInfo::~CResponseQueryPlatformMoneyLockInfo()
{

}

int32_t  CResponseQueryPlatformMoneyLockInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLockType);
	iOutLength += encode_length;

	return success;
}

int32_t  CResponseQueryPlatformMoneyLockInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLockType);
	remain_length -= decode_length;

	return success;
}
void CResponseQueryPlatformMoneyLockInfo::dump()
{

}


/////////////////////////////////////////////////
CRequestUpdatePlatformMoneyLockInfo::CRequestUpdatePlatformMoneyLockInfo()
{
		m_iUpdateMode = 0;
		m_iLockType=0;
}

CRequestUpdatePlatformMoneyLockInfo::~CRequestUpdatePlatformMoneyLockInfo()
{

}

int32_t  CRequestUpdatePlatformMoneyLockInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_iUpdateMode);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLockType);
	iOutLength += encode_length;

	return success;
}
int32_t  CRequestUpdatePlatformMoneyLockInfo::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_iUpdateMode);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLockType);
	remain_length -= decode_length;

	return success;
}
void CRequestUpdatePlatformMoneyLockInfo::dump()
{

}

CResponseUpdatePlatformMoneyLockInfo::CResponseUpdatePlatformMoneyLockInfo()
{
	m_nResultID =0; 
	m_iLockType=0;
}

CResponseUpdatePlatformMoneyLockInfo::~CResponseUpdatePlatformMoneyLockInfo()
{

}

int32_t  CResponseUpdatePlatformMoneyLockInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLockType);
	iOutLength += encode_length;

	return success;
}
int32_t  CResponseUpdatePlatformMoneyLockInfo::decode(const char *pszIn, const int32_t iInLength)
{
if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;
	
	if(m_nResultID == result_id_success)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLockType);
		remain_length -= decode_length;
	}

	return success;
}
void CResponseUpdatePlatformMoneyLockInfo::dump()
{
	
}

///////////////////////////////////////////////////////
CRequestRegisterService::CRequestRegisterService()
{
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_iServiceNumber = 0;
	memset(m_aryServiceRegisterInfo,0,sizeof(m_aryServiceRegisterInfo));

	m_szTransTag[0] = '\0';	
	m_nTransparentDataSize = 0;
	m_nChannelId = 0;
}

CRequestRegisterService::~CRequestRegisterService()
{


}

int32_t  CRequestRegisterService::encode(char *pszOut, int32_t& iOutLength) 
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iServiceNumber);
	iOutLength += encode_length;

     for(int32_t x=0;x<m_iServiceNumber;++x)
	 {
		 encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_aryServiceRegisterInfo[x].m_iServiceID);
		 iOutLength += encode_length;

		 encode_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_aryServiceRegisterInfo[x].m_iTimeType);
		 iOutLength += encode_length;

		 encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_aryServiceRegisterInfo[x].m_iServiceTime);
		 iOutLength += encode_length;
	 }

	encode_length = CCodeEngine::encode_string(&ptmp,m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;

	
	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;
	 
	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	iOutLength += encode_length;
	 
	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nChannelId);
	iOutLength += encode_length;
	return success;
}

int32_t  CRequestRegisterService::decode(const char *pszIn, const int32_t iInLength) 
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iServiceNumber);
	remain_length -= decode_length;

	if(m_iServiceNumber> max_request_register_service_number)
	{	
		m_iServiceNumber = max_request_register_service_number;
	}
	for(int32_t x=0;x<m_iServiceNumber ;++x)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aryServiceRegisterInfo[x].m_iServiceID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_aryServiceRegisterInfo[x].m_iTimeType);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aryServiceRegisterInfo[x].m_iServiceTime);
		remain_length -= decode_length;
	}

	decode_length = CCodeEngine::decode_string(&ptmp,m_szTransTag,(const int16_t)sizeof(m_szTransTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;


	if(m_nTransparentDataSize>max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,(const int32_t)m_nTransparentDataSize);
	remain_length -= decode_length;


	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nChannelId);
	remain_length -= decode_length;
	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}
void CRequestRegisterService::dump()
{


}

CResponseRegisterService::CResponseRegisterService()
{
	m_nResultID = success;
	m_iUin = 0;
	m_szAccount[0] = '\0';
	memset(&m_stServiceDataChg,0,sizeof(m_stServiceDataChg));

	m_szTransTag[0] = '\0';	
	m_nTransparentDataSize = 0;
	
	m_nChannelId =0;
}

CResponseRegisterService::~CResponseRegisterService()
{

}

int32_t  CResponseRegisterService::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	 if(m_nResultID == success)
	 {
			encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stServiceDataChg.m_nCount);
		    iOutLength += encode_length;
			for(int32_t x=0;x<m_stServiceDataChg.m_nCount;++x)
			{
				encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceDataChg.m_stService[x].m_iServiceID);
				iOutLength += encode_length;

				encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceDataChg.m_stService[x].m_iTimeChg);
				iOutLength += encode_length;

				encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceDataChg.m_stService[x].m_iExpireTime);
				iOutLength += encode_length;
			}
	 }

	 encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	 iOutLength += encode_length;

	 encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	 iOutLength += encode_length;

	 encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	 iOutLength += encode_length;
	 
	 
	 //encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nChannelId);
	 //iOutLength += encode_length;
	 return success;
}

int32_t  CResponseRegisterService::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp,m_szAccount,sizeof(m_szAccount));
	remain_length -= decode_length;

	if(m_nResultID == success)
	{
		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stServiceDataChg.m_nCount);
		remain_length -= decode_length;

		if( m_stServiceDataChg.m_nCount > max_request_register_service_number )
		{
			m_stServiceDataChg.m_nCount = max_request_register_service_number ;
		}
		for(int32_t x=0;x<m_stServiceDataChg.m_nCount;++x)
		{
			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceDataChg.m_stService[x].m_iServiceID);
			remain_length -= decode_length;

			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceDataChg.m_stService[x].m_iTimeChg);
			remain_length -= decode_length;

			decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceDataChg.m_stService[x].m_iExpireTime);
			remain_length -= decode_length;
		}
	}

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remain_length -= decode_length;

	if(m_nTransparentDataSize>max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	remain_length -= decode_length;

	//decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nChannelId);
	//remain_length -= decode_length;
	 
	if(remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CResponseRegisterService::dump()
{


}



CNotifyRefreshService::CNotifyRefreshService()
{
	m_iUin = 0;
	memset(&m_stServiceData,0,sizeof(m_stServiceData)); 

}

CNotifyRefreshService::~CNotifyRefreshService()
{


}

int32_t  CNotifyRefreshService::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stServiceData.m_nCount);
	iOutLength += encode_length;

	for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
	{
		encode_length = encode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
		iOutLength += encode_length;
	}

	return success;
}

int32_t  CNotifyRefreshService::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stServiceData.m_nCount);
	remain_length -= decode_length;

	for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
	{
		decode_length = decode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
		remain_length -= decode_length;
	}

	if (remain_length < 0)
	{
		return fail;
	}

	return success;
}

void CNotifyRefreshService::dump()
{



}


/////////////////////////////

CRequestGetUserDetailInfo::CRequestGetUserDetailInfo()
{
		m_iUin = 0;

}
CRequestGetUserDetailInfo::~CRequestGetUserDetailInfo()
{


}

int32_t  CRequestGetUserDetailInfo::encode(char *pszOut, int32_t& iOutLength)
{

	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	return success;

}

int32_t  CRequestGetUserDetailInfo::decode(const char *pszIn, const int32_t iInLength) 
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remain_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remain_length -= decode_length;

	return success;

}
void CRequestGetUserDetailInfo::dump()
{


}


CResponseGetUserDetailInfo::CResponseGetUserDetailInfo()
{
	m_nResultID = result_id_success;
	m_iUin = 0;
	memset(&m_stBaseInfo,0,sizeof(m_stBaseInfo));
	memset(&m_stServiceData,0,sizeof(m_stServiceData));
	m_nLockType = -1;	
	m_iGameID  = 0;
	m_iLockedServerType = -1;
	m_iLockedServerID = -1;			 
	m_iLockedRoomID = invalid_object_id;			 
	m_iLockedTableID = invalid_object_id;
	m_nLockMoney = 0 ;


	m_nHappyBeanLockType = -1;
	m_iHappyBeanGameID = 0;
	m_iHappyBeanLockedServerType = -1;		//当前占有游戏币锁的服务器type
	m_iHappyBeanLockedServerID = -1;			//当前占有游戏币锁的服务器id
	m_iHappyBeanLockedRoomID =invalid_object_id;			//当前占有游戏币锁的房间id
	m_iHappyBeanLockedTableID =invalid_object_id;			//当前占有游戏币锁的游戏桌id
	return ;
}

CResponseGetUserDetailInfo::~CResponseGetUserDetailInfo()
{



}

int32_t  CResponseGetUserDetailInfo::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;


	if(m_nResultID == result_id_success)
	{
		encode_length = encode_player_common_info(&ptmp, m_stBaseInfo);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stServiceData.m_nCount);
		iOutLength += encode_length;

		for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
		{
			encode_length = encode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
			iOutLength += encode_length;
		}


		encode_length = CCodeEngine::encode_int16(&ptmp,m_nLockType);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp,m_iGameID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp,m_iLockedServerType);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_iLockedServerID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_iLockedRoomID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_iLockedTableID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, m_nLockMoney);
		iOutLength += encode_length;


		//happybean lock
		encode_length = CCodeEngine::encode_int16(&ptmp,m_nHappyBeanLockType);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp,m_iHappyBeanGameID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int16(&ptmp,m_iHappyBeanLockedServerType);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_iHappyBeanLockedServerID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_iHappyBeanLockedRoomID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp,m_iHappyBeanLockedTableID);
		iOutLength += encode_length;
	}

	return success;

}

int32_t  CResponseGetUserDetailInfo::decode(const char *pszIn, const int32_t iInLength)
{

	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remained_length = iInLength;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decode_length;


	if(m_nResultID == result_id_success)
	{
		decode_length = decode_player_common_info(&ptmp, m_stBaseInfo);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stServiceData.m_nCount);
		remained_length -= decode_length;

		for (int32_t i = 0; i < m_stServiceData.m_nCount; ++i)
		{
			decode_length = decode_player_servicedata(&ptmp, m_stServiceData.m_stService[i]);
			remained_length -= decode_length;
		}


		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nLockType);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_iGameID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_iLockedServerType);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLockedServerID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLockedRoomID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iLockedTableID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nLockMoney);
		remained_length -= decode_length;


		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nHappyBeanLockType);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_iHappyBeanGameID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_iHappyBeanLockedServerType);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iHappyBeanLockedServerID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iHappyBeanLockedRoomID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp,(uint32_t*)&m_iHappyBeanLockedTableID);
		remained_length -= decode_length;

	}

	if (remained_length < 0)
	{
		return fail;
	}
	return success;
}
void CResponseGetUserDetailInfo::dump()
{


}


CRequestCheckUserIsExist::CRequestCheckUserIsExist()
{
		m_iUin  = 0;
		m_nTransparentDataSize = 0;
		memset(m_TransparentData,0,max_transparent_data_size);

}
CRequestCheckUserIsExist::~CRequestCheckUserIsExist()
{


}

int32_t  CRequestCheckUserIsExist::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	iOutLength += encode_length;
	return success;
}
int32_t  CRequestCheckUserIsExist::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remained_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remained_length -= decode_length;

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	remained_length -= decode_length;

	if(remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CRequestCheckUserIsExist::dump()
{


}


CResponseCheckUserIsExist::CResponseCheckUserIsExist()
{

	m_iUin = 0;
	m_cIsExist = 0;
	m_nTransparentDataSize = 0;
	memset(m_TransparentData,0,max_transparent_data_size);
}
CResponseCheckUserIsExist::~CResponseCheckUserIsExist()
{


}

int32_t CResponseCheckUserIsExist::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cIsExist);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	iOutLength += encode_length;
	return success;
}

int32_t  CResponseCheckUserIsExist::decode(const char *pszIn, const int32_t iInLength)
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remained_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cIsExist);
	remained_length -= decode_length;


	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remained_length -= decode_length;

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	remained_length -= decode_length;

	if (remained_length < 0)
	{
		return fail;
	}

	return success;

}
void CResponseCheckUserIsExist::dump()
{


}

CRequestRenewService::CRequestRenewService()
{
	m_iUin = 0;
	m_szAccount[0] = '\0';

	m_nRenewPrice = 0;
	memset(&m_stServiceRegisterInfo,0,sizeof(m_stServiceRegisterInfo));
	m_szTransTag[0] = '\0';	
	m_nTransparentDataSize = 0;
	memset(m_TransparentData,0,max_transparent_data_size);
}

CRequestRenewService::~CRequestRenewService()
{


}

int32_t  CRequestRenewService::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nRenewPrice);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceRegisterInfo.m_iServiceID);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_stServiceRegisterInfo.m_iTimeType);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceRegisterInfo.m_iServiceTime);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	return success;
}

int32_t  CRequestRenewService::decode(const char *pszIn, const int32_t iInLength) 
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remained_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nRenewPrice);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceRegisterInfo.m_iServiceID);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stServiceRegisterInfo.m_iTimeType);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceRegisterInfo.m_iServiceTime);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remained_length -= decode_length;

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,m_nTransparentDataSize);
	remained_length -= decode_length;

	if(remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CRequestRenewService::dump()
{


}


CResponseRenewService::CResponseRenewService()
{
	m_nResultID = 0;
	m_iUin = 0;
	m_szAccount[0] = '\0';
	m_nCoin = 0 ;
	memset(&m_stServiceChg,0,sizeof(m_stServiceChg));
	m_szTransTag[0] = '\0';	
	m_nTransparentDataSize = 0;
	memset(m_TransparentData,0,max_transparent_data_size);
}

CResponseRenewService::~CResponseRenewService()
{

}

int32_t  CResponseRenewService::encode(char *pszOut, int32_t& iOutLength)
{
	if(NULL == pszOut)
	{
		return fail;
	}

	if (m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	int16_t encode_length = 0;
	char* ptmp = pszOut;
	iOutLength = 0;

	encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += encode_length;

	if(m_nResultID  == result_id_success)
	{
		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nCoin);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceChg.m_iServiceID);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceChg.m_iTimeChg);
		iOutLength += encode_length;

		encode_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_stServiceChg.m_iExpireTime);
		iOutLength += encode_length;

	}

	encode_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	encode_length = CCodeEngine::encode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	iOutLength += encode_length;

	return success;

}

int32_t  CResponseRenewService::decode(const char *pszIn, const int32_t iInLength) 
{
	if(NULL == pszIn || iInLength < 0)
	{
		return fail;
	}
	char* ptmp = const_cast<char*>(pszIn);
	int32_t decode_length = 0;
	int32_t remained_length = iInLength;

	decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_string(&ptmp, m_szAccount,sizeof(m_szAccount));
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remained_length -= decode_length;

	if(m_nResultID  == result_id_success)
	{
		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nCoin);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceChg.m_iServiceID);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceChg.m_iTimeChg);
		remained_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stServiceChg.m_iExpireTime);
		remained_length -= decode_length;

	}

	decode_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,sizeof(m_szTransTag));
	remained_length -= decode_length;

	decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	remained_length -= decode_length;

	if(m_nTransparentDataSize > max_transparent_data_size)
	{
		return fail;
	}

	decode_length = CCodeEngine::decode_memory(&ptmp, m_TransparentData,(int32_t)m_nTransparentDataSize);
	remained_length -= decode_length;

	if(remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CResponseRenewService::dump()
{



}

//SS_MSG_PRESENT_HAPPY_BEAN 
CSSRequestPresentHappyBean::CSSRequestPresentHappyBean()
{
	m_iUin = 0;
	m_iRoomID = -1;
	m_nGameID = -1;
	m_nPresentMode = 0;
	m_iServiceTag = 0;
	m_lDeltaHappyBean = 0;
	m_iMaxPresentCount = 0;
	m_iACT = 0;
	m_szTransTag[0] = '\0';
}

CSSRequestPresentHappyBean::~CSSRequestPresentHappyBean()
{
	//
}

int32_t CSSRequestPresentHappyBean::encode( char *pszOut, int32_t& iOutLength )
{
	if(NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nPresentMode);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iServiceTag);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lDeltaHappyBean);
	iOutLength += coded_length;

	coded_length += CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMaxPresentCount);
	iOutLength += coded_length;

	coded_length += CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iACT);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,max_game_tag_length);
	iOutLength += coded_length;
	return success;

}

int32_t CSSRequestPresentHappyBean::decode( const char *pszIn, const int32_t iInLength )
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int32_t remained_length = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nPresentMode);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServiceTag);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lDeltaHappyBean);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMaxPresentCount);
	remained_length -= decoded_length;

	decoded_length += CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iACT);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,max_game_tag_length);
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}


	return success;
}

void CSSRequestPresentHappyBean::dump()
{

}

//


CSSResponsePresentHappyBean::CSSResponsePresentHappyBean()
{
	m_nResultID = result_id_success;

	m_iUin = 0;
	m_iRoomID = -1;
	m_nGameID = -1;
	m_nPresentMode = 0;
	m_iPresentedCount = 0;
	m_iLastPresentedTime = 0;
	m_lCurHappyBean = 0;
	m_lDeltaHappyBean = 0;
	m_iPlayerCommonInfoMiscFlag = 0;
	m_iACT = 0;
	m_szTransTag[0] = '\0';
}

CSSResponsePresentHappyBean::~CSSResponsePresentHappyBean()
{
	//
}

int32_t CSSResponsePresentHappyBean::encode( char *pszOut, int32_t& iOutLength )
{
	if(NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (int32_t)m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	iOutLength += coded_length;
		
	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iACT);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,max_game_tag_length);
	iOutLength += coded_length;
	if (success == m_nResultID)
	{
		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nPresentMode);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPresentedCount);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLastPresentedTime);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lCurHappyBean);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lDeltaHappyBean);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPlayerCommonInfoMiscFlag);
		iOutLength += coded_length;
	}

	return success;
}

int32_t CSSResponsePresentHappyBean::decode( const char *pszIn, const int32_t iInLength )
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int32_t remained_length = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iACT);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,max_game_tag_length);
	remained_length -= decoded_length;

	if (success == m_nResultID)
	{
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nPresentMode);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPresentedCount);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLastPresentedTime);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lCurHappyBean);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lDeltaHappyBean);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPlayerCommonInfoMiscFlag);
		remained_length -= decoded_length;
	}

	if (remained_length < 0)
	{
		return fail;
	}

	return success;
}

void CSSResponsePresentHappyBean::dump()
{

}


//
CSSNotifyPresentHappyBean::CSSNotifyPresentHappyBean()
{
	m_iUin = 0;
	m_nPresentMode = 0;
	m_iPresentedCount = 0;
	m_iLastPresentedTime = 0;
	m_lCurHappyBean = 0;
	m_iPlayerCommonInfoMiscFlag = 0;
}

CSSNotifyPresentHappyBean::~CSSNotifyPresentHappyBean()
{
	//
}	

int32_t CSSNotifyPresentHappyBean::encode( char *pszOut, int32_t& iOutLength )
{
	if(NULL == pszOut)
	{
		return fail;
	}

	char* ptmp = pszOut;
	int32_t coded_length = 0;

	iOutLength = 0;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nPresentMode);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPresentedCount);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLastPresentedTime);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lCurHappyBean);
	iOutLength += coded_length;

	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iPlayerCommonInfoMiscFlag);
	iOutLength += coded_length;

	return success;

}

int32_t CSSNotifyPresentHappyBean::decode( const char *pszIn, const int32_t iInLength )
{
	if (NULL == pszIn || iInLength <= 0)
	{
		return fail;
	}

	char* ptmp = const_cast<char*>(pszIn);
	int32_t remained_length = iInLength;
	int32_t decoded_length = 0;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nPresentMode);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPresentedCount);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLastPresentedTime);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lCurHappyBean);
	remained_length -= decoded_length;

	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iPlayerCommonInfoMiscFlag);
	remained_length -= decoded_length;

	if (remained_length < 0)
	{
		return fail;
	}

	return success;

}

void CSSNotifyPresentHappyBean::dump()
{

}

///////////////////////////////////////////////
CRequestLockHappyBean::CRequestLockHappyBean()
{
	m_iUin = 0;
	m_iRoomID = invalid_object_id;
	m_iTableID = invalid_object_id;
	m_nGameID = -1;
	m_nLockType = -1;
	m_iACT = 0;

}
CRequestLockHappyBean::~CRequestLockHappyBean()
{

}

  int32_t  CRequestLockHappyBean::encode(char *pszOut, int32_t& iOutLength)
  {

	  if(NULL == pszOut)
	  {
		  return fail;
	  }
	  char* ptmp = pszOut;
	  int32_t coded_length = 0;
	  iOutLength = 0;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iTableID);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nLockType);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iACT);
	  iOutLength += coded_length;
	  return success;
  }

  int32_t  CRequestLockHappyBean::decode(const char *pszIn, const int32_t iInLength)
  {
	  if (NULL == pszIn || iInLength <= 0)
	  {
		  return fail;
	  }

	  char* ptmp = const_cast<char*>(pszIn);
	  int32_t remained_length = iInLength;
	  int32_t decoded_length = 0;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTableID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nLockType);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iACT);
	  remained_length -= decoded_length;

	  if (remained_length < 0)
	  {
		  return fail;
	  }

	  return success;
  }

  void CRequestLockHappyBean::dump()
  {


  }


  CResponseLockHappyBean::CResponseLockHappyBean()
  {
	  m_nResultID = result_id_success;
	  m_iUin = 0;
	  m_nGameID = -1;
	  m_iRoomID = invalid_object_id;
	  m_iTableID = invalid_object_id;
	  m_nLockType = -1;
	  m_iCurrentHappyBean = 0;
	  m_iLockedServerType = -1;
	  m_iLockedServerID = -1;
	  m_iLockedRoomID = invalid_object_id;
	  m_iLockedTableID = invalid_object_id;
	  m_iACT = 0;
  }

  CResponseLockHappyBean::~CResponseLockHappyBean()
  {

  }

  int32_t  CResponseLockHappyBean::encode(char *pszOut, int32_t& iOutLength)
  {
	  if(NULL == pszOut)
	  {
		  return fail;
	  }
	  char* ptmp = pszOut;
	  int32_t coded_length = 0;
	  iOutLength = 0;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  iOutLength += coded_length;
	  
	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	  iOutLength += coded_length;
	  
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iTableID);
	  iOutLength += coded_length;
	  
	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nLockType);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_iCurrentHappyBean);
	  iOutLength += coded_length;
	  
	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iLockedServerType);
	  iOutLength += coded_length;
	   
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLockedServerID);
	  iOutLength += coded_length;
 
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLockedRoomID);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLockedTableID);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iACT);
	  iOutLength += coded_length;
	  return success;

  }

  int32_t  CResponseLockHappyBean::decode(const char *pszIn, const int32_t iInLength)
  {
	  if (NULL == pszIn || iInLength <= 0)
	  {
		  return fail;
	  }

	  char* ptmp = const_cast<char*>(pszIn);
	  int32_t remained_length = iInLength;
	  int32_t decoded_length = 0;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTableID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nLockType);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_iCurrentHappyBean);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iLockedServerType);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLockedServerID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLockedRoomID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLockedTableID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iACT);
	  remained_length -= decoded_length;

	  if (remained_length < 0)
	  {
		  return fail;
	  }
	  return success;

  }
  void CResponseLockHappyBean::dump()
  {

  }



  ///////////////////////////////////////////////////////
  CNotifyRefreshLockHappyBean::CNotifyRefreshLockHappyBean()
  {
	  m_iUin =  0;
	  m_nLockType = -1;
	  m_iServerID = -1;
	  m_nGameID  = -1;
	  m_iRoomID = invalid_object_id;
	  m_iTableID = invalid_object_id;
	  return ;
  }

  CNotifyRefreshLockHappyBean::~CNotifyRefreshLockHappyBean()
  {
	  return ;
  }

  int32_t  CNotifyRefreshLockHappyBean::encode(char *pszOut, int32_t& iOutLength)
  {

	  if(NULL == pszOut)
	  {
		  return fail;
	  }
	  char* ptmp = pszOut;
	  int32_t coded_length = 0;
	  iOutLength = 0;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nLockType);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iServerID);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nGameID);
	  iOutLength += coded_length;
	  
	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iRoomID);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iTableID);
	  iOutLength += coded_length;

	  return success;
  }

  int32_t  CNotifyRefreshLockHappyBean::decode(const char *pszIn, const int32_t iInLength)
  {
	  if (NULL == pszIn || iInLength <= 0)
	  {
		  return fail;
	  }

	  char* ptmp = const_cast<char*>(pszIn);
	  int32_t remained_length = iInLength;
	  int32_t decoded_length = 0;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nLockType);
	   remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nGameID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iRoomID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iTableID);
	  remained_length -= decoded_length;

	  if (remained_length < 0)
	  {
		  return fail;
	  }

	  return success;
  }
  void CNotifyRefreshLockHappyBean::dump()
  {
	  return ;
  }


  //////////////////////////////////////////////////////////////////////////
  CRequestChangeHappyBean::CRequestChangeHappyBean()
  {
	  m_iUin = 0;                                     
	  m_szAccount[0] = '\0'; 
	  m_lUnitHappyBeanChg = 0;
	  m_iUnitCount = 0;
	  m_iStrategy = 0;	
	  m_nServiceTag = 0;
	  m_cFlag = 0;
	  m_szTransTag[0] = '\0';			//事务标识....
	  m_nTransparentDataSize = 0;
	  m_nNofifyTransparentDataSize = 0;
	  return ;
  }
  
  CRequestChangeHappyBean::~CRequestChangeHappyBean()
  {

  }

  int32_t CRequestChangeHappyBean::encode(char *pszOut, int32_t& iOutLength)
  {
	  if(NULL == pszOut)
	  {
		  return fail;
	  }
	  char* ptmp = pszOut;
	  int32_t coded_length = 0;
	  iOutLength = 0;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,max_account_string_length);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lUnitHappyBeanChg);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iUnitCount);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_iStrategy);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nServiceTag);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_cFlag);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_string(&ptmp,m_szTransTag,max_game_tag_length);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_nTransparentDataSize);
	  iOutLength += coded_length;
	
	  coded_length = CCodeEngine::encode_memory(&ptmp,m_szTransparentData,m_nTransparentDataSize);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_nNofifyTransparentDataSize);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_memory(&ptmp,m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	  iOutLength += coded_length;

	  return success;
  }


  int32_t  CRequestChangeHappyBean::decode(const char *pszIn, const int32_t iInLength)
  {

	  if (NULL == pszIn || iInLength <= 0)
	  {
		  return fail;
	  }

	  char* ptmp = const_cast<char*>(pszIn);
	  int32_t remained_length = iInLength;
	  int32_t decoded_length = 0;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount,max_account_string_length);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lUnitHappyBeanChg);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iUnitCount);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_iStrategy);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nServiceTag);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cFlag);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_string(&ptmp,m_szTransTag,max_game_tag_length);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nTransparentDataSize);
	  remained_length -= decoded_length;

	  if(m_nTransparentDataSize>max_transparent_data_size)
	  {
			 return fail;
	  }
	  decoded_length = CCodeEngine::decode_memory(&ptmp,m_szTransparentData,m_nTransparentDataSize);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp,(uint16_t*)&m_nNofifyTransparentDataSize);
	  remained_length -= decoded_length;

	  if(m_nNofifyTransparentDataSize>max_sub_message_size)
	  {
		  return fail;
	  }
	  decoded_length = CCodeEngine::decode_memory(&ptmp,m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	  remained_length -= decoded_length;

	  if (remained_length < 0)
	  {
		  return fail;
	  }
	  return success;
  }

  void CRequestChangeHappyBean::dump()
  {
	  return ;
  }


  //////////////////////////////////////////////////////
  CResponseChangeHappyBean::CResponseChangeHappyBean()
  {
	  m_nResultID = success;
	  m_iUin = 0;		
	  m_szAccount[0] = '\0'; 
	  m_lCurrentHappyBean = 0;
	  m_lUnitHappyBeanChg = 0;
	  m_iUnitCount = 0;
	  m_iStrategy = 0;
	  m_lHappyBeanChg = 0;
	  m_szTransTag[0] = '\0';		
	  m_nTransparentDataSize = 0;
	  m_nNofifyTransparentDataSize = 0;
	  return ;
  }

  CResponseChangeHappyBean::~CResponseChangeHappyBean()
  {

	  return ;
  }

  int32_t  CResponseChangeHappyBean::encode(char *pszOut, int32_t& iOutLength)
  {
	  if(NULL == pszOut)
	  {
		  return fail;
	  }
	  char* ptmp = pszOut;
	  int32_t coded_length = 0;
	  iOutLength = 0;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nResultID);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount,max_account_string_length);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lCurrentHappyBean);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lUnitHappyBeanChg);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iUnitCount);
	  iOutLength += coded_length;
	  
	  coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_iStrategy);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lHappyBeanChg);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_string(&ptmp, m_szTransTag,max_game_tag_length);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nTransparentDataSize);
	  iOutLength += coded_length;
	  
	  coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData,m_nTransparentDataSize);
	  iOutLength += coded_length;


	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nNofifyTransparentDataSize);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	  iOutLength += coded_length;

	  return success;
  }

  int32_t  CResponseChangeHappyBean::decode(const char *pszIn, const int32_t iInLength)
  {

	  if (NULL == pszIn || iInLength <= 0)
	  {
		  return fail;
	  }

	  char* ptmp = const_cast<char*>(pszIn);
	  int32_t remained_length = iInLength;
	  int32_t decoded_length = 0;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nResultID);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount,max_account_string_length);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lCurrentHappyBean);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lUnitHappyBeanChg);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iUnitCount);
	   remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_iStrategy);
	   remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lHappyBeanChg);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_string(&ptmp, m_szTransTag,max_game_tag_length);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nTransparentDataSize);
	  remained_length -= decoded_length;

	  if(m_nTransparentDataSize>=max_transparent_data_size)
	  {
			 return fail;
	  }
	  decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData,m_nTransparentDataSize);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	  remained_length -= decoded_length;

	  if(m_nNofifyTransparentDataSize>max_sub_message_size)
	  {
		 return fail;
	  }
	  decoded_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	  remained_length -= decoded_length;


	  if (remained_length < 0)
	  {
		  return fail;
	  }
	  return success;
  }
  
  void CResponseChangeHappyBean::dump()
  {

	  return ;
  }

  
  CNotifyRefreshHappyBean::CNotifyRefreshHappyBean()
  {

	  m_iUin = 0;			
	  m_lCurrentHappyBean = 0;
	  m_lHappyBeanChg = 0;
	  m_nNofifyTransparentDataSize = 0;
	  return ;
  }
  
  CNotifyRefreshHappyBean::~CNotifyRefreshHappyBean()
  {


  }
  
  int32_t  CNotifyRefreshHappyBean::encode(char *pszOut, int32_t& iOutLength)
  {
	  if(NULL == pszOut)
	  {
		  return fail;
	  }
	  char* ptmp = pszOut;
	  int32_t coded_length = 0;
	  iOutLength = 0;

	  coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lCurrentHappyBean);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_int64(&ptmp, (uint64_t)m_lHappyBeanChg);
	  iOutLength += coded_length;
	 
	  coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nNofifyTransparentDataSize);
	  iOutLength += coded_length;

	  coded_length = CCodeEngine::encode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	  iOutLength += coded_length;
	  
	  return success;
  }
  
  int32_t  CNotifyRefreshHappyBean::decode(const char *pszIn, const int32_t iInLength)
  {
	  if (NULL == pszIn || iInLength <= 0)
	  {
		  return fail;
	  }

	  char* ptmp = const_cast<char*>(pszIn);
	  int32_t remained_length = iInLength;
	  int32_t decoded_length = 0;

	  decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  remained_length -= decoded_length;
 
	  decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lCurrentHappyBean);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int64(&ptmp, (uint64_t*)&m_lHappyBeanChg);
	  remained_length -= decoded_length;

	  decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nNofifyTransparentDataSize);
	  remained_length -= decoded_length;

	  if(m_nNofifyTransparentDataSize>max_sub_message_size)
	  {
			return fail;
	  }
	  decoded_length = CCodeEngine::decode_memory(&ptmp, m_szNofifyTransparentData,m_nNofifyTransparentDataSize);
	  remained_length -= decoded_length;

	  if(remained_length<0)
	  {
			return fail;
	  }
	  return success;
  }


  void CNotifyRefreshHappyBean::dump()
  {

	  return ;
  }

	/////////////////////////////// CNotifyQunPlayerData /////////////
	CNotifyQunPlayerData::CNotifyQunPlayerData()
	{
	    m_iUin = 0;
	    memset(&m_stWebQunData,0,sizeof(m_stWebQunData));
	    m_nDataCount = 0;
	    memset(m_astData,0,sizeof(m_astData));
	}
	CNotifyQunPlayerData::~CNotifyQunPlayerData()
	{
		
	}
	
	int32_t encode_player_qun_data(char** pOut, stQunPlayerData& data)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}
		int32_t iOutLength = 0;
		int32_t coded_length = 0;
	
	        coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_iDataType);
	        iOutLength += coded_length;
	        
	        switch(data.m_iDataType)
	        {
	        case QPD_Type_Charming:
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stCharming.m_iDeltaValue);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stCharming.m_iCurrentValue);
	        	iOutLength += coded_length;
	        	break;
	        case QPD_Type_Achievement:
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stAchievement.m_iDeltaValue);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stAchievement.m_iCurrentValue);
	        	iOutLength += coded_length;
	        	break;
	        case QPD_Type_Money:
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stMoney.m_iDeltaValue);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stMoney.m_iCurrentValue);
	        	iOutLength += coded_length;
	        	break;
	        case QPD_Type_HappyBean :
	        	coded_length = CCodeEngine::encode_int64(pOut, (uint64_t)data.m_DataValue.m_stHappyBean.m_lDeltaValue);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int64(pOut, (uint64_t)data.m_DataValue.m_stHappyBean.m_lCurrentValue);
	        	iOutLength += coded_length;
	        	break;
	        case QPD_Type_GameResultChange:
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iDeltaGamePoint);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iDeltaWinRound);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iDeltaLossRound);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iDeltaDrawRound);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iDeltaEscapeRound);
	        	iOutLength += coded_length;
	        	
				coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iDeltaCostTime);
	        	iOutLength += coded_length;


	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iGamePoint);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iWinRound);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iLossRound);
	        	iOutLength += coded_length;
                
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iDrawRound);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iEscapeRound);
	        	iOutLength += coded_length;

				coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stGameResultChange.m_iCostTime);
	        	iOutLength += coded_length;
	         	break;
	        default:
	        	;
	        };
		return iOutLength;
	}
	
	int32_t  CNotifyQunPlayerData::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
	  	{
			return fail;
	 	}
	  	char* ptmp = pszOut;
	  	int32_t coded_length = 0;
	 	iOutLength = 0;

	  	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  	iOutLength += coded_length;

	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_stWebQunData.m_nDataSize);
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_memory(&ptmp, m_stWebQunData.m_szWebQunInfo,m_stWebQunData.m_nDataSize);
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nDataCount);
	  	iOutLength += coded_length;
	  	
	  	for(int16_t i=0;i<m_nDataCount;++i)
	  	{
	  		coded_length = encode_player_qun_data(&ptmp,m_astData[i]);
	  		iOutLength += coded_length;
	  	}
		return success;
	}
	
	
	
	int32_t decode_player_qun_data(char** pOut, stQunPlayerData& data)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}
		int32_t iOutLength = 0;
		int32_t decode_length = 0;
	
		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data.m_iDataType);
		iOutLength += decode_length;
	        
	        switch(data.m_iDataType)
	        {
	        case QPD_Type_Charming:
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stCharming.m_iDeltaValue);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stCharming.m_iCurrentValue);
	        	iOutLength += decode_length;
	        	break;
	        case QPD_Type_Achievement:
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stAchievement.m_iDeltaValue);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stAchievement.m_iCurrentValue);
	        	iOutLength += decode_length;
	        	break;
	        case QPD_Type_Money:
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stMoney.m_iDeltaValue);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stMoney.m_iCurrentValue);
	        	iOutLength += decode_length;
	        	break;
	        case QPD_Type_HappyBean :
	        	decode_length = CCodeEngine::decode_int64(pOut, (uint64_t*)&data.m_DataValue.m_stHappyBean.m_lDeltaValue);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int64(pOut, (uint64_t*)&data.m_DataValue.m_stHappyBean.m_lCurrentValue);
	        	iOutLength += decode_length;
	        	break;
	        case QPD_Type_GameResultChange:
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iDeltaGamePoint);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iDeltaWinRound);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iDeltaLossRound);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iDeltaDrawRound);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iDeltaEscapeRound);
	        	iOutLength += decode_length;
	        	
				decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iDeltaCostTime);
	        	iOutLength += decode_length;

	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iGamePoint);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iWinRound);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iLossRound);
	        	iOutLength += decode_length;
                
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iDrawRound);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iEscapeRound);
	        	iOutLength += decode_length;

				decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stGameResultChange.m_iCostTime);
	        	iOutLength += decode_length;

	         	break;
	        default:
	        	;
	        };
		return iOutLength;
	}
	
	int32_t  CNotifyQunPlayerData::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
	  	{
			return fail;
	  	}

	  	char* ptmp = const_cast<char*>(pszIn);
	  	int32_t remained_length = iInLength;
	  	int32_t decoded_length = 0;

	  	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  	remained_length -= decoded_length;

	  	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stWebQunData.m_nDataSize);
	  	remained_length -= decoded_length;
	  	
	  	if(m_stWebQunData.m_nDataSize>max_web_qun_data_size)
	  	{
	  		m_stWebQunData.m_nDataSize = max_web_qun_data_size;
	  	}
	  	decoded_length = CCodeEngine::decode_memory(&ptmp, m_stWebQunData.m_szWebQunInfo,m_stWebQunData.m_nDataSize);
	  	remained_length -= decoded_length;
	  	
	  	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDataCount);
	  	remained_length -= decoded_length;
	  	
	  	if(m_nDataCount>QPD_MAX_DATA_NUMBER)
	  	{
	  		m_nDataCount=QPD_MAX_DATA_NUMBER;
	  	}
	  	
	  	for(int16_t i=0;i<m_nDataCount;++i)
	  	{
	  		decoded_length = decode_player_qun_data(&ptmp,m_astData[i]);
	  		remained_length -= decoded_length;
	  	}
	  	if(remained_length<0)
	  		return fail;
	  		
		return success;
	}
	void CNotifyQunPlayerData::dump()
	{
		return ;
	}
	
	
	
	///////////// CNotifyLogicHelloBeat //////////////
	CNotifyLogicHelloBeat::CNotifyLogicHelloBeat()
	{
		m_iUin = 0;			
		m_nGameCount = 0;
		memset(m_aryGameId,0,sizeof(m_aryGameId));
	
		m_nMoneyLockType = -1;
		m_iMoneyLockedServerType = -1;			 
		m_iMoneyLockedServerID = -1;
		m_nMoneyLockedGameID = 0;		 
		m_iMoneyLockedRoomID = -1 ;			 
		m_iMoneyLockedTableID =-1;			 
		m_nLockMoney = 0;
		
		m_nHappyBeanLockType =-1;
		m_iHappyBeanLockedServerType = -1;			 
		m_iHappyBeanLockedServerID = -1;
		m_nHappyBeanLockedGameID = 0;			 
		m_iHappyBeanLockedRoomID = -1;			 
		m_iHappyBeanLockedTableID =-1;	
	}
	CNotifyLogicHelloBeat::~CNotifyLogicHelloBeat()
	{
		
	}
	int32_t  CNotifyLogicHelloBeat::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
	  	{
			return fail;
	 	}
	  	char* ptmp = pszOut;
	  	int32_t coded_length = 0;
	 	iOutLength = 0;

	  	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  	iOutLength += coded_length;

	  	coded_length = CCodeEngine::encode_int8(&ptmp, (uint8_t)m_nGameCount);
	  	iOutLength += coded_length;
	 	for(int8_t i=0;i<m_nGameCount;++i)
	 	{
	 		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_aryGameId[i]);
	  		iOutLength += coded_length;	
	 	}
	 
	 	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nMoneyLockType);
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iMoneyLockedServerType);
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMoneyLockedServerID);
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nMoneyLockedGameID);
	  	iOutLength += coded_length;
	  		 
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMoneyLockedRoomID);
	  	iOutLength += coded_length;
	  		
	  	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iMoneyLockedTableID);
	  	iOutLength += coded_length;	 
		 	
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_nLockMoney);
	  	iOutLength += coded_length;	 
	  	
	  	
	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nHappyBeanLockType);
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_iHappyBeanLockedServerType);
	  	iOutLength += coded_length;

 		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iHappyBeanLockedServerID);
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nHappyBeanLockedGameID);
	  	iOutLength += coded_length;
		
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iHappyBeanLockedRoomID);
	  	iOutLength += coded_length;
	  			 
		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iHappyBeanLockedTableID);
	  	iOutLength += coded_length;		 
		return success;
	}
	int32_t  CNotifyLogicHelloBeat::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
	  	{
			return fail;
	  	}

	  	char* ptmp = const_cast<char*>(pszIn);
	  	int32_t remained_length = iInLength;
	  	int32_t decoded_length = 0;

	  	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  	remained_length -= decoded_length;

	  	decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_nGameCount);
	  	remained_length -= decoded_length;
	 	for(int8_t i=0;i<m_nGameCount;++i)
	 	{
	 		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_aryGameId[i]);
	  		remained_length -= decoded_length;	
	 	}
	 
	 	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nMoneyLockType);
	  	remained_length -= decoded_length;
	  	
	  	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iMoneyLockedServerType);
	  	remained_length -= decoded_length;
	  	
	  
	  	
	  	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoneyLockedServerID);
	  	remained_length -= decoded_length;
	  	
	  	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nMoneyLockedGameID);
	  	remained_length -= decoded_length;
	  		 
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoneyLockedRoomID);
	  	remained_length -= decoded_length;
	  		
	  	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMoneyLockedTableID);
	  	remained_length -= decoded_length;	 
		 	
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nLockMoney);
	  	remained_length -= decoded_length;
	  	
	  	
	  	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nHappyBeanLockType);
	  	remained_length -= decoded_length;
	  	
	  	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iHappyBeanLockedServerType);
	  	remained_length -= decoded_length;

 		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iHappyBeanLockedServerID);
	  	remained_length -= decoded_length;
		
		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nHappyBeanLockedGameID);
	  	remained_length -= decoded_length;
	  	
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iHappyBeanLockedRoomID);
	  	remained_length -= decoded_length;
	  			 
		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iHappyBeanLockedTableID);
	  	remained_length -= decoded_length;	 
		return success;
		
	}
	void CNotifyLogicHelloBeat::dump()
	{
		
	}
	
	
	///////////////////////////////////////////////////////////
	int32_t encode_player_action_data(char** pOut, stPlayerActionData& data)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}
		int32_t iOutLength = 0;
		int32_t coded_length = 0;
	
	        coded_length = CCodeEngine::encode_int8(pOut, (uint8_t)data.m_nDataType);
	        iOutLength += coded_length;
	        
	        switch(data.m_nDataType)
	        {
	        case PAD_TYPE_LOGIN:
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stOnlineData.m_iLoginCount);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int16(pOut, (uint16_t)data.m_DataValue.m_stOnlineData.m_nHallServerId);
	        	iOutLength += coded_length;
	        	
	        	coded_length = CCodeEngine::encode_int32(pOut, (uint32_t)data.m_DataValue.m_stOnlineData.m_iHallPlayerId);
	        	iOutLength += coded_length;
	        	break;
	        default:
	        	;
	        };
		return iOutLength;
	}
	
	int32_t decode_player_action_data(char** pOut, stPlayerActionData& data)
	{
		if (*pOut == NULL || pOut == NULL)
		{
			return 0;
		}
		int32_t iOutLength = 0;
		int32_t decode_length = 0;
	
		decode_length = CCodeEngine::decode_int8(pOut, (uint8_t*)&data.m_nDataType);
		iOutLength += decode_length;
	        
	        switch(data.m_nDataType)
	        {
	        case PAD_TYPE_LOGIN:
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stOnlineData.m_iLoginCount);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int16(pOut, (uint16_t*)&data.m_DataValue.m_stOnlineData.m_nHallServerId);
	        	iOutLength += decode_length;
	        	
	        	decode_length = CCodeEngine::decode_int32(pOut, (uint32_t*)&data.m_DataValue.m_stOnlineData.m_iHallPlayerId);
	        	iOutLength += decode_length;
	        	break;
	        default:
	        	;
	        };
		return iOutLength;
	}
	
	CNotifyPlayerActionData::CNotifyPlayerActionData()
	{
		m_iUin = 0;
		m_stAccount[0] ='\0';
		m_nDataCount =0;
	        memset(m_astData,0,sizeof(m_astData));
	}
	
	CNotifyPlayerActionData::~CNotifyPlayerActionData()
	{
		
	}
	
	
	
	int32_t  CNotifyPlayerActionData::encode(char *pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
	  	{
			return fail;
	 	}
	  	char* ptmp = pszOut;
	  	int32_t coded_length = 0;
	 	iOutLength = 0;

	  	coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iUin);
	  	iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_stAccount,(const int16_t)sizeof(m_stAccount));
	  	iOutLength += coded_length;
	  	
	  	coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nDataCount);
	  	iOutLength += coded_length;
	  	
	  	for(int16_t i=0;i<m_nDataCount;++i)
	  	{
	  		coded_length =encode_player_action_data(&ptmp,m_astData[i]);
			iOutLength += coded_length;
	  	}
		return success;
	}
	
	int32_t  CNotifyPlayerActionData::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
	  	{
			return fail;
	  	}

	  	char* ptmp = const_cast<char*>(pszIn);
	  	int32_t remained_length = iInLength;
	  	int32_t decoded_length = 0;

	  	decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iUin);
	  	remained_length -= decoded_length;
	  	
	  	decoded_length = CCodeEngine::decode_string(&ptmp, m_stAccount,(const int16_t)sizeof(m_stAccount));
	  	remained_length -= decoded_length;
	  	
	  	decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nDataCount);
	  	remained_length -= decoded_length;
	  	
	  	for(int16_t i=0;i<m_nDataCount;++i)
	  	{
	  		decoded_length =decode_player_action_data(&ptmp,m_astData[i]);
			remained_length -= decoded_length;
			if(remained_length<0)
			{
				return fail;	
			}
	  	}
		return success;
	}
	
	 void CNotifyPlayerActionData::dump()
	 {
	 	return ;
	 }

}}
