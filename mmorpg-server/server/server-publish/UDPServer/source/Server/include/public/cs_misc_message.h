/**
@file   cs_misc_message.h
@brief  定义了php和misc server通讯的包头和包体结构

@author ankye@mail.51.com
@date Begin 2009.1
@date Dnd 2009.1
*/

#ifndef _CS_MISC_MESSAGE_H_
#define _CS_MISC_MESSAGE_H_

#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "public/server_message.h"
using namespace KingNet::Server;

namespace KingNet { namespace Server {


	enum
	{
		max_friend_list_count = 100,    //最大玩伴数100个
		max_friend_group_length = 19,   //最大好友分组名长度
		max_friend_group_list_count = 32, //最大好友分组数
	};
	typedef struct tagCSPlayerBaseInfo
	{
		int32_t  m_iUIN;		//UIN
		TAccount m_szAccount;	//accout
		int32_t m_iGroupFlag; //组标志
	}stCSPlayerBaseInfo;

	typedef struct tagCSFriendGroupInfo
	{
		int32_t m_iGroupMask;
		char m_szGroupName[max_friend_group_length];
	}stCSFriendGroupInfo;

	//更新身份证信息...
	class CCSRequestUpdatePlayerCardID:public CMessageBody
	{
	public:
		CCSRequestUpdatePlayerCardID();
		~CCSRequestUpdatePlayerCardID();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSRequestUpdatePlayerCardID& operator = (const CCSRequestUpdatePlayerCardID& original);

		int32_t m_iUin;
		TAccount m_szAccount;
		TIDCard m_szIDCard;
		char m_szUserName[max_player_name_length];//用户身份证的真实姓名...
	};

	class CCSResponseUpdatePlayerCardID:public CMessageBody
	{
	public:
		CCSResponseUpdatePlayerCardID();
		~CCSResponseUpdatePlayerCardID();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		CCSResponseUpdatePlayerCardID& operator = (const CCSResponseUpdatePlayerCardID& original);

		int32_t m_iUin;
		TAccount m_szAccount;
		int16_t m_nResultID;
	};
	//查询身份证信息
	class CCSRequestGetPlayerCardID:public CMessageBody
	{
	public:
		CCSRequestGetPlayerCardID();
		~CCSRequestGetPlayerCardID();
		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		int32_t m_iUin;
	};
	class CCSResponseGetPlayerCardID:public CMessageBody
	{
	public:
		CCSResponseGetPlayerCardID();
		~CCSResponseGetPlayerCardID();
		virtual int32_t  encode(char *pszOut, int32_t& iOutLength) ;
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength) ;
		virtual void dump() ;

		int32_t m_iUin;
		int16_t m_nResultID;  
		TIDCard m_szIDCard;				//身份证号

	};

	//拉取简单玩伴列表
	class CCSRequestGetProfileFriendList : public CMessageBody
	{
	public:
		CCSRequestGetProfileFriendList();
		~CCSRequestGetProfileFriendList();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		CCSRequestGetProfileFriendList& operator = (const CCSRequestGetProfileFriendList& original);
		virtual void dump();

	public:
		int32_t m_iMyUIN; //用户UIN
		int32_t m_iGroupMask;//用户的群组掩码
	};
	//获取简单的玩伴列表应答
	class CCSResponseGetProfileFriendList : public CMessageBody
	{
	public:
		CCSResponseGetProfileFriendList();
		~CCSResponseGetProfileFriendList();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //返回值，success为成功，否则为失败 
		int32_t m_iMyUIN;              //玩伴所属的UIN
		int16_t m_nFriendCount;        //玩伴的数量
		stCSPlayerBaseInfo m_astFriendInfo[max_friend_list_count];
	};

	//请求拉取好友分组
	class CCSRequestGetFriendGroup : public CMessageBody
	{
	public:
		CCSRequestGetFriendGroup();
		~CCSRequestGetFriendGroup();

		virtual int32_t encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int32_t m_iMyUIN;
	};

	//响应拉取好友分组
	class CCSResponseGetFriendGroup : public CMessageBody
	{
	public:
		CCSResponseGetFriendGroup();
		~CCSResponseGetFriendGroup();
		
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int8_t  m_cGroupCount;
		stCSFriendGroupInfo m_astGroup[max_friend_group_list_count];

	};

	//请求拉取群管权限信息
	typedef CCSRequestGetFriendGroup CCSRequestGetPlayerGroupData;
	
	//响应拉取群管权限信息
	class CCSResponseGetPlayerGroupData : public CMessageBody
	{
	public:
		CCSResponseGetPlayerGroupData();
		~CCSResponseGetPlayerGroupData();
		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();
	
	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int8_t  m_cGroupCount;
		stQunDetailInfo m_stGroupInfo[max_web_qun_count];
	};

}}//namespace KingNet { namespace Server 

#endif


