
#include "Common/common_codeengine.h"
#include "public/server_state_message_body.h"

namespace KingNet { namespace Server {

	//SS_HALL_NOTIFY_INIT
	CNotifyHallServerInit::CNotifyHallServerInit()
	{
		m_iLocalIP = 0;
		m_nLocalPort = 0;
	}

	CNotifyHallServerInit::~CNotifyHallServerInit()
	{
        //do nothing
	}

	int32_t CNotifyHallServerInit::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, (uint32_t)m_iLocalIP);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp,(uint16_t)m_nLocalPort);
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyHallServerInit::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iLocalIP);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nLocalPort);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CNotifyHallServerInit::dump()
	{
        //do nothing
	}

//////////////////////////////////////////////////////////////////////////
	CNotifyPlayerProfile::CNotifyPlayerProfile()
	{
		m_iUin = 0;
		m_szAccount[0] = '\0';
		m_cSex = 0;
	}

	CNotifyPlayerProfile::~CNotifyPlayerProfile()
	{

	}

	int32_t CNotifyPlayerProfile::encode(char * pszOut, int32_t & iOutLength)
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

		coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cSex);
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyPlayerProfile::decode(const char * pszIn, const int32_t iInLength)
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

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, (const int16_t)sizeof(m_szAccount));
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cSex);
		remained_length -= decoded_length;

		return success;
	}

    void CNotifyPlayerProfile::dump()
    {
        //do nothing
    }

//////////////////////////////////////////////////////////////////////////

    //SS_HALL_STATE_NOTIFY_KICKPLAYER
    CNotifyKickPlayerToHallServer::CNotifyKickPlayerToHallServer()
    {
        m_iVictimUin = 0;
        m_szVictimAccount[0] = '\0';

        m_iActorUin = 0;
        m_szActorAccount[0] = '\0';

        m_nReasonID = 0;
        m_szReasonMessage[0] = '\0';
    }

    CNotifyKickPlayerToHallServer::~CNotifyKickPlayerToHallServer()
    {
        //do nothing
    }

    int32_t CNotifyKickPlayerToHallServer::encode( char *pszOut, int32_t& iOutLength )
    {
        if (NULL == pszOut)
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int32(&ptmp, m_iVictimUin);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_string(&ptmp, m_szVictimAccount, (const int16_t)sizeof(m_szVictimAccount));
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int32(&ptmp, m_iActorUin);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_string(&ptmp, m_szActorAccount, (const int16_t)sizeof(m_szActorAccount));
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_nReasonID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
        iOutLength += coded_length;


        return success;
    }

    int32_t CNotifyKickPlayerToHallServer::decode( const char *pszIn, const int32_t iInLength )
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int16_t remained_length  = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iVictimUin);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_string(&ptmp, m_szVictimAccount, (const int16_t)sizeof(m_szVictimAccount));
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iActorUin);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_string(&ptmp, m_szActorAccount, (const int16_t)sizeof(m_szActorAccount));
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nReasonID);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, (const int16_t)sizeof(m_szReasonMessage));
        remained_length -= decoded_length;

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }

    void CNotifyKickPlayerToHallServer::dump()
    {
        //do nothing
    }


