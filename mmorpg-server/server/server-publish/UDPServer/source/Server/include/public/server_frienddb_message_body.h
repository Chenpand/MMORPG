#ifndef _FRIEND_DB_SERVER_MESSAGE_BODY_H_
#define _FRIEND_DB_SERVER_MESSAGE_BODY_H_

#include "public/server_message.h"

namespace KingNet { namespace Server {

        
#define MAX_TAG_NAME_LENGTH         16      //��ǩ��������󳤶�
#define MAX_POSITIVE_FRIEND_COUNT   100     //һ���û����������������
#define MAX_REVERSE_FRIEND_COUNT    1000    //һ���û����������������
#define MAX_TAG_COUNT_PER_FRIEND    1       //��һ�������ǩ���������
#define MAX_RESPONSE_FRIEND_COUNT	100		//һ�η���Ӧ��ʱ�������������ͨ�Ű�64K������
#define INVALID_TAG_ID				0		//��Ч�ı�ǩID
#define MAX_FRIEND_REMARKS_LENGTH	16		//��鱸ע������󳤶ȣ�15+1��
#define MAX_GROUP_NAME_LENGTH		19		//������������󳤶ȣ�18+1��
#define MAX_FRIEND_GROUP_COUNT		32		//һ��������32��������
#define MAX_PLAY_WITH_COUNT			20		//��˭һ�������������������20
#define MAX_STAT_TAG_COUNT			30		//ͳ�Ʊ�ǩ���30��
#define MAX_BLACKLIST_PLAYER_COUNT	100		//�������е������100��


	enum enmFriendGroupIndex
	{
		system_group_index_begin = 0,
		system_group_index_end   = 7,
		custom_group_index_begin = 8,
		custom_group_index_end   = 18,
		max_custom_group_count   = (custom_group_index_end - custom_group_index_begin),

		custom_group_mask_begin = (1 << custom_group_index_begin),
		custom_group_mask_end	= (1 << custom_group_index_end),
	};
	      

	//����������ϵı�ǩ��Ϣ
	class CTagInfo
	{
	public:
		CTagInfo();
		~CTagInfo();

		CTagInfo& operator= (const CTagInfo& src);
		bool valid_tag_id();
		//�������
		void clear();

	public:
		int32_t     m_iTagID;      //��ǩID��Ψһ��ʶһ����ǩ
		char        m_szTagName[MAX_TAG_NAME_LENGTH];        //��ǩ����
		int32_t     m_iTimestamp;   //���ñ�ǩ��ʱ��������ֶβ���������
	};

	//�����Ϣ
	class CFriendInfo
	{
	public:
		CFriendInfo();
		~CFriendInfo();

		CFriendInfo& operator= (const CFriendInfo& src);
		void clear();

	public:
		int32_t  m_iFriendUIN;			//����UIN
		TAccount m_szFriendAccount;		//����accout
		int32_t  m_iTimestamp;			//���ֶβ����б���
		int16_t  m_nTagCount;			//��ǩ����
		CTagInfo m_astTag[MAX_TAG_COUNT_PER_FRIEND];
		char     m_szRemarks[MAX_FRIEND_REMARKS_LENGTH];
		int32_t  m_iGroupFlag;
	};

	//�û�������ǩ��ͳ����Ϣ
	class CStatTagInfo
	{
	public:
		CStatTagInfo();
		~CStatTagInfo();

		CStatTagInfo& operator= (const CStatTagInfo& src);
		bool operator< (const CStatTagInfo& tag2);
		void clear();

	public:
		int32_t	m_iTagID;       //��ǩID��Ψһ��ʶһ����ǩ
		char    m_szTagName[MAX_TAG_NAME_LENGTH];        //��ǩ����
		int32_t m_iCount;       //�����ñ�ǩ��������
		int32_t m_iTimestamp;   //������ñ�ǩ��ʱ���
	};

	class CPlayerBaseInfo
	{
	public:
		CPlayerBaseInfo();
		~CPlayerBaseInfo();

		CPlayerBaseInfo& operator= (const CPlayerBaseInfo& src);
		void clear();

