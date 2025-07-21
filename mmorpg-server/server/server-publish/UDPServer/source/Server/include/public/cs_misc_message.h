/**
@file   cs_misc_message.h
@brief  ������php��misc serverͨѶ�İ�ͷ�Ͱ���ṹ

@author ankye@mail.51.com
@date Begin 2009.1
@date Dnd 2009.1
*/

#ifndef _CS_MISC_MESSAGE_H_
#define _CS_MISC_MESSAGE_H_

#include "Common/common_base.h"  // for (fail/sucess)�ȶ���
#include "public/server_message.h"
using namespace KingNet::Server;

namespace KingNet { namespace Server {


	enum
	{
		max_friend_list_count = 100,    //��������100��
		max_friend_group_length = 19,   //�����ѷ���������
		max_friend_group_list_count = 32, //�����ѷ�����
	};
	typedef struct tagCSPlayerBaseInfo
	{
		int32_t  m_iUIN;		//UIN
		TAccount m_szAccount;	//accout
		int32_t m_iGroupFlag; //���־
	}stCSPlayerBaseInfo;

	typedef struct tagCSFriendGroupInfo
	{
		int32_t m_iGroupMask;
		char m_szGroupName[max_friend_group_length];
	}stCSFriendGroupInfo;

	//�������֤��Ϣ...
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
		char m_szUserName[max_player_name_length];//�û����֤����ʵ����...
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
	//��ѯ���֤��Ϣ
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
		TIDCard m_szIDCard;				//���֤��

	};

	//��ȡ������б�
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
		int32_t m_iMyUIN; //�û�UIN
		int32_t m_iGroupMask;//�û���Ⱥ������
	};
	//��ȡ�򵥵�����б�Ӧ��
	class CCSResponseGetProfileFriendList : public CMessageBody
	{
	public:
		CCSResponseGetProfileFriendList();
		~CCSResponseGetProfileFriendList();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t m_iMyUIN;              //���������UIN
		int16_t m_nFriendCount;        //��������
		stCSPlayerBaseInfo m_astFriendInfo[max_friend_list_count];
	};

	//������ȡ���ѷ���
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

	//��Ӧ��ȡ���ѷ���
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

	//������ȡȺ��Ȩ����Ϣ
	typedef CCSRequestGetFriendGroup CCSRequestGetPlayerGroupData;
	
	//��Ӧ��ȡȺ��Ȩ����Ϣ
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


