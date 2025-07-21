#ifndef _GROUP_DB_SERVER_MESSAGE_BODY_H_
#define _GROUP_DB_SERVER_MESSAGE_BODY_H_

#include "public/server_message.h"

namespace KingNet { namespace Server {

	#define MAX_GROUP_RANK_COUNT		10     //Ⱥ��������ʾ���10��
	#define MAX_GROUP_TOP_RANK_COUNT   100     //ȫ��Ⱥ�����������ʾ100��

	
	typedef struct stGroupRank
	{
		int32_t  m_iRank;							//����
		char     m_szGroupID[max_qun_uid_length];   //Ⱥid
		int64_t  m_iRankValue;						//��������ֵ
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
		//�������
		void clear();

	public:
		char     m_szGroupID[max_qun_uid_length];      //��ǩID��Ψһ��ʶһ��Ⱥ
		int64_t  m_i51Point;                           //51��
		int64_t  m_iHappyBean;                         //���ֶ�
		int64_t  m_iCharming;                          //����ֵ
		int64_t  m_iAchievement;                       //�ɾ�ֵ	
		int64_t  m_iSumPlayGameTime;				   //������ʱ��
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

	//����Ⱥ����
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

	//����Ⱥ����Ӧ��
	class CResponseGetGroupRankInfo : public CMessageBody
	{
	public:
		CResponseGetGroupRankInfo();
		~CResponseGetGroupRankInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;           //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		char		m_szGroupID[max_qun_uid_length];
		int8_t		m_byRankType;
		int16_t		m_shReturnCount;
		stGroupRank m_astRankInfo[MAX_GROUP_RANK_COUNT];  
		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//����ȫ��Ⱥ����
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

	//����ȫ��Ⱥ����Ӧ��
	class CResponseGetGroupTopRankInfo : public CMessageBody
	{
	public:
		CResponseGetGroupTopRankInfo();
		~CResponseGetGroupTopRankInfo();

		virtual int32_t  encode(char* pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char* pszIn, const int32_t iInLength);
		virtual void dump();

	public:
		int16_t     m_nResultID;           //����ֵ��successΪ�ɹ�������Ϊʧ�� 
		
		int8_t		m_byRankType;
		int16_t		m_shReturnCount;
		stGroupRank m_astRankInfo[MAX_GROUP_TOP_RANK_COUNT];  

		TReason		m_szReasonMessage;
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

#endif //_FRIEND_DB_SERVER_MESSAGE_BODY_H_

