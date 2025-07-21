#ifndef _FRIEND_DB_SERVER_MESSAGE_BODY_H_
#define _FRIEND_DB_SERVER_MESSAGE_BODY_H_

#include "public/server_message.h"

namespace KingNet { namespace Server {

        
#define MAX_TAG_NAME_LENGTH         16      //标签的名称最大长度
#define MAX_POSITIVE_FRIEND_COUNT   100     //一个用户最多的正向玩伴数量
#define MAX_REVERSE_FRIEND_COUNT    1000    //一个用户最多的逆向玩伴数量
#define MAX_TAG_COUNT_PER_FRIEND    1       //给一个玩伴打标签的最大数量
#define MAX_RESPONSE_FRIEND_COUNT	100		//一次返回应答时候的玩伴个数，受通信包64K的限制
#define INVALID_TAG_ID				0		//无效的标签ID
#define MAX_FRIEND_REMARKS_LENGTH	16		//玩伴备注名的最大长度（15+1）
#define MAX_GROUP_NAME_LENGTH		19		//玩伴分组名的最大长度（18+1）
#define MAX_FRIEND_GROUP_COUNT		32		//一个玩家最多32个玩伴分组
#define MAX_PLAY_WITH_COUNT			20		//和谁一起玩过的人总数不大于20
#define MAX_STAT_TAG_COUNT			30		//统计标签最多30个
#define MAX_BLACKLIST_PLAYER_COUNT	100		//黑名单中的人最多100个


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
	      

	//贴在玩伴身上的标签信息
	class CTagInfo
	{
	public:
		CTagInfo();
		~CTagInfo();

		CTagInfo& operator= (const CTagInfo& src);
		bool valid_tag_id();
		//清空数据
		void clear();

	public:
		int32_t     m_iTagID;      //标签ID，唯一标识一个标签
		char        m_szTagName[MAX_TAG_NAME_LENGTH];        //标签名称
		int32_t     m_iTimestamp;   //贴该标签的时间戳，该字段不参与编解码
	};

	//玩伴信息
	class CFriendInfo
	{
	public:
		CFriendInfo();
		~CFriendInfo();

		CFriendInfo& operator= (const CFriendInfo& src);
		void clear();

	public:
		int32_t  m_iFriendUIN;			//玩伴的UIN
		TAccount m_szFriendAccount;		//玩伴的accout
		int32_t  m_iTimestamp;			//该字段不进行编码
		int16_t  m_nTagCount;			//标签个数
		CTagInfo m_astTag[MAX_TAG_COUNT_PER_FRIEND];
		char     m_szRemarks[MAX_FRIEND_REMARKS_LENGTH];
		int32_t  m_iGroupFlag;
	};

	//用户被贴标签的统计信息
	class CStatTagInfo
	{
	public:
		CStatTagInfo();
		~CStatTagInfo();

		CStatTagInfo& operator= (const CStatTagInfo& src);
		bool operator< (const CStatTagInfo& tag2);
		void clear();

	public:
		int32_t	m_iTagID;       //标签ID，唯一标识一个标签
		char    m_szTagName[MAX_TAG_NAME_LENGTH];        //标签名称
		int32_t m_iCount;       //被贴该标签的总数量
		int32_t m_iTimestamp;   //最近贴该标签的时间戳
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
		TAccount m_szAccount;	//账号
		int32_t  m_iGroupFlag;	//组信息
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//请求正向玩伴列表
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
		int32_t m_iDestUIN;		//目标玩家的Uin
	};

	//正向玩伴列表应答
	class CResponsePositiveFriendList : public CMessageBody
	{
	public:
		CResponsePositiveFriendList();
		~CResponsePositiveFriendList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;           //返回值，success为成功，否则为失败 
		int32_t     m_iMyUIN;              //玩伴列表所有者的UIN
		int16_t     m_nFriendCount;        //玩伴的数量
		CFriendInfo m_astFriendInfo[MAX_RESPONSE_FRIEND_COUNT];  //玩伴的详细信息

		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//state server请求逆向玩伴列表
	typedef CRequestPositiveFriendList CRequestReverseList;

	//逆向玩伴列表应答
	class CResponseReverseList : public CMessageBody
	{
	public:
		CResponseReverseList();
		~CResponseReverseList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //返回值，success为成功，否则为失败 
		int32_t m_iMyUIN;              //自己的UIN
		int16_t m_nFriendCount;        //玩伴的数量
		int32_t m_aiFriendUIN[MAX_REVERSE_FRIEND_COUNT];  //玩伴的UIN
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//可以请求他人的被贴标签统计信息
	//SS_MSG_GET_SOMEONE_TAG_COUNT
	typedef CRequestPositiveFriendList CRequestTagStatisticalInfo;