	public:
		int32_t  m_iUIN;		//UIN
		TAccount m_szAccount;	//�˺�
		int32_t  m_iGroupFlag;	//����Ϣ
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//������������б�
	//SS_MSG_GET_POSITIVE_FRIEND_LIST
	class CRequestPositiveFriendList : public CMessageBody
	{
	public:
		CRequestPositiveFriendList();
		~CRequestPositiveFriendList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t m_iUIN;			//UIN
		int32_t m_iDestUIN;		//Ŀ����ҵ�Uin
	};

	//��������б�Ӧ��
	class CResponsePositiveFriendList : public CMessageBody
	{
	public:
		CResponsePositiveFriendList();
		~CResponsePositiveFriendList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;           //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t     m_iMyUIN;              //����б������ߵ�UIN
		int16_t     m_nFriendCount;        //��������
		CFriendInfo m_astFriendInfo[MAX_RESPONSE_FRIEND_COUNT];  //������ϸ��Ϣ

		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//state server������������б�
	typedef CRequestPositiveFriendList CRequestReverseList;

	//��������б�Ӧ��
	class CResponseReverseList : public CMessageBody
	{
	public:
		CResponseReverseList();
		~CResponseReverseList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t m_iMyUIN;              //�Լ���UIN
		int16_t m_nFriendCount;        //��������
		int32_t m_aiFriendUIN[MAX_REVERSE_FRIEND_COUNT];  //����UIN
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//�����������˵ı�����ǩͳ����Ϣ
	//SS_MSG_GET_SOMEONE_TAG_COUNT
	typedef CRequestPositiveFriendList CRequestTagStatisticalInfo;

	//��������ǩͳ����Ϣ��Ӧ��
	class CResponseTagStatisticalInfo : public CMessageBody
	{
	public:
		CResponseTagStatisticalInfo();
		~CResponseTagStatisticalInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;	//����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t     m_iMyUIN;       //�������Լ���UIN	
		int32_t     m_iDestUIN;		//Ŀ����ҵ�UIN
		int16_t     m_nTagCount;	//��ǩ����
		CStatTagInfo m_astTag[MAX_STAT_TAG_COUNT];

		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//������������ǩ
	class CRequestAddTagToFriend : public CMessageBody
	{
	public:
		CRequestAddTagToFriend();
		~CRequestAddTagToFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t		m_iMyUIN;			//�Լ���UIN
		TAccount	m_szMyAccount;		//�Լ���accout
		int32_t		m_iFriendUIN;		//����UIN
		TAccount	m_szFriendAccount;	//����account
		int32_t		m_iTagID;			//��ǩID��Ψһ��ʶһ����ǩ
		char		m_szTagName[MAX_TAG_NAME_LENGTH];	//��ǩ����
	};

	//���������ǩ��Ӧ��
	class CResponseAddTagToFriend : public CMessageBody
	{
	public:
		CResponseAddTagToFriend();
		~CResponseAddTagToFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iDestUIN;
		int32_t m_iTagID;
		char m_szTagName[MAX_TAG_NAME_LENGTH];

		TReason m_szReasonMessage;
	};

	//���������� SS_MSG_ADD_REVERSE_TAG
	typedef CRequestAddTagToFriend CRequestAddReverseTag;
	typedef CResponseAddTagToFriend CResponseAddReverseTag;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//����ɾ�����
	class CRequestDeleteFriend : public CMessageBody
	{
	public:
		CRequestDeleteFriend();
		~CRequestDeleteFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t	m_iMyUIN;     //�Լ���UIN
		int32_t	m_iFriendUIN; //����UIN
	};

	//ɾ������Ӧ��
	class CResponseDeleteFriend : public CMessageBody
	{
	public:
		CResponseDeleteFriend();
		~CResponseDeleteFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;             //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t m_iMyUIN;              //�������Լ���UIN	
		int32_t m_iFriendUIN;          //����UIN
		TReason m_szReasonMessage;
	};

	enum enmNotifyFriendInfoControlCMD
	{
		update_friend_info = 1,
		delete_friend	   = 2,
		add_friend		   = 3,
	};

