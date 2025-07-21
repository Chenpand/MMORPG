/**
@file   cs_gm_message.h
@brief  定义了gameadm.51.com php 和server通讯的包头和包体结构

@author sjij@mail.51.com
@date Begin 2008.10
@date Dnd 2008.10
*/

#ifndef _CS_GM_MESSAGE_H_
#define _CS_GM_MESSAGE_H_

#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "public/server_message.h"

using namespace KingNet::Server;

namespace KingNet { namespace Server {

//const int32_t MAX_CS_FILE_CONTENT_LENGTH = 65536;
/*
const int32_t MAX_COMMODITY_SIZE = 15;
const int32_t MAX_FAILED_MESSAGE_LENGTH = 256;

const int32_t MAX_WORD_LENGTH = 1024;
const int16_t MAX_USER_WORD_LENGTH = 600;
*/

const int32_t MAX_PUNISH_REASON_LEN = 2048;


	/**	
	@brief  gameadm.51.com发送的禁言或者封账号请求协议
	*/
	class  CCSRequestPunish:public CMessageBody
	{
	public:
		CCSRequestPunish();
		~CCSRequestPunish();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestPunish& operator = (const CCSRequestPunish& original);

	public:
		
		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		int8_t   m_byPunishType;
		
		char m_szReason[MAX_PUNISH_REASON_LEN];

		int32_t m_iRequestTimestamp;
		int32_t m_iPunishEndTimestamp;
	private:
	};

	
	/**	
	@brief  gameadm.51.com发送的禁言或者封账号响应协议
	*/
	class  CCSResponsePunish:public CMessageBody
	{
	public:
		CCSResponsePunish();
		~CCSResponsePunish();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;
		char m_szFailReason[256];
	};
	

	/**	
	@brief  gameadm.51.com发送系统消息请求协议
	*/
	class  CCSRequestSendSystemMessage:public CMessageBody
	{
	public:
		CCSRequestSendSystemMessage();
		~CCSRequestSendSystemMessage();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestSendSystemMessage& operator = (const CCSRequestSendSystemMessage& original);

	public:

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;
		char m_szMessageContent[max_system_message_length];
		int32_t m_iRequestTimestamp;
	private:
	};


	/**	
	@brief  gameadm.51.com发送系统消息响应协议
	*/
	class  CCSResponseSendSystemMessage:public CMessageBody
	{
	public:
		CCSResponseSendSystemMessage();
		~CCSResponseSendSystemMessage();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;
		TReason m_szFailReason;
	};

	//GM Server VS (Hall Server & Logic Server)
	

	struct stSystemMessage 
	{
		int32_t m_iLowVersion;
		int32_t m_iHighVersion;
		int16_t m_nSize;
		char m_szMessage[max_system_message_length];
	};

	enum 
	{
		max_system_message_count	= 6,				//最大能提交的系统消息数
		max_system_message_version_count = 3,			//最多每条系统消息支持的不同版本数目
	};

	int32_t encode_system_message(char**pOut, stSystemMessage& system_message);
	int32_t decode_system_message(char**pOut, stSystemMessage& system_message);

	//SS_MSG_RELAY_SYSTEM_MESSAGE
	class CRequestRelaySystemMessage : public CMessageBody
	{
	public:
		CRequestRelaySystemMessage();
		~CRequestRelaySystemMessage();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		RoomID m_iRoomID;
		GameID m_nGameID;
        int32_t m_iMessageID;

        int8_t m_cCount;
		stSystemMessage m_stSystemMessage[max_system_message_version_count];

	protected:
	private:
	};

	class CResponseRelaySystemMessage : public CMessageBody
	{
	public:
		CResponseRelaySystemMessage();
		~CResponseRelaySystemMessage();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		int16_t m_nResultID;
		RoomID m_iRoomID;
		GameID m_nGameID;
        int32_t m_iMessageID;
		TReason m_szResultString;
	protected:
	private:
	};

	//发送时机
	enum enmPushTrigger
	{
		enmPushTrigger_null				= 0x00,
		enmPushTrigger_enter_room		= 0x01,
		enmPushTrigger_login_platform	= 0x02,
		