//////////////////////////////////////////////////////////////////////////


    //SS_HALL_STATE_REQUEST_GET_PLAYER_STATUS
    CRequestGetPlayerStatusFromStateServer::CRequestGetPlayerStatusFromStateServer()
    {
        m_iMyUin = 0;
        m_nCount = 0;

    }

    CRequestGetPlayerStatusFromStateServer::~CRequestGetPlayerStatusFromStateServer()
    {
        //do nothing
    }

    int32_t CRequestGetPlayerStatusFromStateServer::encode( char *pszOut, int32_t& iOutLength )
    {
        if (NULL == pszOut)
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int32(&ptmp, m_iMyUin);
        iOutLength += coded_length;

        if (MAX_REQUEST_PLAYER_COUNT < m_nCount)
        {
            m_nCount = MAX_REQUEST_PLAYER_COUNT;
        }
        coded_length = CCodeEngine::encode_int16(&ptmp, m_nCount);
        iOutLength += coded_length;

        for ( int32_t i = 0; i < m_nCount; ++i)
        {
            coded_length = CCodeEngine::encode_int32(&ptmp, m_aiUin[i]);
            iOutLength += coded_length;
        }

        return success;
    }

    int32_t CRequestGetPlayerStatusFromStateServer::decode( const char *pszIn, const int32_t iInLength )
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int16_t remained_length  = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUin);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nCount);
        remained_length -= decoded_length;

        if (MAX_REQUEST_PLAYER_COUNT < m_nCount)
        {
            m_nCount = MAX_REQUEST_PLAYER_COUNT;
        }
        for ( int32_t i = 0; i < m_nCount; ++i)
        {
            decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_aiUin[i]);
            remained_length -= decoded_length;
        }

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }

    void CRequestGetPlayerStatusFromStateServer::dump()
    {
        //do nothing
    }

	CRequestGetAllPlayerState::CRequestGetAllPlayerState()
	{
		m_cFlag = 0;
	}

	CRequestGetAllPlayerState::~CRequestGetAllPlayerState()
	{

	}

	void CRequestGetAllPlayerState::dump()
	{
		//do nothing
	}

	int32_t CRequestGetAllPlayerState::encode( char *pszOut, int32_t& iOutLength )
	{
		if (NULL == pszOut)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cFlag);
		iOutLength += coded_length;

		return success;
	}

	int32_t CRequestGetAllPlayerState::decode( const char *pszIn, const int32_t iInLength )
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cFlag);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	CResponseGetAllPlayerState::CResponseGetAllPlayerState()
	{
		m_nResultID = success;
	}

	CResponseGetAllPlayerState::~CResponseGetAllPlayerState()
	{

	}

	void CResponseGetAllPlayerState::dump()
	{
		//do nothing
	}

	int32_t CResponseGetAllPlayerState::encode( char *pszOut, int32_t& iOutLength )
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

	int32_t CResponseGetAllPlayerState::decode( const char *pszIn, const int32_t iInLength )
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

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}
		
	int32_t encode_user_status(char** pszOut, stUserStatus& userStatus)
    {
        //前2Byte存储stStatus的实际大小
        if (*pszOut == NULL || pszOut == NULL)
        {
            return 0;
        }

        int16_t status_size = 0;
        char* ptmp = *pszOut;

        int32_t iOutLength = 0;
        int32_t coded_length = 0;

        *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

        coded_length = CCodeEngine::encode_int16(pszOut, userStatus.m_iGameID);
        status_size += coded_length;

        coded_length = CCodeEngine::encode_int32(pszOut, userStatus.m_nServerID);
        status_size += coded_length;

        coded_length = CCodeEngine::encode_int32(pszOut, userStatus.m_nRoomID);
        status_size += coded_length;

        coded_length = CCodeEngine::encode_int32(pszOut, userStatus.m_nTableID);
        status_size += coded_length;

        coded_length = CCodeEngine::encode_int8(pszOut, userStatus.m_iSeatID);
        status_size += coded_length;

        coded_length = CCodeEngine::encode_int8(pszOut, userStatus.m_iState);
        status_size += coded_length;

        coded_length = CCodeEngine::encode_string(pszOut, 
            userStatus.m_szPath, sizeof(userStatus.m_szPath));
        status_size += coded_length;

        *pszOut -= (status_size + sizeof(int16_t));
        coded_length = CCodeEngine::encode_int16(pszOut, status_size);

        iOutLength = (status_size + coded_length);

        *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

        return iOutLength;
    }

    int32_t decode_user_status(char **pszOut, stUserStatus& userStatus)
    {
        if (*pszOut == NULL || pszOut == NULL)
        {
            return 0;
        }

        int16_t status_size = 0;
        int32_t iOutLength = 0;
        int32_t decode_length = 0;

        decode_length = CCodeEngine::decode_int16(pszOut,(uint16_t*)&status_size);
        iOutLength += decode_length;

        decode_length = CCodeEngine::decode_int16(pszOut, (uint16_t*)&userStatus.m_iGameID);
        iOutLength += decode_length;
        status_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&userStatus.m_nServerID);
        iOutLength += decode_length;
        status_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&userStatus.m_nRoomID);
        iOutLength += decode_length;
        status_size -= decode_length;

        decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&userStatus.m_nTableID);
        iOutLength += decode_length;
        status_size -= decode_length;

        decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&userStatus.m_iSeatID);
        iOutLength += decode_length;
        status_size -= decode_length;

        decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&userStatus.m_iState);
        iOutLength += decode_length;
        status_size -= decode_length;

        decode_length = CCodeEngine::decode_string(pszOut, 
            userStatus.m_szPath, (const int16_t)sizeof(userStatus.m_szPath));
        iOutLength += decode_length;
        status_size -= decode_length;

        if (status_size < 0)
        {
            return 0;
        }
        else
        {
            //skip remained code
            (*pszOut)  += status_size;
            iOutLength += status_size;
        }

        return iOutLength;
    }

    int32_t encode_state_data( char** pszOut, stStateData& data )
    {
        //前2Byte存储stStateData的实际大小
        if (*pszOut == NULL || pszOut == NULL)
        {
            return 0;
        }

        int16_t data_size = 0;
        char* ptmp = *pszOut;

        int32_t iOutLength = 0;
        int32_t coded_length = 0;

        *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

        coded_length = CCodeEngine::encode_int8(pszOut, data.m_cClass);
        data_size += coded_length;

        if(HALL_SERVER_STATUE_DATA_TYPE == data.m_cClass)
        {
            coded_length = CCodeEngine::encode_int32(pszOut, data.hallState.m_iState);
            data_size += coded_length;
        }
        else if(LOGIC_SERVER_STATUE_DATA_TYPE == data.m_cClass)
        {
            if(data.logicState.m_cRoomCount > MAX_ROOM_SESSION_COUNT)
            {
                data.logicState.m_cRoomCount = MAX_ROOM_SESSION_COUNT;
            }

            coded_length = CCodeEngine::encode_int8(pszOut, data.logicState.m_cRoomCount);
            data_size += coded_length;

            for ( int32_t i = 0; i < data.logicState.m_cRoomCount; ++i)
            {
                coded_length = encode_user_status(pszOut, data.logicState.m_astStatus[i]);
                data_size += coded_length;
            }
        }
        else
        {
            return 0;
        }

        *pszOut -= (data_size + sizeof(int16_t));
        coded_length = CCodeEngine::encode_int16(pszOut, data_size);

        iOutLength = (data_size + coded_length);

        *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

        return iOutLength;
    }

    int32_t decode_state_data( char** pszOut, stStateData& data )
    {
        if (*pszOut == NULL || pszOut == NULL)
        {
            return 0;
        }

        int16_t data_size = 0;
        int32_t iOutLength = 0;
        int32_t decode_length = 0;

        decode_length = CCodeEngine::decode_int16(pszOut,(uint16_t*)&data_size);
        iOutLength += decode_length;

        decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&data.m_cClass);
        iOutLength += decode_length;
        data_size -= decode_length;

        if (HALL_SERVER_STATUE_DATA_TYPE == data.m_cClass)
        {
            decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&data.hallState.m_iState);
            iOutLength += decode_length;
            data_size -= decode_length;
        }
        else if(LOGIC_SERVER_STATUE_DATA_TYPE == data.m_cClass)
        {
            decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&data.logicState.m_cRoomCount);
            iOutLength += decode_length;
            data_size -= decode_length;

            if(data.logicState.m_cRoomCount > MAX_ROOM_SESSION_COUNT)
            {
                data.logicState.m_cRoomCount = MAX_ROOM_SESSION_COUNT;
            }

            for ( int32_t i = 0; i < data.logicState.m_cRoomCount; ++i)
            {
                decode_length = decode_user_status(pszOut, data.logicState.m_astStatus[i]);
                iOutLength += decode_length;
                data_size -= decode_length;
            }
        }
        else
        {
            return 0;
        }

        if (data_size < 0)
        {
            return 0;
        }
        else
        {
            //skip remained code
            (*pszOut) += data_size;
            iOutLength += data_size;
        }

        return iOutLength;
    }


    int32_t encode_playerstatus_info( char** pszOut, stPlayerStatusInfo& playerstatus )
    {
        //前2Byte存储stPlayerStatusInfo的实际大小
        if (*pszOut == NULL || pszOut == NULL)
        {
            return 0;
        }

        int16_t playerstatus_size = 0;
        char* ptmp = *pszOut;

        int32_t iOutLength = 0;
        int32_t coded_length = 0;

        *pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

        coded_length = CCodeEngine::encode_int32(pszOut, playerstatus.m_nUin);
        playerstatus_size += coded_length;

        coded_length = CCodeEngine::encode_string(pszOut, playerstatus.m_szAccount, (const int16_t)sizeof(playerstatus.m_szAccount));
        playerstatus_size += coded_length;

        if(playerstatus.m_byClassCount > MAX_CLASS_COUNT)
        {
            playerstatus.m_byClassCount = MAX_CLASS_COUNT;
        }

        coded_length = CCodeEngine::encode_int8(pszOut, playerstatus.m_byClassCount);
        playerstatus_size += coded_length;

        for ( int32_t i = 0; i < playerstatus.m_byClassCount; ++i)
        {
            coded_length = encode_state_data(pszOut, playerstatus.m_stStateData[i]);
            playerstatus_size += coded_length;
        }

        *pszOut -= (playerstatus_size + sizeof(int16_t));
        coded_length = CCodeEngine::encode_int16(pszOut, playerstatus_size);

        iOutLength = (playerstatus_size + coded_length);

        *pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

        return iOutLength;
    }

    int32_t decode_playerstatus_info( char** pszOut, stPlayerStatusInfo& playerstatus )
    {
        if (*pszOut == NULL || pszOut == NULL)
        {
            return 0;
        }

        int16_t playerstatus_size = 0;
        int32_t iOutLength = 0;
        int32_t decode_length = 0;

        decode_length = CCodeEngine::decode_int16(pszOut,(uint16_t*)&playerstatus_size);
        iOutLength += decode_length;

        decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&playerstatus.m_nUin);
        iOutLength += decode_length;
        playerstatus_size -= decode_length;

        decode_length = CCodeEngine::decode_string(pszOut, playerstatus.m_szAccount, (const uint16_t)sizeof(playerstatus.m_szAccount));
        iOutLength += decode_length;
        playerstatus_size -= decode_length;

        decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&playerstatus.m_byClassCount);
        iOutLength += decode_length;
        playerstatus_size -= decode_length;

        if(playerstatus.m_byClassCount > MAX_CLASS_COUNT)
        {
            playerstatus.m_byClassCount = MAX_CLASS_COUNT;
        }

        for(int32_t i = 0; i < playerstatus.m_byClassCount; ++i)
        {
            decode_length = decode_state_data(pszOut, playerstatus.m_stStateData[i]);
            iOutLength += decode_length;
            playerstatus_size -= decode_length;

        }

        if (playerstatus_size < 0)
        {
            return 0;
        }
        else
        {
            //skip remained code
            (*pszOut) += playerstatus_size;
            iOutLength += playerstatus_size;
        }

        return iOutLength;
    }


    //SS_HALL_STATE_REQUEST_GET_PLAYER_STATUS
    CResponseGetPlayerStatusFromStateServer::CResponseGetPlayerStatusFromStateServer()
    {
        m_nResultID = result_id_success;
        m_iMyUin = 0;
        m_nCount = 0;
    }

    CResponseGetPlayerStatusFromStateServer::~CResponseGetPlayerStatusFromStateServer()
    {
        //do nothing
    }

    int32_t CResponseGetPlayerStatusFromStateServer::encode( char *pszOut, int32_t& iOutLength )
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

        coded_length = CCodeEngine::encode_int32(&ptmp, m_iMyUin);
        iOutLength += coded_length;

        if (result_id_success == m_nResultID)
        {
            if (MAX_RESPONSE_PLAYER_COUNT < m_nCount)
            {
                m_nCount = MAX_RESPONSE_PLAYER_COUNT;
            }
            coded_length = CCodeEngine::encode_int16(&ptmp, m_nCount);
            iOutLength += coded_length;

            for (int32_t i = 0; i < m_nCount; ++i)
            {
                //coded_length = CCodeEngine::encode_int16(&ptmp, m_astPlayerStatus[i].m_shSize);
                //iOutLength += coded_length;

                coded_length = encode_playerstatus_info(&ptmp, m_astPlayerStatus[i]);
                iOutLength += coded_length;
            }
        }

        return success;
    }

    int32_t CResponseGetPlayerStatusFromStateServer::decode( const char *pszIn, const int32_t iInLength )
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

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iMyUin);
        remained_length -= decoded_length;

        if (result_id_success == m_nResultID)
        {
            decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nCount);
            remained_length -= decoded_length;

            if (MAX_RESPONSE_PLAYER_COUNT < m_nCount)
            {
                m_nCount = MAX_RESPONSE_PLAYER_COUNT;
            }
            for (int32_t i = 0; i < m_nCount; ++i)
            {
                //decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_astPlayerStatus[i].m_shSize);
                //remained_length -= decoded_length;

                decoded_length = decode_playerstatus_info(&ptmp, m_astPlayerStatus[i]);
                remained_length -= decoded_length;
            }
        }

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }

    void CResponseGetPlayerStatusFromStateServer::dump()
    {
        //do nothing
    }

	int32_t encode_state_data_For_GM( char** pszOut, stStateData_GM& data )
	{
		//前2Byte存储stStateData的实际大小
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t data_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int8(pszOut, data.m_cClass);
		data_size += coded_length;

		if(HALL_SERVER_STATUE_DATA_TYPE == data.m_cClass)
		{
			coded_length = CCodeEngine::encode_int32(pszOut, data.hallState.m_iState);
			data_size += coded_length;

			coded_length = CCodeEngine::encode_int32(pszOut, data.hallState.m_nServerId);
			data_size += coded_length;
		}
		else if(LOGIC_SERVER_STATUE_DATA_TYPE == data.m_cClass)
		{
			if(data.logicState.m_cRoomCount > MAX_ROOM_SESSION_COUNT)
			{
				data.logicState.m_cRoomCount = MAX_ROOM_SESSION_COUNT;
			}

			coded_length = CCodeEngine::encode_int8(pszOut, data.logicState.m_cRoomCount);
			data_size += coded_length;

			for ( int32_t i = 0; i < data.logicState.m_cRoomCount; ++i)
			{
				coded_length = encode_user_status(pszOut, data.logicState.m_astStatus[i]);
				data_size += coded_length;
			}
		}
		else
		{
			return 0;
		}

		*pszOut -= (data_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, data_size);

		iOutLength = (data_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_state_data_For_GM( char** pszOut, stStateData_GM& data )
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t data_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszOut,(uint16_t*)&data_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&data.m_cClass);
		iOutLength += decode_length;
		data_size -= decode_length;

		if (HALL_SERVER_STATUE_DATA_TYPE == data.m_cClass)
		{
			decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&data.hallState.m_iState);
			iOutLength += decode_length;
			data_size -= decode_length;

			decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&data.hallState.m_nServerId);
			iOutLength += decode_length;
			data_size -= decode_length;
		}
		else if(LOGIC_SERVER_STATUE_DATA_TYPE == data.m_cClass)
		{
			decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&data.logicState.m_cRoomCount);
			iOutLength += decode_length;
			data_size -= decode_length;

			if(data.logicState.m_cRoomCount > MAX_ROOM_SESSION_COUNT)
			{
				data.logicState.m_cRoomCount = MAX_ROOM_SESSION_COUNT;
			}

			for ( int32_t i = 0; i < data.logicState.m_cRoomCount; ++i)
			{
				decode_length = decode_user_status(pszOut, data.logicState.m_astStatus[i]);
				iOutLength += decode_length;
				data_size -= decode_length;
			}
		}
		else
		{
			return 0;
		}

		if (data_size < 0)
		{
			return 0;
		}
		else
		{
			//skip remained code
			(*pszOut) += data_size;
			iOutLength += data_size;
		}

		return iOutLength;
	}

	int32_t encode_playerstatus_info_For_GM( char** pszOut, stPlayerStatusInfo_GM& playerstatus )
	{
		//前2Byte存储stPlayerStatusInfo的实际大小
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t playerstatus_size = 0;
		char* ptmp = *pszOut;

		int32_t iOutLength = 0;
		int32_t coded_length = 0;

		*pszOut += sizeof(int16_t); //为元素的长度预留2Byte的空间

		coded_length = CCodeEngine::encode_int32(pszOut, playerstatus.m_nUin);
		playerstatus_size += coded_length;

		coded_length = CCodeEngine::encode_string(pszOut, playerstatus.m_szAccount, (const int16_t)sizeof(playerstatus.m_szAccount));
		playerstatus_size += coded_length;

		if(playerstatus.m_byClassCount > MAX_CLASS_COUNT)
		{
			playerstatus.m_byClassCount = MAX_CLASS_COUNT;
		}

		coded_length = CCodeEngine::encode_int8(pszOut, playerstatus.m_byClassCount);
		playerstatus_size += coded_length;

		for ( int32_t i = 0; i < playerstatus.m_byClassCount; ++i)
		{
			coded_length = encode_state_data_For_GM(pszOut, playerstatus.m_stStateData[i]);
			playerstatus_size += coded_length;
		}

		*pszOut -= (playerstatus_size + sizeof(int16_t));
		coded_length = CCodeEngine::encode_int16(pszOut, playerstatus_size);

		iOutLength = (playerstatus_size + coded_length);

		*pszOut = ptmp + iOutLength; //	把指针指向下一个可用位置

		return iOutLength;
	}

	int32_t decode_playerstatus_info_For_GM( char** pszOut, stPlayerStatusInfo_GM& playerstatus )
	{
		if (*pszOut == NULL || pszOut == NULL)
		{
			return 0;
		}

		int16_t playerstatus_size = 0;
		int32_t iOutLength = 0;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(pszOut,(uint16_t*)&playerstatus_size);
		iOutLength += decode_length;

		decode_length = CCodeEngine::decode_int32(pszOut, (uint32_t*)&playerstatus.m_nUin);
		iOutLength += decode_length;
		playerstatus_size -= decode_length;

		decode_length = CCodeEngine::decode_string(pszOut, playerstatus.m_szAccount, (const uint16_t)sizeof(playerstatus.m_szAccount));
		iOutLength += decode_length;
		playerstatus_size -= decode_length;

		decode_length = CCodeEngine::decode_int8(pszOut, (uint8_t*)&playerstatus.m_byClassCount);
		iOutLength += decode_length;
		playerstatus_size -= decode_length;

		if(playerstatus.m_byClassCount > MAX_CLASS_COUNT)
		{
			playerstatus.m_byClassCount = MAX_CLASS_COUNT;
		}

		for(int32_t i = 0; i < playerstatus.m_byClassCount; ++i)
		{
			decode_length = decode_state_data_For_GM(pszOut, playerstatus.m_stStateData[i]);
			iOutLength += decode_length;
			playerstatus_size -= decode_length;

		}

		if (playerstatus_size < 0)
		{
			return 0;
		}
		else
		{
			//skip remained code
			(*pszOut) += playerstatus_size;
			iOutLength += playerstatus_size;
		}

		return iOutLength;
	}

	//SS_GM_GET_PLAYER_STATE
	CResponseGetPlayerStateForGMServer::CResponseGetPlayerStateForGMServer()
	{
		m_nResultID = result_id_success;
		m_nCount = 0;
	}

	CResponseGetPlayerStateForGMServer::~CResponseGetPlayerStateForGMServer()
	{
		//do nothing
	}

	int32_t CResponseGetPlayerStateForGMServer::encode( char *pszOut, int32_t& iOutLength )
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

		if (result_id_success == m_nResultID)
		{
			if (MAX_RESPONSE_PLAYER_COUNT < m_nCount)
			{
				m_nCount = MAX_RESPONSE_PLAYER_COUNT;
			}
			coded_length = CCodeEngine::encode_int16(&ptmp, m_nCount);
			iOutLength += coded_length;

			for (int32_t i = 0; i < m_nCount; ++i)
			{
				coded_length = encode_playerstatus_info_For_GM(&ptmp, m_astPlayerStatus[i]);
				iOutLength += coded_length;
			}
		}

		return success;
	}

	int32_t CResponseGetPlayerStateForGMServer::decode( const char *pszIn, const int32_t iInLength )
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

		if (result_id_success == m_nResultID)
		{
			decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nCount);
			remained_length -= decoded_length;

			if (MAX_RESPONSE_PLAYER_COUNT < m_nCount)
			{
				m_nCount = MAX_RESPONSE_PLAYER_COUNT;
			}
			for (int32_t i = 0; i < m_nCount; ++i)
			{
				decoded_length = decode_playerstatus_info_For_GM(&ptmp, m_astPlayerStatus[i]);
				remained_length -= decoded_length;
			}
		}

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseGetPlayerStateForGMServer::dump()
	{
		//do nothing
	}