	//SS_MSG_NOTIFY_FRIEND_INFO  ȫ��֪ͨ��������tag��Ϣ notify
	class CNotifyFriendInfo :public CMessageBody
	{
	public:
		CNotifyFriendInfo();
		~CNotifyFriendInfo();
		
		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iMyUIN;
		int16_t m_nControlCMD;
		CFriendInfo m_stFriend;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int32_t encode_taginfo(char** pszOut, CTagInfo& taginfo);
	int32_t decode_taginfo(char** pszIn, CTagInfo& taginfo);

	int32_t encode_friendinfo(char** pszOut, CFriendInfo& friendinfo);
	int32_t decode_friendinfo(char** pszIn, CFriendInfo& friendinfo);

	int32_t encode_tag_statistics(char** pszOut, CStatTagInfo& statistic);
	int32_t decode_tag_statistics(char** pszIn, CStatTagInfo& statistic);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��ȡ�򵥵�����б�ֻ�к��ѵ�UIN
	typedef CRequestPositiveFriendList CRequestSimpleFriendList;

	//��ȡ�򵥵�����б�Ӧ��
	class CResponseSimpleFriendList : public CMessageBody
	{
	public:
		CResponseSimpleFriendList();
		~CResponseSimpleFriendList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t m_iMyUIN;              //���������UIN
		int16_t m_nFriendCount;        //��������
		int32_t m_aiFriendUIN[MAX_POSITIVE_FRIEND_COUNT];  //����UIN
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��������������
	class CRequestAddGroup : public CMessageBody
	{
	public:
		CRequestAddGroup();
		~CRequestAddGroup();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t  m_iMyUIN;              
		TAccount m_szMyAccount;
		char     m_szGroupName[MAX_GROUP_NAME_LENGTH];	//��������
	};

	//����������Ӧ��
	class CResponseAddGroup : public CMessageBody
	{
	public:
		CResponseAddGroup();
		~CResponseAddGroup();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iGroupMask;
		char    m_szGroupName[MAX_GROUP_NAME_LENGTH];
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//ɾ�������������
	class CRequestDeleteGroup : public CMessageBody
	{
	public:
		CRequestDeleteGroup();
		~CRequestDeleteGroup();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t m_iMyUIN;              
		int32_t m_iGroupMask;
	};

	//ɾ���������Ӧ��
	class CResponseDeleteGroup : public CMessageBody
	{
	public:
		CResponseDeleteGroup();
		~CResponseDeleteGroup();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iGroupMask;
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//������Ƶ�ĳ�����������
	class CRequestUpdateFriendGroupInfo : public CMessageBody
	{
	public:
		CRequestUpdateFriendGroupInfo();
		~CRequestUpdateFriendGroupInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t  m_iMyUIN;
		TAccount m_szMyAccount;
		int32_t  m_iFriendUIN;
		TAccount m_szFriendAccount;
		int32_t  m_iGroupFlag;
	};

	//������Ƶ�ĳ�������Ӧ��
	class CResponseUpdateFriendGroupInfo : public CMessageBody
	{
	public:
		CResponseUpdateFriendGroupInfo();
		~CResponseUpdateFriendGroupInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iFriendUIN;
		int32_t m_iGroupFlag;
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��������ĳ�����������
	class CRequestGroupRename : public CMessageBody
	{
	public:
		CRequestGroupRename();
		~CRequestGroupRename();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t m_iMyUIN;
		int32_t m_iGroupMask;
		char    m_szGroupName[MAX_GROUP_NAME_LENGTH];
	};

	//��������ĳ�������Ӧ��
	class CResponseGroupRename : public CMessageBody
	{
	public:
		CResponseGroupRename();
		~CResponseGroupRename();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iGroupMask;
		char    m_szGroupName[MAX_GROUP_NAME_LENGTH];
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��ȡ�����������Ϣ������
	typedef CRequestPositiveFriendList CRequestGetGroupInfo;

	class CGroupInfo
	{
	public:
		CGroupInfo();
		~CGroupInfo();

		CGroupInfo& operator= (const CGroupInfo& src);
		void clear();

	public:
		int32_t m_iGroupMask;
		char    m_szGroupName[MAX_GROUP_NAME_LENGTH];
		bool    m_bUsed;		//���ֶβ������
	};

	int32_t encode_group_info(char** pszOut, CGroupInfo& group_info);
	int32_t decode_group_info(char** pszIn, CGroupInfo& group_info);

