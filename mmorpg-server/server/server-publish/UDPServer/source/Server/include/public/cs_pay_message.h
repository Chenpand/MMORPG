/**
@file   cs_pay_message.h
@brief  ������pay.51.com php ��market serverͨѶ�İ�ͷ�Ͱ���ṹ

@author sjij@mail.51.com
@date Begin 2008.9
@date Dnd 2008.9
*/

#ifndef _CS_PAY_MESSAGE_H_
#define _CS_PAY_MESSAGE_H_

#include "Common/common_base.h"  // for (fail/sucess)�ȶ���
#include "public/server_message.h"

using namespace KingNet::Server;

namespace KingNet { namespace Server {

const int32_t MAX_ORDER_ID_LEN = 64;
const int32_t MAX_TOKEN_LEN = 128;
const int32_t MAX_VIP_TYPE_LEN = 10;
const int32_t MAX_VIP_ADDITIONAL_LEN = 1024;

	/**	
	@brief  pay.51.com���͵ĳ�ֵ����Э��
	*/
	class  CCSRequestPayToGame:public CMessageBody
	{
	public:
		CCSRequestPayToGame();
		~CCSRequestPayToGame();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestPayToGame& operator = (const CCSRequestPayToGame& original);

	public:
		
		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iChannel;
		int32_t m_iAmount;
		int32_t m_iPayamount;

		int32_t m_iRequestTimestamp;
		
	private:
	};

	
	/**	
	@brief  pay.51.com���͵ĳ�ֵ��ӦЭ��
	*/
	class  CCSResponsePayToGame:public CMessageBody
	{
	public:
		CCSResponsePayToGame();
		~CCSResponsePayToGame();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iChannel;
		int32_t m_iAmount;
		int32_t m_iPayamount;
		int32_t m_iMoney;   //���º�ĵ�ǰ��Ϸ�ҵ�����

		int32_t m_iRequestTimestamp;
		
		char m_szGameSequenceID[MAX_ORDER_ID_LEN];
		int32_t m_iResponseTimestamp;

	};	

	/**	
	@brief  pay.51.com���͵Ķһ�����Э��
	*/
	class  CCSRequestPayExchange:public CMessageBody
	{
	public:
		CCSRequestPayExchange();
		~CCSRequestPayExchange();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestPayExchange& operator = (const CCSRequestPayExchange& original);

	public:

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iExcid;  //�һ��������ݼ�¼��id
		int32_t m_iPayamount;

		int32_t m_iRequestTimestamp;

	private:
	};


	/**	
	@brief  pay.51.com���͵Ķһ���ӦЭ��
	*/
	class  CCSResponsePayExchange:public CMessageBody
	{
	public:
		CCSResponsePayExchange();
		~CCSResponsePayExchange();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;

		int32_t  m_iSrcUin;
		TAccount m_szSrcAccount;

		int32_t  m_iDstUin;
		TAccount m_szDstAccount;

		char m_szOrderID[MAX_ORDER_ID_LEN];

		int32_t m_iExcid;     //�һ��������ݼ�¼��id
		int32_t m_iPayamount;

		int32_t m_iRequestTimestamp;

		char m_szGameSequenceID[MAX_ORDER_ID_LEN];
		int32_t m_iResponseTimestamp;
	};


	/**	
	@brief  ��ѯ��Ϸ������Э��
	*/
	class  CCSRequestSearchGameMoney:public CMessageBody
	{
	public:
		CCSRequestSearchGameMoney();
		~CCSRequestSearchGameMoney();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

		CCSRequestSearchGameMoney& operator = (const CCSRequestSearchGameMoney& original);

	public:

		int32_t  m_iSrcUin;
	
	private:
	};


	/**	
	@brief  pay.51.com���͵Ĳ�ѯ��Ϸ����ӦЭ��
	*/
	class  CCSResponseSearchGameMoney:public CMessageBody
	{
	public:
		CCSResponseSearchGameMoney();
		~CCSResponseSearchGameMoney();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		void dump();

	public:

		int16_t  m_shResultID;
		int32_t  m_iSrcUin;		
		int32_t  m_iCoin;
	};

	/**
	@brief  pay.51.com���͵Ĳ�ѯ�û��Ƿ�ΪVIP����Э��
	*/
	class CCSRequestSearchVipInfo:public CMessageBody
	{
	public:
		CCSRequestSearchVipInfo();
		~CCSRequestSearchVipInfo();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:

		int32_t m_iSrcUin;
		//TAccount m_szSrcAccount;
	};

	/**
	 @brief ��ѯ�û��Ƿ�ΪVIP�ظ�
	 */
	class CCSResponseSearchVipInfo:public CMessageBody
	{
	public:
		CCSResponseSearchVipInfo();
		~CCSResponseSearchVipInfo();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:

		int32_t m_iSrcUin;
		//TAccount m_szSrcAccount;
		int16_t m_shResultID;
		char m_szVipType[MAX_VIP_TYPE_LEN];  //vip����
		int32_t m_iEndTime;                  //vip����ʱ��

	};

