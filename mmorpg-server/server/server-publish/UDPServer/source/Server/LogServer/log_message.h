#ifndef __LogServer_MESSAGE_H__
#define __LogServer_MESSAGE_H__

#include "Common/common_namespace_definition.h"
#include "gameplatform_types.h"
#include "Common/common_object.h"
#include "gameplatform_message.h"

#include "public/server_message.h"
#include "public/cs_head.h"

namespace KingNet { namespace Server { namespace LogServer{


	enum enmOtherMessageType
	{
		////server <--> connector server相应内部协议
		message_other_start_request = 1,			//enmCtrMsgType_Start_Req		c->s		
		message_other_transform_request ,			//enmCtrMsgType_Transform_Req   c<->s
		message_other_force_close_request,			//enmCtrMsgType_ForceCloseConnect_Req s->c
		message_other_disconnect_notify,			//enmCtrMsgType_DisconnectNotify_Req  c->s
		message_other_grouptransform_quest,			//enmCtrMsgType_GroupTransform_Req    c<->s

		//
		message_other_timeout = 100,				//时钟超时
	};


	class CMessage
	{
	public:
		CMessage();
		~CMessage();

		int32_t  encode(char *pszOut, int32_t& iOutLength);
		int32_t  decode(const char *pszIn, const int32_t iInLength);
		void dump();


		int32_t create_message_body();
		static CMessageBody* create_body(int32_t iMessageID, int8_t iMessageType);
		static CMessageBody* create_request_message_body(int32_t iMessageID);
		static CMessageBody* create_response_message_body(int32_t iMessageID);
		static CMessageBody* create_notify_message_body(int32_t iMessageID);
		static CMessageBody* create_other_message_body(int32_t iMessageID);

	public:

		CMessageHead m_stHead;
		CMessageBody* m_pstBody;
	};


	class CMessageEntry : public CObject
	{
	public:
		CMessageEntry();
		~CMessageEntry();

		int32_t initialize();
		int32_t resume();

		virtual int32_t get_key(void* key, int32_t& keylength);
		virtual int32_t set_key(const void*key, int32_t keylength);

		virtual int32_t dump(int32_t handle);

	public:
		int16_t m_iMessageID;
		int8_t  m_nMessageType;
		int8_t  m_bSourceEntityType;
		int8_t  m_bDestEntityType;
		int32_t m_iSourceID;
		int32_t m_iDestID;
		int16_t m_shVersionType;
		int32_t m_iMessageSequence;

		int32_t m_iBodySize;
		int32_t m_iBodyOffsetInQueue;

	private:
		DECLARE_DYN_CREATE
	};

}}}//namespace KingNet { namespace Server { namespace LogServer{

#endif // __LogServer_MESSAGE_H__