//////////////////////////////////////////////////////////////////////////

    //SS_HALL_STATE_REPORT_GAME_STATE
    CReportGameState::CReportGameState()
    {
        m_nUin  = -1;
        m_szAccount[0] = '\0';
        m_byClassCount = 0;
    }

    CReportGameState::~CReportGameState()
    {
        //do nothing
    }

    void CReportGameState::dump()
    {
        //do nothing
    }

    int32_t CReportGameState::encode(char *pszOut, int32_t& iOutLength)
    {
        if (NULL == pszOut || 0 > iOutLength)
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int32(&ptmp, m_nUin);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_string(&ptmp, m_szAccount, sizeof(m_szAccount));
        iOutLength += coded_length;

        if(m_byClassCount > MAX_CLASS_COUNT)
        {
            m_byClassCount = MAX_CLASS_COUNT;
        }

        coded_length = CCodeEngine::encode_int8(&ptmp, m_byClassCount);
        iOutLength += coded_length;

        for ( int32_t i = 0; i < m_byClassCount; ++i)
        {
            coded_length = encode_state_data(&ptmp, m_stStateData[i]);
            iOutLength += coded_length;
        }

        return success;
    }

    int32_t CReportGameState::decode(const char *pszIn, const int32_t iInLength)
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int16_t remained_length  = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nUin);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_string(&ptmp, m_szAccount, sizeof(m_szAccount));
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_byClassCount);
        remained_length -= decoded_length;

        if(m_byClassCount > MAX_CLASS_COUNT)
        {
            m_byClassCount = MAX_CLASS_COUNT;
        }

        for(int32_t i=0; i<m_byClassCount; ++i)
        {
            decoded_length = decode_state_data(&ptmp, m_stStateData[i]);
            remained_length -= decoded_length;
        }

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }

//////////////////////////////////////////////////////////////////////////


    //SS_HALL_STATE_REQUEST_TRANSFER_MESSAGE
    CRequestTransferMessage::CRequestTransferMessage()
    {
        m_nSourceUin    = -1;
        m_iTransferType = -1;
        m_iDestUinCount = 0;
        m_nDataSize     = 0;
        m_szTransparentData[0] = '\0';
    }

    CRequestTransferMessage::~CRequestTransferMessage()
    {
        //do nothing
    }

    void CRequestTransferMessage::dump()
    {
        //do nothing
    }

    int32_t CRequestTransferMessage::encode(char *pszOut, int32_t& iOutLength)
    {
        if ((NULL == pszOut) || (m_nDataSize > max_transparent_data_size))
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int32(&ptmp, m_nSourceUin);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int8(&ptmp, m_iTransferType);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_iDestUinCount);
        iOutLength += coded_length;

        if (MAX_DEST_UIN_NUMS < m_iDestUinCount)
        {
            return fail;
        }

        for (int16_t i = 0; i < m_iDestUinCount; i++)
        {
            coded_length = CCodeEngine::encode_int32(&ptmp, m_DestUins[i]);
            iOutLength += coded_length;
        }

        coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nDataSize);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
        iOutLength += coded_length;

        return success;
    }

    int32_t CRequestTransferMessage::decode(const char *pszIn, const int32_t iInLength)
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int32_t remained_length = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_nSourceUin);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t *)&m_iTransferType);
        remained_length -= decoded_length;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_iDestUinCount);
        remained_length -= decoded_length;

        if (MAX_DEST_UIN_NUMS < m_iDestUinCount)
        {
            m_iDestUinCount = MAX_DEST_UIN_NUMS;
        }

        for (int32_t i = 0; i < m_iDestUinCount; i++)
        {
            decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_DestUins[i]);
            remained_length -= decoded_length;
        }


        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_nDataSize);
        remained_length -= decoded_length;

		if (m_nDataSize > max_transparent_data_size)
		{
			return fail;
		}

        decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
        remained_length -= decoded_length;

        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }


    CResponseTransferMessage::CResponseTransferMessage()
    {
        m_iResultID = result_id_fail;
        m_nDataSize = 0;
        m_szTransparentData[0] = '\0';
        m_szReasonMessage[0] = '\0';
    }

    CResponseTransferMessage::~CResponseTransferMessage()
    {
        //do nothing
    }

    void CResponseTransferMessage::dump()
    {
        //do nothing
    }

    int32_t CResponseTransferMessage::encode(char *pszOut, int32_t& iOutLength)
    {
        if ((NULL == pszOut) || (m_nDataSize > max_transparent_data_size))
        {
            return fail;
        }

        char* ptmp = pszOut;
        int32_t coded_length = 0;

        iOutLength = 0;

        coded_length = CCodeEngine::encode_int16(&ptmp, m_iResultID);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nDataSize);
        iOutLength += coded_length;

        coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
        iOutLength += coded_length;

        if (0 != m_iResultID)
        {
            coded_length = CCodeEngine::encode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
            iOutLength += coded_length;
        }

        return success;
    }
    int32_t CResponseTransferMessage::decode(const char *pszIn, const int32_t iInLength)
    {
        if (NULL == pszIn || iInLength <= 0)
        {
            return fail;
        }

        char* ptmp = const_cast<char*>(pszIn);
        int32_t remained_length = iInLength;
        int32_t decoded_length = 0;

        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_iResultID);
        remained_length -= decoded_length;



        decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_nDataSize);
        remained_length -= decoded_length;

		if (m_nDataSize > max_transparent_data_size)
		{
			return fail;
		}

        decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
        remained_length -= decoded_length;

        if (0 != m_iResultID)
        {
            decoded_length = CCodeEngine::decode_string(&ptmp, m_szReasonMessage, sizeof(m_szReasonMessage));
            remained_length -= decoded_length;
        }


        if (remained_length < 0)
        {
            return fail;
        }

        return success;
    }