		//to be continue ...
	};

	//SS_MSG_SUBMIT_SYSTEM_MESSAGE
	

	//SS_MSG_SUBMIT_SYSTEM_MESSAGE
	class CRequestSubmitSystemMessage : public CMessageBody
	{
	public:
		CRequestSubmitSystemMessage();
		~CRequestSubmitSystemMessage();
	
		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		RoomID m_iRoomID;
		int8_t m_cPushTrigger;
		int8_t m_cMessageIndex;
		int16_t m_nGameID;
        int32_t m_iMessageID;

		int8_t m_cCount;
		stSystemMessage m_astSystemMessages[max_system_message_version_count];


	protected:
	private:
	};

	class CResponseSubmitSystemMessage : public CMessageBody
	{
	public:
		CResponseSubmitSystemMessage();
		~CResponseSubmitSystemMessage();


		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		int16_t m_nResultID;
		RoomID m_iRoomID;
		int8_t m_cPushTrigger;
		int8_t m_cMessageIndex;
		int16_t m_nGameID;
        int32_t m_iMessageID;

		TReason m_szResultString;
	protected:
	private:
	};

    //SS_MSG_CANCEL_MESSAGE
    class CRequestCancelMessage : public CMessageBody
    {
    public:
        CRequestCancelMessage();
        ~CRequestCancelMessage();

        int32_t encode(char *pszOut, int32_t& iOutLength);
        int32_t decode(const char *pszIn, const int32_t iInLength);
        void dump();

        int32_t m_iMessageID;
        RoomID m_iRoomID;
        int16_t m_nGameID;

    protected:
    private:
    };

    class CResponseCancelMessage : public CMessageBody
    {
    public:
        CResponseCancelMessage();
        ~CResponseCancelMessage();

        int32_t encode(char *pszOut, int32_t& iOutLength);
        int32_t decode(const char *pszIn, const int32_t iInLength);
        void dump();

        int16_t m_nResultID;
        int32_t m_iMessageID;
        RoomID m_iRoomID;
        int16_t m_nGameID;
        TReason m_szResultString;

    protected:
    private:
    };

	//SS_MSG_CANCEL_SYSTEM_MESSAGE
	class CRequestCancelSystemMessage : public CMessageBody
	{
	public:
		CRequestCancelSystemMessage();
		~CRequestCancelSystemMessage();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();
		
		RoomID m_iRoomID;
		int8_t m_cPushTrigger;
		int8_t m_cMessageIndex;
		int16_t m_nGameID;


	protected:
	private:
	};

	class CResponseCancelSystemMessage : public CMessageBody
	{
	public:
		CResponseCancelSystemMessage();
		~CResponseCancelSystemMessage();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		int16_t m_nResultID;
		RoomID m_iRoomID;
		int8_t m_cPushTrigger;
		int8_t m_cMessageIndex;
		int16_t m_nGameID;
		TReason m_szResultString;

	protected:
	private:
	};


    //SS_MSG_SEND_SYSTEM_MESSAGE_TO_PLAYER
    class CRequestSendSystemMessageToPlayer : public CMessageBody
    {
    public:
        CRequestSendSystemMessageToPlayer();
        ~CRequestSendSystemMessageToPlayer();

        int32_t encode(char *pszOut, int32_t& iOutLength);
        int32_t decode(const char *pszIn, const int32_t iInLength);
        void dump();

        int32_t m_iUin;
        int32_t m_iMessageID;

        int8_t m_cCount;
        stSystemMessage m_stSystemMessage[max_system_message_version_count];


    protected:
    private:
    };

    class CResponseSendSystemMessageToPlayer : public CMessageBody
    {
    public:
        CResponseSendSystemMessageToPlayer();
        ~CResponseSendSystemMessageToPlayer();

        int32_t encode(char *pszOut, int32_t& iOutLength);
        int32_t decode(const char *pszIn, const int32_t iInLength);
        void dump();

        int16_t m_nResultID;
        int32_t m_iUin;
        int32_t m_iMessageID;
        TReason m_szResultString;

    protected:
    private:
    };


}}//namespace KingNet { namespace Server

#endif

