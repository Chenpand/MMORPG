#ifndef _SERVER_CHAT_MESSAGE_BODY_H_
#define _SERVER_CHAT_MESSAGE_BODY_H_

#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "public/server_message.h"

namespace KingNet { namespace Server {
	

	enum{
		max_addition_info_size = 32,
		max_chat_group_count = 50000,
	};
	enum publish_info_flag{
		publish_info = 1,
		no_publish_info = 0
	};

	typedef struct stChatGroup
	{
		int32_t m_iGroupID;
		int32_t m_iPlayerIndex1;
		int32_t m_iPlayerIndex2;
		int32_t m_iAdminIndex;
	}stChatGroup;

	//超时协议，系统内部使用
	class COtherTimeout : public CMessageBody
	{
	public:
		COtherTimeout();
		~COtherTimeout();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int16_t m_nTimerMark;
		int8_t m_cAdditionSize;	//m_stAdditionInfo的具体大小
		union _U
		{
			int8_t m_abyOtherInfo[max_addition_info_size/sizeof(int8_t)];
			int16_t m_anOtherInfo[max_addition_info_size/sizeof(int16_t)];
			int32_t m_aiOtherInfo[max_addition_info_size/sizeof(int32_t)];

		}m_stAdditionInfo;
	};
	//CS_MSG_LOGIN_CHAT_ROOM  登录聊天室
	class CCSRequestLoginChatRoom : public CMessageBody
	{
	public:
		CCSRequestLoginChatRoom();
		~CCSRequestLoginChatRoom();
		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestLoginChatRoom& operator = (const CCSRequestLoginChatRoom& original);

	public:
		int32_t m_iUid;
		int8_t  m_cSex;
		int8_t  m_cPublishInfoFlag;
		int16_t m_shPublishInfoLen;
		TUserBaseInfo m_pbyUserBaseInfo;

	};

	class CCSResponseLoginChatRoom : public CMessageBody
	{
	public:
		CCSResponseLoginChatRoom();
		~CCSResponseLoginChatRoom();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSResponseLoginChatRoom& operator = (const CCSResponseLoginChatRoom& original);
	public:
		int16_t m_shResultID;

		TReason m_szReason;

	};

	//CS_MSG_START_CHAT   发起聊天 
	class CCSRequestStartChat : public CMessageBody
	{
	public:
		CCSRequestStartChat();
		~CCSRequestStartChat();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestStartChat& operator = (const CCSRequestStartChat& original);
	public:
		int32_t m_iUid;
		int8_t  m_cDestSex;
		int32_t m_iChatContentType;
		int8_t	m_cPublishInfoFlag;
	};

	typedef class CCSResponseLoginChatRoom CCSResponseStartChat;


	
	//CS_MSG_NOTIFY_START_CHAT 通知开始聊天
	class CCSNotifyStartChat : public CMessageBody
	{
	public:
		CCSNotifyStartChat();
		~CCSNotifyStartChat();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSNotifyStartChat& operator = (const CCSNotifyStartChat& original);
	public:
		int32_t m_iSrcUid;
		int32_t m_iDestUid;
		int8_t	m_cDestSex;
		int8_t	m_cPublishInfoFlag;
		int16_t m_shPublishInfoLen;
		TUserBaseInfo m_pbyUserBaseInfo;
	};

	//CS_MSG_TRANSPORT_CHAT_INFO 中转聊天记录
	class CCSNotifyTransportChatInfo:public CMessageBody
	{
	public:
		CCSNotifyTransportChatInfo();
		~CCSNotifyTransportChatInfo();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSNotifyTransportChatInfo& operator = (const CCSNotifyTransportChatInfo& original);
	public:
		int32_t m_iSrcUid;
		int32_t m_iDestUid;
		int16_t m_shChatContentLen;
		TUserBaseInfo m_pbyChatContent;
	};