//////////////////////////////////////////////////////////////////////////

    //SS_HALL_STATE_NOTIFY_TRANSFER_MESSAGE
	CNotifyTransferMessage::CNotifyTransferMessage()
    {
        m_nSourceUin    = -1;
        m_iTransferType = -1;
        m_nDestUin      = -1;
        m_nDataSize     = 0;
        m_szTransparentData[0] = '\0';
    }

	CNotifyTransferMessage::~CNotifyTransferMessage()
    {
        //do nothing
    }

	void CNotifyTransferMessage::dump()
    {
        //do nothing
    }

	int32_t CNotifyTransferMessage::encode(char* pszOut, int32_t& iOutLength)
	{
		if ((NULL == pszOut) || ((m_nDataSize > max_transparent_data_size)))
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nSourceUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_iTransferType);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nDestUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int16(&ptmp, (uint16_t)m_nDataSize);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyTransferMessage::decode(const char* pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int32_t remained_length = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_nSourceUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int8(&ptmp, (uint8_t *)&m_iTransferType);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t *)&m_nDestUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_int16(&ptmp, (uint16_t *)&m_nDataSize);
		remained_length -= decoded_length;

		if (m_nDataSize > max_transparent_data_size)
		{
			return fail;
		}

		decoded_length = CCodeEngine::decode_memory(&ptmp, m_szTransparentData, (const int32_t)m_nDataSize);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

	CNotifyStatusUpdateToFriend::CNotifyStatusUpdateToFriend()
	{
		m_nDstUin    = -1;
		m_szDstAccount[0] = '\0';
	}

	CNotifyStatusUpdateToFriend::~CNotifyStatusUpdateToFriend()
	{

	}

	void CNotifyStatusUpdateToFriend::dump()
	{
		//do nothing
	}

	int32_t CNotifyStatusUpdateToFriend::encode(char *pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut || 0 > iOutLength)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_nDstUin);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_string(&ptmp, m_szDstAccount, sizeof(m_szDstAccount));
		iOutLength += coded_length;

		coded_length = encode_playerstatus_info(&ptmp, m_stPlayerStatus);
		iOutLength += coded_length;

		return success;
	}

	int32_t CNotifyStatusUpdateToFriend::decode(const char *pszIn, const int32_t iInLength)
	{
		if (NULL == pszIn || iInLength <= 0)
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remained_length  = iInLength;
		int32_t decoded_length = 0;

		decoded_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_nDstUin);
		remained_length -= decoded_length;

		decoded_length = CCodeEngine::decode_string(&ptmp, m_szDstAccount, sizeof(m_szDstAccount));
		remained_length -= decoded_length;

		decoded_length = decode_playerstatus_info(&ptmp, m_stPlayerStatus);
		remained_length -= decoded_length;

		if (remained_length < 0)
		{
			return fail;
		}

		return success;
	}