	//��ȡ�����������Ϣ��Ӧ��
	class CResponseGetGroupInfo : public CMessageBody
	{
	public:
		CResponseGetGroupInfo();
		~CResponseGetGroupInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int8_t  m_cGroupCount;
		CGroupInfo m_astGroup[MAX_FRIEND_GROUP_COUNT];
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//������鱸ע������
	class CRequestUpdateFriendRemarks : public CMessageBody
	{
	public:
		CRequestUpdateFriendRemarks();
		~CRequestUpdateFriendRemarks();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t  m_iMyUIN;
		TAccount m_szMyAccount;
		int32_t  m_iFriendUIN;
		TAccount m_szFriendAccount;
		char     m_szRemarks[MAX_FRIEND_REMARKS_LENGTH];
	};

	//������鱸ע��Ӧ��
	class CResponseUpdateFriendRemarks : public CMessageBody
	{
	public:
		CResponseUpdateFriendRemarks();
		~CResponseUpdateFriendRemarks();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iFriendUIN;
		char    m_szRemarks[MAX_FRIEND_REMARKS_LENGTH];
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��ȡ��Һ�˭�����Ϣ������
	typedef CRequestPositiveFriendList CRequestGetPlayWithInfo;

	class CPlayWithInfo
	{
	public:
		CPlayWithInfo();
		~CPlayWithInfo();
		CPlayWithInfo& operator= (const CPlayWithInfo& src);
		void clear();

	public:
		int32_t  m_iFriendUIN;
		TAccount m_szFriendAccount;
		int16_t  m_nGameID;
		int32_t  m_iTimestamp;
	};

	int32_t encode_play_with_info(char** pszOut, CPlayWithInfo& info);
	int32_t decode_play_with_info(char** pszIn, CPlayWithInfo& info);

	//��ȡ��Һ�˭�����Ϣ��Ӧ��
	class CResponseGetPlayWithInfo : public CMessageBody
	{
	public:
		CResponseGetPlayWithInfo();
		~CResponseGetPlayWithInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int8_t  m_cCount;
		CPlayWithInfo m_astInfo[MAX_PLAY_WITH_COUNT];
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//��Ӻ�˭�����Ϣ������
	class CRequestAddPlayWithInfo : public CMessageBody
	{
	public:
		CRequestAddPlayWithInfo();
		~CRequestAddPlayWithInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t  m_iMyUIN;
		TAccount m_szMyAccount;
		int32_t  m_iFriendUIN;
		TAccount m_szFriendAccount;
		int16_t  m_nGameID;
	};

	//��Ӻ�˭�����Ϣ��Ӧ��
	class CResponseAddPlayWithInfo : public CMessageBody
	{
	public:
		CResponseAddPlayWithInfo();
		~CResponseAddPlayWithInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		CPlayWithInfo m_stInfo;
		TReason m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//֪ͨ��˭�����Ϣ������
	class CNotifyDelPlayWithInfo : public CMessageBody
	{
	public:
		CNotifyDelPlayWithInfo();
		~CNotifyDelPlayWithInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t m_iMyUIN;
		CPlayWithInfo m_stInfo;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class CRequestAddFriend : public CMessageBody
	{
	public:
		CRequestAddFriend();
		~CRequestAddFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t  m_iMyUIN;
		TAccount m_szMyAccount;
		int32_t  m_iFriendUIN;
		TAccount m_szFriendAccount;
		int32_t  m_iTagID;
		char     m_szTagName[MAX_TAG_NAME_LENGTH];
		char     m_szRemarks[MAX_FRIEND_REMARKS_LENGTH];
		int32_t  m_iGroupFlag;
		int32_t  m_iGameID;
	};

	class CResponseAddFriend : public CMessageBody
	{
	public:
		CResponseAddFriend();
		~CResponseAddFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iFriendUIN;
		int32_t m_iTagID;
		char    m_szTagName[MAX_TAG_NAME_LENGTH];
		char    m_szRemarks[MAX_FRIEND_REMARKS_LENGTH];
		int32_t m_iGroupFlag;
		TReason m_szReasonMessage;
	};

//////////////////////////////////////////////////////////////////////////