	//CS_MSG_PUBLISH_PRIVATE_INFO  公开个人信息
	class CCSRequestPublishPrivateInfo : public CMessageBody
	{
	public:
		CCSRequestPublishPrivateInfo();
		~CCSRequestPublishPrivateInfo();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestPublishPrivateInfo& operator = (const CCSRequestPublishPrivateInfo& original);
	public:
		int32_t m_iSrcUid;
		int32_t m_iDestUid;
		int8_t m_cPublishInfoFlag;
	};

	typedef class CCSResponseLoginChatRoom CCSResponsePublishPrivateInfo;

	class CCSNotifyPublishPrivateInfo : public CMessageBody
	{
	public:
		CCSNotifyPublishPrivateInfo();
		~CCSNotifyPublishPrivateInfo();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSNotifyPublishPrivateInfo& operator = (const CCSNotifyPublishPrivateInfo& original);
	public:
		int32_t m_iSrcUid;
		int32_t m_iDestUid;
		int8_t  m_cPublishInfoFlag;
		int16_t m_shPublishInfoLen;
		TUserBaseInfo m_pbyUserBaseInfo;
	};


	//CS_MSG_CHAT_HEART 聊天心跳包
	class CCSNotifyChatHeart:public CMessageBody
	{
	public:
		CCSNotifyChatHeart();
		~CCSNotifyChatHeart();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSNotifyChatHeart& operator = (const CCSNotifyChatHeart& original);
	public:
		int32_t m_iUid;
	};

	//CS_MSG_STOP_CHAT 退出聊天,TCP不断开
	class CCSRequestStopChat: public CMessageBody
	{
	public:
		CCSRequestStopChat();
		~CCSRequestStopChat();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestStopChat& operator = (const CCSRequestStopChat& original);
	public:
		int32_t m_iSrcUid;
		int32_t m_iDestUid;
	};

	typedef class CCSResponseLoginChatRoom CCSResponseStopChat;
	

	//CS_MSG_LOGOUT_CHAT_ROOM 退出聊天室，断开TCP
	class CCSRequestLogoutChatRoom : public CMessageBody
	{
	public:
		CCSRequestLogoutChatRoom();
		~CCSRequestLogoutChatRoom();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestLogoutChatRoom& operator = (const CCSRequestLogoutChatRoom& original);
	public:
		int32_t m_iSrcUid;
		int32_t m_iDestUid;
	};

	typedef class CCSResponseLoginChatRoom CCSResponseLogoutChatRoom;

	//CS_MSG_NOTIFY_CLOSE_CHAT 通知聊天结束
	class CCSNotifyCloseChat : public CMessageBody
	{
	public:
		CCSNotifyCloseChat();
		~CCSNotifyCloseChat();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSNotifyCloseChat& operator = (const CCSNotifyCloseChat& original);
	public:
		int32_t m_iUid;
		int16_t m_shReasonID;
		TReason m_szReason;
	};


	//CS_MSG_ADMIN_GET_CHAT_GROUPS

	class CCSRequestGetChatGroups : public CMessageBody
	{
	public:
		CCSRequestGetChatGroups();
		~CCSRequestGetChatGroups();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestGetChatGroups& operator = (const CCSRequestGetChatGroups& original);
	public:
		int32_t m_iUid;
		int32_t m_iChatContentType;
	};



	class CCSResponseGetChatGroups : public CMessageBody
	{
	public:
		CCSResponseGetChatGroups();
		~CCSResponseGetChatGroups();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSResponseGetChatGroups& operator = (const CCSResponseGetChatGroups& original);
	public:
		int32_t m_iGroupCount;
		stChatGroup m_szChatGroup[max_chat_group_count];
	};

	//CS_MSG_ADMIN_ADD_CHAT_GROUP

	class CCSRequestAddChatGroup : public CMessageBody
	{
	public:
		CCSRequestAddChatGroup();
		~CCSRequestAddChatGroup();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestAddChatGroup& operator = (const CCSRequestAddChatGroup& original);
	public:
		int32_t m_iUid;
		int32_t m_iGroupID;
	};

	typedef class CCSResponseLoginChatRoom CCSResponseAddChatGroup;
	

}}//namespace KingNet { namespace Server 

#endif