	//请求被贴标签统计信息的应答
	class CResponseTagStatisticalInfo : public CMessageBody
	{
	public:
		CResponseTagStatisticalInfo();
		~CResponseTagStatisticalInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;	//返回值，success为成功，否则为失败 
		int32_t     m_iMyUIN;       //请求者自己的UIN	
		int32_t     m_iDestUIN;		//目标玩家的UIN
		int16_t     m_nTagCount;	//标签数量
		CStatTagInfo m_astTag[MAX_STAT_TAG_COUNT];

		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//请求给玩伴贴标签
	class CRequestAddTagToFriend : public CMessageBody
	{
	public:
		CRequestAddTagToFriend();
		~CRequestAddTagToFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t		m_iMyUIN;			//自己的UIN
		TAccount	m_szMyAccount;		//自己的accout
		int32_t		m_iFriendUIN;		//玩伴的UIN
		TAccount	m_szFriendAccount;	//玩伴的account
		int32_t		m_iTagID;			//标签ID，唯一标识一个标签
		char		m_szTagName[MAX_TAG_NAME_LENGTH];	//标签名称
	};

	//给玩伴贴标签的应答
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

	//添加逆向玩伴 SS_MSG_ADD_REVERSE_TAG
	typedef CRequestAddTagToFriend CRequestAddReverseTag;
	typedef CResponseAddTagToFriend CResponseAddReverseTag;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//请求删除玩伴
	class CRequestDeleteFriend : public CMessageBody
	{
	public:
		CRequestDeleteFriend();
		~CRequestDeleteFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int32_t	m_iMyUIN;     //自己的UIN
		int32_t	m_iFriendUIN; //玩伴的UIN
	};

	//删除玩伴的应答
	class CResponseDeleteFriend : public CMessageBody
	{
	public:
		CResponseDeleteFriend();
		~CResponseDeleteFriend();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;             //返回值，success为成功，否则为失败 
		int32_t m_iMyUIN;              //请求者自己的UIN	
		int32_t m_iFriendUIN;          //玩伴的UIN
		TReason m_szReasonMessage;
	};

	enum enmNotifyFriendInfoControlCMD
	{
		update_friend_info = 1,
		delete_friend	   = 2,
		add_friend		   = 3,
	};

	//SS_MSG_NOTIFY_FRIEND_INFO  全量通知玩伴的所有tag信息 notify
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

	//获取简单的玩伴列表，只有好友的UIN
	typedef CRequestPositiveFriendList CRequestSimpleFriendList;

	//获取简单的玩伴列表应答
	class CResponseSimpleFriendList : public CMessageBody
	{
	public:
		CResponseSimpleFriendList();
		~CResponseSimpleFriendList();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //返回值，success为成功，否则为失败 
		int32_t m_iMyUIN;              //玩伴所属的UIN
		int16_t m_nFriendCount;        //玩伴的数量
		int32_t m_aiFriendUIN[MAX_POSITIVE_FRIEND_COUNT];  //玩伴的UIN
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//添加玩伴分组的请求
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
		char     m_szGroupName[MAX_GROUP_NAME_LENGTH];	//玩伴分组名
	};

	//添加玩伴分组的应答
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

	//删除玩伴分组的请求
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

	//删除玩伴分组的应答
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

	//把玩伴移到某个分组的请求
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

	//把玩伴移到某个分组的应答
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

	//重新命名某个分组的请求
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

	//重新命名某个分组的应答
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

	//获取玩家玩伴分组信息的请求
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
		bool    m_bUsed;		//该字段不编解码
	};

	int32_t encode_group_info(char** pszOut, CGroupInfo& group_info);
	int32_t decode_group_info(char** pszIn, CGroupInfo& group_info);

	//获取玩家玩伴分组信息的应答
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

	//更新玩伴备注的请求
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

	//更新玩伴备注的应答
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

	//获取玩家和谁玩过信息的请求
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

	//获取玩家和谁玩过信息的应答
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

	//添加和谁玩过信息的请求
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

	//添加和谁玩过信息的应答
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

	//通知和谁玩过信息的请求
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

	//获取简单的玩伴列表，只有好友的UIN
	typedef CRequestPositiveFriendList CRequestFriendBaseInfo;

	//获取简单的玩伴列表应答
	class CResponseFriendBaseInfo : public CMessageBody
	{
	public:
		CResponseFriendBaseInfo();
		~CResponseFriendBaseInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t m_nResultID;           //返回值，success为成功，否则为失败 
		int32_t m_iMyUIN;              //玩伴所属的UIN
		int16_t m_nFriendCount;        //玩伴的数量
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
		int16_t m_nResultID;	//返回值，success为成功，否则为失败 
		int32_t m_iMyUIN;		//黑名单所属的UIN
		int16_t m_nCount;		//黑名单的数量
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

	//某段时间内最多跟N个不同的人聊天
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

