#ifndef _GROUP_DB_SERVER_MESSAGE_BODY_H_
#define _GROUP_DB_SERVER_MESSAGE_BODY_H_

#include "public/server_message.h"

namespace KingNet { namespace Server {

	#define MAX_GROUP_RANK_COUNT		10     //群组排名显示最多10个
	#define MAX_GROUP_TOP_RANK_COUNT   100     //全服群组排名最多显示100个

	
	typedef struct stGroupRank
	{
		int32_t  m_iRank;							//排名
		char     m_szGroupID[max_qun_uid_length];   //群id
		int64_t  m_iRankValue;						//排名具体值
	}GroupRank, *PGroupRank;


	enum enmRankType
	{
		enm_51point_rank_type		= 0x01,
		enm_happybean_rank_type		= 0x02,
		enm_charming_rank_type		= 0x03,
		enm_achievement_rank_type	= 0x04,
		enm_game_result_change_type = 0x05,
	};

    class CGroupRankInfo
	{
	public:
		CGroupRankInfo();
		~CGroupRankInfo();

		CGroupRankInfo& operator= (const CGroupRankInfo& src);
		//清空数据
		void clear();

	public:
		char     m_szGroupID[max_qun_uid_length];      //标签ID，唯一标识一个群
		int64_t  m_i51Point;                           //51点
		int64_t  m_iHappyBean;                         //欢乐豆
		int64_t  m_iCharming;                          //魅力值
		int64_t  m_iAchievement;                       //成就值	
		int64_t  m_iSumPlayGameTime;				   //总在线时长
	};
    
	class C51PointRank_Pre
	{
	public:
		bool operator () (const CGroupRankInfo &dr1,  const CGroupRankInfo &dr2) 
		{
			if (dr1.m_i51Point != dr2.m_i51Point) 
			{
				return dr1.m_i51Point > dr2.m_i51Point;
			} else 
			{
				int32_t ret = strncmp(dr1.m_szGroupID, dr2.m_szGroupID, max_qun_uid_length);
				if(ret <= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	};

	class CHappyBeanRank_Pre
	{
	public:
		bool operator () (const CGroupRankInfo &dr1,  const CGroupRankInfo &dr2) 
		{
			if (dr1.m_iHappyBean != dr2.m_iHappyBean) 
			{
				return dr1.m_iHappyBean > dr2.m_iHappyBean;
			} else 
			{
				int32_t ret = strncmp(dr1.m_szGroupID, dr2.m_szGroupID, max_qun_uid_length);
				if(ret <= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	};

	class CCharmingRank_Pre
	{
	public:
		bool operator () (const CGroupRankInfo &dr1,  const CGroupRankInfo &dr2) 
		{
			if (dr1.m_iCharming != dr2.m_iCharming) 
			{
				return dr1.m_iCharming > dr2.m_iCharming;
			} else 
			{
				int32_t ret = strncmp(dr1.m_szGroupID, dr2.m_szGroupID, max_qun_uid_length);
				if(ret <= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	};

	class CAchievementRank_Pre
	{
	public:
		bool operator () (const CGroupRankInfo &dr1,  const CGroupRankInfo &dr2) 
		{
			if (dr1.m_iAchievement != dr2.m_iAchievement) 
			{
				return dr1.m_iAchievement > dr2.m_iAchievement;
			} else 
			{
				int32_t ret = strncmp(dr1.m_szGroupID, dr2.m_szGroupID, max_qun_uid_length);
				if(ret <= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	};

	class CSunPlayGameTimeRank_Pre
	{
	public:
		bool operator () (const CGroupRankInfo &dr1,  const CGroupRankInfo &dr2) 
		{
			if (dr1.m_iSumPlayGameTime != dr2.m_iSumPlayGameTime) 
			{
				return dr1.m_iSumPlayGameTime > dr2.m_iSumPlayGameTime;
			} else 
			{
				int32_t ret = strncmp(dr1.m_szGroupID, dr2.m_szGroupID, max_qun_uid_length);
				if(ret <= 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//请求群排名
	//SS_MSG_GET_GROUP_RANK
	int32_t encode_rankinfo(char** pszOut, stGroupRank& groupInfo);
	int32_t decode_rankinfo(char** pszIn, stGroupRank& groupInfo);

	class CRequestGetGroupRankInfo : public CMessageBody
	{
	public:
		CRequestGetGroupRankInfo();
		~CRequestGetGroupRankInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		char m_szGroupID[max_qun_uid_length];
		int8_t  m_byRankType;
		int32_t m_iBeforRankNum;
		int32_t m_iAfterRankNum;
		
	};

	//请求群排名应答
	class CResponseGetGroupRankInfo : public CMessageBody
	{
	public:
		CResponseGetGroupRankInfo();
		~CResponseGetGroupRankInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;           //返回值，success为成功，否则为失败 
		char		m_szGroupID[max_qun_uid_length];
		int8_t		m_byRankType;
		int16_t		m_shReturnCount;
		stGroupRank m_astRankInfo[MAX_GROUP_RANK_COUNT];  
		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//请求全服群排名
	//SS_MSG_GET_GROUP_TOP_RANK
	class CRequestGetGroupTopRankInfo : public CMessageBody
	{
	public:
		CRequestGetGroupTopRankInfo();
		~CRequestGetGroupTopRankInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int8_t  m_byRankType;
		int32_t m_iRankNum;
	};

	//请求全服群排名应答
	class CResponseGetGroupTopRankInfo : public CMessageBody
	{
	public:
		CResponseGetGroupTopRankInfo();
		~CResponseGetGroupTopRankInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;           //返回值，success为成功，否则为失败 
		
		int8_t		m_byRankType;
		int16_t		m_shReturnCount;
		stGroupRank m_astRankInfo[MAX_GROUP_TOP_RANK_COUNT];  

		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

#endif //_FRIEND_DB_SERVER_MESSAGE_BODY_H_