	/**
	 @brief pay.51.com���û�����ͨ��������
	 */
	class CCSRequestPayToVip:public CMessageBody
	{
	public:
		CCSRequestPayToVip();
		~CCSRequestPayToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];   //��ˮ��
		int32_t m_iSrcUin;                    //����UIN
		TAccount m_szSrcAccount;              //����account
		int32_t m_iRequestTime;               //��������ʱ��ʱ���
		int32_t m_iOperUin;                   //����UIN
		TAccount m_szOperAccount;             //����account
		int32_t m_iAmount;                    //��ֵ�۷ѽ��
		int32_t m_iChannelID;                 //����ID
		int32_t m_iComboID;                   //�ײ�ID����Ҫ�ǳ�ֵ���ĵ�
		int32_t m_iGoodsID;                   //�ID�������Ƿ����͸��û������Ʒ�����
		int32_t m_iAddTime;                   //���û���VIP��ʱ��������ֵ
		int32_t m_iVipType;                   //�û���vip����
		int32_t m_iAddVipInt;                 //�����ֶΣ�����չ��;
		char m_szAddVipStr[MAX_VIP_ADDITIONAL_LEN];         //�����ֶΣ�����չ��;
	};
	
	/**
	 @brief ���û�������ظ�
	 */
	class CCSResponsePayToVip:public CMessageBody
	{
	public:
		CCSResponsePayToVip();
		~CCSResponsePayToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];     //php���͹�������ˮ��
		int32_t m_iSrcUin;                      //������uin
		TAccount m_szSrcAccount;                //������account
		int32_t m_iOperUin;						//�����uin
		TAccount m_szOperAccount;				//�����account
		int16_t m_shResultID;					//���ؽ��
		char m_szTransTag[MAX_ORDER_ID_LEN];	//server���ɵ�һ����ˮ��
		int16_t m_shActResultID;				//������Ʒ���
		int32_t m_iVipEndTime;					//VIPʧЧʱ��

	};

	/**
	 @brief  �ֻ���������
	 */
	class CCSRequestPayMobileToVip:public CMessageBody
	{
	public:
		CCSRequestPayMobileToVip();
		~CCSRequestPayMobileToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];   //��ˮ��
		int32_t m_iSrcUin;                    //����UIN
		TAccount m_szSrcAccount;              //����account
		int32_t m_iRequestTime;               //��������ʱ��ʱ���
		int32_t m_iChannelID;                 //����ID
		int32_t m_iComboID;                   //�ײ�ID����Ҫ�ǳ�ֵ���ĵ�
		int32_t m_iGoodsID;                   //�ID�������Ƿ����͸��û������Ʒ�����
		char m_szMobile[MAX_ORDER_ID_LEN];    //���ֻ�����
		int32_t m_iAddVipInt;                 //�����ֶΣ�����չ��;
		char m_szAddVipStr[MAX_VIP_ADDITIONAL_LEN];         //�����ֶΣ�����չ��;

	};

	/************************************************************************/
	/* @brief  �ֻ���������ظ�                                             */
	/************************************************************************/
	class CCSResponsePayMobileToVip:public CMessageBody
	{
	public:
		CCSResponsePayMobileToVip();
		~CCSResponsePayMobileToVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];             //php���ɵ���ˮ��
		int32_t m_iSrcUin;								//����UIN
		TAccount m_szSrcAccount;						//����account
		int16_t m_shResultID;							//���ؽ��
		char m_szTransTag[MAX_ORDER_ID_LEN];			//server���ɵ�ȫ��Ψһ��ˮ��
		int16_t m_shActResultID;					    //����ص�ID
		//int32_t m_iGoodsID;							//�ID
	};

	/************************************************************************/
	/* @brief �ֻ������˶�VIP����                                           */
	/************************************************************************/
	class CCSRequestPayUnsubscriptVip:public CMessageBody
	{
	public:
		CCSRequestPayUnsubscriptVip();
		~CCSRequestPayUnsubscriptVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];		//phpǰ�����ɵ���ˮ��
		int32_t m_iSrcUin;						//�����û�UIN
		TAccount m_szSrcAccount;				//�����û�Account
		int32_t m_iRequestTime;					//��������ʱ��
		char m_szMobile[MAX_ORDER_ID_LEN];		//�ֻ���

	};

	/************************************************************************/
	/* @brief �ֻ������˶�VIP�ظ�                                           */
	/************************************************************************/
	class CCSResponsePayUnsubscriptVip:public CMessageBody
	{
	public:
		CCSResponsePayUnsubscriptVip();
		~CCSResponsePayUnsubscriptVip();

		int32_t encode(char* pszOut, int32_t& iOutLength);
		int32_t decode(const char* pszIn, const int32_t iInLength);
		void dump();

	public:
		char m_szOrderID[MAX_ORDER_ID_LEN];		//��ˮ��
		int32_t m_iSrcUin;						//�����û�UIN
		TAccount m_szSrcAccount;				//�����û�Account
		int16_t m_shResultID;					//���ؽ��
		char m_szTransTag[MAX_ORDER_ID_LEN];	//server������ˮ��
	};


}}//namespace KingNet { namespace Server

#endif

