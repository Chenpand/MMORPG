#ifndef _SERVER_GROUP_CHAT_MESSAGE_BODY_H_
#define _SERVER_GROUP_CHAT_MESSAGE_BODY_H_

#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "public/server_message.h"

namespace KingNet { namespace Server {
	

	enum{
		max_chat_room_count  = 500,
		max_player_count = 100,
		max_addition_info_size = 32,
	};
	enum
	{
		type_login_chat_room = 1,
		type_logout_chat_room = 2,
	};
	typedef struct stGroupRoom
	{
		int32_t m_iGroupID;
		int32_t m_iPlayerCount;
	}stGroupRoom;

	typedef struct stPlayer
	{
		int32_t m_iUid;
		int16_t m_shPublishInfoLen;
		TUserBaseInfo m_pbyUserBaseInfo;
	}stPlayer;
	////超时协议，系统内部使用
	//class COtherTimeout : public CMessageBody
	//{
	//public:
	//	COtherTimeout();
	//	~COtherTimeout();

	//	virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
	//	virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
	//	virtual void dump();

	//	int16_t m_nTimerMark;
	//	int8_t m_cAdditionSize;	//m_stAdditionInfo的具体大小
	//	union _U
	//	{
	//		int8_t m_abyOtherInfo[max_addition_info_size/sizeof(int8_t)];
	//		int16_t m_anOtherInfo[max_addition_info_size/sizeof(int16_t)];
	//		int32_t m_aiOtherInfo[max_addition_info_size/sizeof(int32_t)];

	//	}m_stAdditionInfo;
	//};
//	拉取聊天室人数
//		CS_MSG_GET_CHAT_ROOM_ONLINE_COUNT

	class CCSRequestGetChatRoomOnlineCount : public CMessageBody
	{
	public:
		CCSRequestGetChatRoomOnlineCount();
		~CCSRequestGetChatRoomOnlineCount();
		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

	public:
		int32_t m_iUid;
		int32_t m_iChatRoomIDCount;
		int32_t m_szChatRoomID[max_chat_room_count];
	};

	class CCSResponseGetChatRoomOnlineCount : public CMessageBody
	{
	public:
		CCSResponseGetChatRoomOnlineCount();
		~CCSResponseGetChatRoomOnlineCount();
		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSResponseGetChatRoomOnlineCount& operator = (const CCSResponseGetChatRoomOnlineCount& original);
	public:
		int16_t m_shResultID;
		//success
		int32_t m_iGameRoomCount;
		stGroupRoom m_szGroupRoom[max_chat_room_count];
		//fail
		TReason m_szReason;
		
	};
	//CS_MSG_LOGIN_CHAT_ROOM  登录聊天室
	class CCSRequestLoginGroupChatRoom : public CMessageBody
	{
	public:
		CCSRequestLoginGroupChatRoom();
		~CCSRequestLoginGroupChatRoom();
		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;
	public:
		int32_t m_iUid;
		int32_t m_iChatRoomID;
		int16_t m_shPublishInfoLen;
		TUserBaseInfo m_pbyUserBaseInfo;

	};

	class CCSResponseLoginGroupChatRoom : public CMessageBody
	{
	public:
		CCSResponseLoginGroupChatRoom();
		~CCSResponseLoginGroupChatRoom();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

	public:
		int16_t m_shResultID;
		//success
		int32_t m_iChatRoomID;
		int16_t m_shPlayerCount;
		stPlayer m_szPlayer[max_player_count];
		//fail
		TReason m_szReason;

	};



	
	//CS_MSG_NOTIFY_LOGIN_LOGOUT_ROOM 通知某人进聊天室或者退出聊天室
	class CCSNotifyLoginLogoutChatRoom : public CMessageBody
	{
	public:
		CCSNotifyLoginLogoutChatRoom();
		~CCSNotifyLoginLogoutChatRoom();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;
	public:
		int32_t m_iSrcUid;
		int8_t	m_cType;
		int16_t m_shPublishInfoLen;
		TUserBaseInfo m_pbyUserBaseInfo;
	};

	//CS_MSG_TRANSPORT_CHAT_INFO 中转聊天记录
	class CCSNotifyTransportGroupChatInfo:public CMessageBody
	{
	public:
		CCSNotifyTransportGroupChatInfo();
		~CCSNotifyTransportGroupChatInfo();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

	public:
		int32_t m_iSrcUid;
		int32_t m_iChatRoomID;
		int16_t m_shChatContentLen;
		TChatContent m_pbyChatContent;
		int16_t m_shPublishInfoLen;
		TUserBaseInfo m_pbyUserBaseInfo;
	};





	//CS_MSG_STOP_CHAT 退出聊天,TCP不断开
	class CCSRequestStopGroupChat: public CMessageBody
	{
	public:
		CCSRequestStopGroupChat();
		~CCSRequestStopGroupChat();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;
	public:
		int32_t m_iSrcUid;
		int32_t m_iChatRoomID;
	};

	class CCSResponseStopGroupChat : public CMessageBody
	{
	public:
		CCSResponseStopGroupChat();
		~CCSResponseStopGroupChat();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

	public:
		int16_t m_shResultID;

		TReason m_szReason;

	};
	

	//CS_MSG_LOGOUT_CHAT_ROOM 退出聊天室，断开TCP
	class CCSRequestLogoutGroupChatRoom : public CMessageBody
	{
	public:
		CCSRequestLogoutGroupChatRoom();
		~CCSRequestLogoutGroupChatRoom();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

	public:
		int32_t m_iSrcUid;
		int32_t m_iChatRoomID;
	};

	
	typedef class CCSResponseStopGroupChat CCSResponseLogoutGroupChatRoom;





	

}}//namespace KingNet { namespace Server 

#endif