	//��ȡ�򵥵�����б�ֻ�к��ѵ�UIN
	typedef CRequestPositiveFriendList CRequestFriendBaseInfo;

	//��ȡ�򵥵�����б�Ӧ��
	class CResponseFriendBaseInfo : public CMessageBody
	{
	public:
		CResponseFriendBaseInfo();
		~CResponseFriendBaseInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t m_iMyUIN;              //���������UIN
		int16_t m_nFriendCount;        //��������
		CPlayerBaseInfo m_astFriendInfo[MAX_POSITIVE_FRIEND_COUNT];
	};


//////////////////////////////////////////////////////////////////////////

	struct BlackListInfo 
	{
		int32_t  m_iUIN;
		TAccount m_szAccount;

		BlackListInfo& operator= (const BlackListInfo& src);
	};
	
	int32_t encode_black_list_info(char** pszOut, BlackListInfo& info);
	int32_t decode_black_list_info(char** pszIn, BlackListInfo& info);

	typedef CRequestPositiveFriendList CRequestBlackList;

	class CResponseBlackList : public CMessageBody
	{
	public:
		CResponseBlackList();
		~CResponseBlackList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;	//����ֵ��successΪ�ɹ�������Ϊʧ�� 
		int32_t m_iMyUIN;		//������������UIN
		int16_t m_nCount;		//������������
		BlackListInfo m_astBlackList[MAX_BLACKLIST_PLAYER_COUNT];
		TReason m_szReasonMessage;
	};

//////////////////////////////////////////////////////////////////////////

	class CRequestAddBlackList : public CMessageBody
	{
	public:
		CRequestAddBlackList();
		~CRequestAddBlackList();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t  m_iMyUIN;
		TAccount m_szMyAccount;
		BlackListInfo m_stBlackList;
	};

	class CResponseAddBlackList : public CMessageBody
	{
	public:
		CResponseAddBlackList();
		~CResponseAddBlackList();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		BlackListInfo m_stBlackList;
	};

//////////////////////////////////////////////////////////////////////////

	typedef CRequestDeleteFriend CRequestDeleteBlackList;

	typedef CResponseDeleteFriend CNotifyDeleteBlackList;

//////////////////////////////////////////////////////////////////////////

	class CRequestAddNotVerifiedFriend : public CMessageBody
	{
	public:
		CRequestAddNotVerifiedFriend();
		~CRequestAddNotVerifiedFriend();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t  m_iMyUIN;
		TAccount m_szMyAccount;
		int32_t  m_iFriendUIN;
		TAccount m_szFriendAccount;
	};

//////////////////////////////////////////////////////////////////////////
	
	class CRequestCheckNotVerifiedFriend : public CMessageBody
	{
	public:
		CRequestCheckNotVerifiedFriend();
		~CRequestCheckNotVerifiedFriend();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t m_iMyUIN;
		int32_t m_iFriendUIN;
	};

	class CResponseCheckNotVerifiedFriend : public CMessageBody
	{
	public:
		CResponseCheckNotVerifiedFriend();
		~CResponseCheckNotVerifiedFriend();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;
		int32_t m_iMyUIN;
		int32_t m_iFriendUIN;
	};

//////////////////////////////////////////////////////////////////////////

	//ĳ��ʱ��������N����ͬ��������
	struct stChatInfo
	{
		int32_t  m_iUIN;
		TAccount m_szAccount;
		int32_t  m_iTime;

		stChatInfo& operator= (const stChatInfo& src);
		bool operator< (const stChatInfo& other);
	};

	int32_t encode_chat_info(char** pszOut, stChatInfo& info);
	int32_t decode_chat_info(char** pszIn, stChatInfo& info);

#define MAX_CHAT_WITH_OTHER_COUNT 100

	class CChatWithOtherInfo : public CMessageBody
	{
	public:
		CChatWithOtherInfo();
		~CChatWithOtherInfo();

		virtual int32_t encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t  m_iMyUIN;
		TAccount m_szMyAccount;
		int16_t  m_nCount;
		stChatInfo m_astInfo[MAX_CHAT_WITH_OTHER_COUNT];
	};

//////////////////////////////////////////////////////////////////////////


}}

#endif //_FRIEND_DB_SERVER_MESSAGE_BODY_H_