//////////////////////////////////////////////////////////////////////////

	CNotifyBroadcastNoticeInfo::CNotifyBroadcastNoticeInfo()
	{
		m_cNoticeType = 0;
		m_nBuffSize	  = 0;
	}

	CNotifyBroadcastNoticeInfo::~CNotifyBroadcastNoticeInfo()
	{
		//do nothing
	}

	void CNotifyBroadcastNoticeInfo::dump()
	{
		//do nothing
	}

	int32_t CNotifyBroadcastNoticeInfo::encode(char* pszOut, int32_t& iOutLength)
	{
		if(NULL == pszOut)
		{
			return fail;
		}

		if ((m_cNoticeType != notice_info_type_by_uin) && 
			(m_cNoticeType != notice_info_type_by_game_id) &&
			(m_cNoticeType != notice_info_type_all_hall))
		{
			return fail;
		}

		if (m_nBuffSize > max_transparent_data_size)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;
		iOutLength = 0;

		encode_length = CCodeEngine::encode_int8(&ptmp, m_cNoticeType);
		iOutLength += encode_length;

		if (notice_info_type_by_uin == m_cNoticeType)
		{
			if ((m_stDstInfo.m_stUIN.m_cCount <= 0) || (m_stDstInfo.m_stUIN.m_cCount > max_notice_uin_count))
			{
				return fail;
			}

			encode_length = CCodeEngine::encode_int8(&ptmp, m_stDstInfo.m_stUIN.m_cCount);
			iOutLength += encode_length;

			for (int8_t i=0; i<m_stDstInfo.m_stUIN.m_cCount; ++i)
			{
				encode_length = CCodeEngine::encode_int32(&ptmp, m_stDstInfo.m_stUIN.m_aiUIN[i]);
				iOutLength += encode_length;
			}
		}
		else if (notice_info_type_by_game_id == m_cNoticeType)
		{
			if ((m_stDstInfo.m_stGame.m_cCount <= 0) || (m_stDstInfo.m_stGame.m_cCount > max_notice_game_id_count))
			{
				return fail;
			}

			encode_length = CCodeEngine::encode_int8(&ptmp, m_stDstInfo.m_stGame.m_cCount);
			iOutLength += encode_length;

			for(int8_t i=0; i<m_stDstInfo.m_stGame.m_cCount; ++i)
			{
				encode_length = CCodeEngine::encode_int16(&ptmp, m_stDstInfo.m_stGame.m_anGameID[i]);
				iOutLength += encode_length;
			}
		}
		else
		{
			//do nothing
		}

		encode_length = CCodeEngine::encode_int16(&ptmp, m_nBuffSize);
		iOutLength += encode_length;

		if (m_nBuffSize > 0)
		{
			encode_length = CCodeEngine::encode_memory(&ptmp, m_szNoticeBuff, m_nBuffSize);
			iOutLength += encode_length;
		}

		return success;
	}

	int32_t CNotifyBroadcastNoticeInfo::decode(const char* pszIn, const int32_t iInLength)
	{
		if ((NULL == pszIn) || (iInLength <= 0))
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cNoticeType);
		remain_length -= decode_length;

		if ((m_cNoticeType != notice_info_type_by_uin) && 
			(m_cNoticeType != notice_info_type_by_game_id) &&
			(m_cNoticeType != notice_info_type_all_hall))
		{
			return fail;
		}

		if (notice_info_type_by_uin == m_cNoticeType)
		{
			decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stDstInfo.m_stUIN.m_cCount);
			remain_length -= decode_length;

			if ((m_stDstInfo.m_stUIN.m_cCount <= 0) || (m_stDstInfo.m_stUIN.m_cCount > max_notice_uin_count))
			{
				return fail;
			}

			for (int8_t i=0; i<m_stDstInfo.m_stUIN.m_cCount; ++i)
			{
				decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stDstInfo.m_stUIN.m_aiUIN[i]);
				remain_length -= decode_length;
			}
		}
		else if (notice_info_type_by_game_id == m_cNoticeType)
		{
			decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stDstInfo.m_stGame.m_cCount);
			remain_length -= decode_length;

			if ((m_stDstInfo.m_stGame.m_cCount <= 0) || (m_stDstInfo.m_stGame.m_cCount > max_notice_game_id_count))
			{
				return fail;
			}

			for(int8_t i=0; i<m_stDstInfo.m_stGame.m_cCount; ++i)
			{
				decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_stDstInfo.m_stGame.m_anGameID[i]);
				remain_length -= decode_length;
			}
		}
		else
		{
			//do nothing
		}

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nBuffSize);
		remain_length -= decode_length;
		if (m_nBuffSize > max_transparent_data_size)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_memory(&ptmp, m_szNoticeBuff, m_nBuffSize);
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

	CNotifyBroadcastNoticeToDstServer::CNotifyBroadcastNoticeToDstServer()
	{
		m_cNotifyType = -1;
		m_nBuffSize = 0;
	}

	CNotifyBroadcastNoticeToDstServer::~CNotifyBroadcastNoticeToDstServer()
	{

	}
	
	int32_t  CNotifyBroadcastNoticeToDstServer::encode(char *pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut || 0 > iOutLength)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t encode_length = 0;
		iOutLength = 0;

		encode_length = CCodeEngine::encode_int8(&ptmp, m_cNotifyType);
		iOutLength += encode_length;

		switch(m_cNotifyType)
		{
		case notify_to_hall:
		case notify_to_logic:
			break;

		case notify_to_logic_room:
			{
				if ((m_stDstInfo.m_stRooms.m_cCount <= 0) || (m_stDstInfo.m_stRooms.m_cCount > max_notify_room_id_count))
				{
					return fail;
				}

				encode_length = CCodeEngine::encode_int8(&ptmp, m_stDstInfo.m_stRooms.m_cCount);
				iOutLength += encode_length;

				for(int8_t i=0; i<m_stDstInfo.m_stRooms.m_cCount; ++i)
				{
					encode_length = CCodeEngine::encode_int32(&ptmp, m_stDstInfo.m_stRooms.m_aiRoom[i]);
					iOutLength += encode_length;
				}
			}
			break;

		default:
			return fail;
		}

		encode_length = CCodeEngine::encode_int16(&ptmp, m_nBuffSize);
		iOutLength += encode_length;

		if (m_nBuffSize > 0)
		{
			encode_length = CCodeEngine::encode_memory(&ptmp, m_szNoticeBuff, m_nBuffSize);
			iOutLength += encode_length;
		}

		return success;
	}

	int32_t  CNotifyBroadcastNoticeToDstServer::decode(const char *pszIn, const int32_t iInLength)
	{
		if ((NULL == pszIn) || (iInLength <= 0))
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cNotifyType);
		remain_length -= decode_length;

		switch(m_cNotifyType)
		{
		case notify_to_hall:
		case notify_to_logic:
			break;

		case notify_to_logic_room:
			{
				decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_stDstInfo.m_stRooms.m_cCount);
				remain_length -= decode_length;

				if ((m_stDstInfo.m_stRooms.m_cCount <= 0) || (m_stDstInfo.m_stRooms.m_cCount > max_notify_room_id_count))
				{
					return fail;
				}

				for(int8_t i=0; i<m_stDstInfo.m_stRooms.m_cCount; ++i)
				{
					decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_stDstInfo.m_stRooms.m_aiRoom[i]);
					remain_length -= decode_length;
				}
			}
			break;

		default:
			return fail;
		}

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_nBuffSize);
		remain_length -= decode_length;
		if (m_nBuffSize > max_transparent_data_size)
		{
			return fail;
		}

		decode_length = CCodeEngine::decode_memory(&ptmp, m_szNoticeBuff, m_nBuffSize);
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CNotifyBroadcastNoticeToDstServer::dump()
	{

	}

	CRequestRegisterToStateServer::CRequestRegisterToStateServer()
	{
		m_iServerID = -1;
		m_cServerType = -1;
	}

	CRequestRegisterToStateServer::~CRequestRegisterToStateServer()
	{

	}

	int32_t  CRequestRegisterToStateServer::encode(char *pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut || 0 > iOutLength)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cServerType);
		iOutLength += coded_length;

		return success;
	}

	int32_t  CRequestRegisterToStateServer::decode(const char *pszIn, const int32_t iInLength)
	{
		if ((NULL == pszIn) || (iInLength <= 0))
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cServerType);
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CRequestRegisterToStateServer::dump()
	{

	}


	CResponseRegisterToStateServer::CResponseRegisterToStateServer()
	{

	}

	CResponseRegisterToStateServer::~CResponseRegisterToStateServer()
	{

	}

	int32_t  CResponseRegisterToStateServer::encode(char *pszOut, int32_t& iOutLength)
	{
		if (NULL == pszOut || 0 > iOutLength)
		{
			return fail;
		}

		char* ptmp = pszOut;
		int32_t coded_length = 0;

		iOutLength = 0;

		coded_length = CCodeEngine::encode_int16(&ptmp, m_iResultID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int32(&ptmp, m_iServerID);
		iOutLength += coded_length;

		coded_length = CCodeEngine::encode_int8(&ptmp, m_cServerType);
		iOutLength += coded_length;

		return success;
	}

	int32_t  CResponseRegisterToStateServer::decode(const char *pszIn, const int32_t iInLength)
	{
		if ((NULL == pszIn) || (iInLength <= 0))
		{
			return fail;
		}

		char* ptmp = const_cast<char*>(pszIn);
		int16_t remain_length = iInLength;
		int32_t decode_length = 0;

		decode_length = CCodeEngine::decode_int16(&ptmp, (uint16_t*)&m_iResultID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int32(&ptmp, (uint32_t*)&m_iServerID);
		remain_length -= decode_length;

		decode_length = CCodeEngine::decode_int8(&ptmp, (uint8_t*)&m_cServerType);
		remain_length -= decode_length;

		if (remain_length < 0)
		{
			return fail;
		}

		return success;
	}

	void CResponseRegisterToStateServer::dump()
	{

	}
}}//namespace KingNet { namespace Server {

