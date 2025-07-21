/**
@file   server_catalog_message_body.h
@brief  �����˺�Logic server, state server client ͨѶ�İ�ͷ�Ͱ���ṹ

@author sjij@mail.51.com
@date Begin 2008.5
@date Dnd 2008.5
*/

#ifndef _SERVER_MESSAGE_H_
#define _SERVER_MESSAGE_H_

#include <stdarg.h>
#include "Common/common_base.h"  // for (fail/sucess)�ȶ���
#include "server_message.h"
#include "gameplatform_message.h"

namespace KingNet { namespace Server { 

//////////////////////////////////////////////////////////////////////////
//Client VS Catalog Server
//////////////////////////////////////////////////////////////////////////

const int32_t MAX_ROOM_COUNT_PER_SERVER = 100;
const int32_t HASH_CODE_LEN = 16;

//���ģ��汾��
const int32_t MAX_CS_MODULE_VERSION_COUNT = 128;
//cs�������ļ�
const int32_t MAX_CS_CONFIG_FILE_COUNT = 64;
//�������
const int32_t MAX_CS_BLOCK_COUNT = 256;
//���ͳ������
const int32_t MAX_CS_STATISTIC_COUNT = 8;
//�ڵ���
const int32_t MAX_NODE_POPULATION_COUNT = 1024;
const int32_t MAX_CONFIG_DATA_SIZE = 0x8000; //32K

const int32_t BLOCK_ITEM_COUNT = 256;				 //256����
const int32_t REQUEST_BLOCK_ITEM_COUNT = 64;
const int32_t BLOCK_CONTENT_MAX_LENGTH = 64*1024;    //�����С 64K
const int32_t ROOM_ITEM_COUNT = 2*10240;             //2����Ϸ����
const int32_t ONE_ROOM_BLOCK_COUNT = 16;             //ÿ��������ֵ�������
const int32_t ONE_BLOCK_NODE_COUNT = 1024;

const int32_t REASON_LEN = 256;


enum
{
	max_server_config_data_size = 0x2800,
	max_server_room_data_size = 0xB800,			//modified by aprilliu, 2009-04-13
	max_dirty_words_size = 0x4000,
	max_state_server_data_size = 0x8000,
	max_cs_file_content_length = 0xffff
};


	struct stCSMoudleVersions
	{
		int16_t shModuleID;			//ModuleID�б� 0������ 1�������� 2�����๫����
		int32_t shLocalVersion;
	};

	struct stCSConfigHash
	{
		int16_t shConfigID;         //�����ļ�ID 0��Dynamic.cfg	1��Catalog.svr2��Logic.svr
		//3��Download.svr 4��Status.svr 5��Personal.svr
		char shLocalHash[HASH_CODE_LEN];
	};

	struct stCSBlockHash
	{
		int16_t shBlockID;          //����Ŀ¼������ block id
		char  BlockHash[HASH_CODE_LEN];//hash code
	};

	struct stCSStatistic
	{
		int32_t nFileLen;           //ͳ������
		int32_t nActualFileLen;
		char  FileContent[max_cs_file_content_length];
	};


	/**	
	@brief  ��¼��ȡ��ʼ���ݿ���Ϣ��
	*/
	class  CCSRequestGetInitInfo
	{
	public:
		CCSRequestGetInitInfo();
		~CCSRequestGetInitInfo();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:

		struct stCSRequestGetInitInfo
		{
			int16_t shMoudleVersionsCount;
			stCSMoudleVersions pMoudleVersions[MAX_CS_MODULE_VERSION_COUNT];

			int16_t shConfigHashCount;
			stCSConfigHash pConfigHash[MAX_CS_CONFIG_FILE_COUNT];

			int16_t shBlockHashCount;
			stCSBlockHash pBlockHash[MAX_CS_BLOCK_COUNT];

			int16_t shStatisticCount;
			stCSStatistic pStatistic[MAX_CS_STATISTIC_COUNT];
		};

		stCSRequestGetInitInfo m_stBody;
	private:
	};

	struct stCSVersionControl
	{
		//��Ҫ���°汾��Ϣ
		int16_t shModuleID;			//ModuleID�б� 0������ 1�������� 2�����๫����
		int16_t shServerVersion;	
		int16_t shUpdateType;       //1����Ҫ���� 2���������	3����������
	};

	struct stCSConfigData
	{
		//��Ҫ���°汾��Ϣ
		int16_t shConfigID;			
		int16_t shConfigContentLen;	
		int32_t shActualConfigContentLen;
		char ConfigContent[max_cs_file_content_length];
	};

	struct stNodeOnline
	{
		int32_t nNode;
		int32_t nMMOnlinecount;
		int32_t nOtherOnlinecount;
	};

	/**	
	@brief  ��¼��ȡ��ʼ���ݿ���Ϣ�� ��Ӧ
	*/
	class  CCSResponseFailedInfo
	{
	public:
		CCSResponseFailedInfo();
		~CCSResponseFailedInfo();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:
		int16_t shResult;
		int16_t shReasonLen;
		char szReason[REASON_LEN];

	};

	/**	
	@brief  ��¼��ȡ��ʼ���ݿ���Ϣ�� ��Ӧ
	*/
	class  CCSResponseGetInitInfo
	{
	public:
		CCSResponseGetInitInfo();
		~CCSResponseGetInitInfo();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:
		struct stCSResponseGetInitInfo
		{
			int16_t shResult;
			int32_t ntimeStamp; 

			int16_t shVersionControlCount;
			int8_t  byVerCLen;            //Ϊ���ݰ汾ʹ�õĳ���
			stCSVersionControl pVersionControl[MAX_CS_MODULE_VERSION_COUNT];

			//������Ϣ
			int16_t  shConfigIDCount;
			stCSConfigData pConfigData[MAX_CS_CONFIG_FILE_COUNT];

			//�ڵ���Ϣ
			int16_t  shValidBlockCount;
			int16_t  pValidBlock[MAX_CS_BLOCK_COUNT];        //�ڵ���Ϣ


			int32_t     nHallMMOnlineCount;
			int32_t     nHallOtherOnlineCount;
			int16_t     shPopulationBlockCount;
			stNodeOnline  pPopulationBlockData[MAX_NODE_POPULATION_COUNT];


			//�Ƿ��и��ڵ�
			int8_t    byHaveRoot;
			//���ڵ�
			int16_t   shBlockID;
			int16_t   shNodeBlockLen;
			int32_t   shDecompressedLen;
			char      NodeBlockData[max_cs_file_content_length];			
		};

		stCSResponseGetInitInfo m_stBody;

	private:
	};

	/**	
	@brief  �õ�Ŀ¼�����ݰ���
	*/
	class  CCSRequestGetBlockInfo
	{
	public:
		CCSRequestGetBlockInfo();
		~CCSRequestGetBlockInfo();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:
		struct stBlock
		{
			int16_t shBlockID;		
			int8_t  byType;      //0��NodeBlock	1��PopulationBlock
		};

		struct stCSRequestBlockInfo
		{
			int16_t shBlockCount;
			stBlock pBlock[BLOCK_ITEM_COUNT];
		};

		stCSRequestBlockInfo m_stBody;
	private:
	};

	/**	
	@brief  �õ�Ŀ¼������������ ��Ӧ
	*/
	struct stBlockFileContent
	{
		int16_t shBlockLen;
		int32_t shDecompressedLen;
		char content[max_cs_file_content_length];
	};

	struct stNodeOnlineCount
	{
		int16_t shNodeCount;
		stNodeOnline pArrNodeOnline[ONE_BLOCK_NODE_COUNT];
	};

	struct stBlockContent
	{
		int8_t  byType;      //0��NodeBlock	1��PopulationBlock
		int16_t shBlockID;

		union unContent 
		{
			stBlockFileContent BlockContent;
			stNodeOnlineCount NodeOnlineCount;
		}unContent;
	};


	class  CCSResponseGetBlockInfo
	{
	public:
		CCSResponseGetBlockInfo();
		~CCSResponseGetBlockInfo();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:

		struct stCSResponseGetBlockInfo
		{
			int32_t nHallMMOnlineCount;
			int32_t nHallOtherOnlineCount;

			int16_t shBlockCount;
			stBlockContent pBlockContent[REQUEST_BLOCK_ITEM_COUNT];			
		};

		stCSResponseGetBlockInfo m_stBody;
	private:
	};

	/**	
	@brief  ��ѯ������ڿ�
	*/
	class  CCSRequestSearchBlock
	{
	public:
		CCSRequestSearchBlock();
		~CCSRequestSearchBlock();

		int32_t encode(char *pszOut, int32_t& iOutLength);
		int32_t decode(const char *pszIn, const int32_t iInLength);
		int32_t dump();

	public:
		struct stCSRequestSearchBlock
		{
			int16_t shIDCount;
			int32_t pNodeIDs[MAX_CS_BLOCK_COUNT];
		};

		stCSRequestSearchBlock m_stBody;
	private:
	};

	//////////////////////////////////////////////////////////////////////////
	//Logic Server VS Catalog Server
	//////////////////////////////////////////////////////////////////////////
	/**	
	@brief  SS_MSG_REGISTER request
	*/
	class CRequestRegisterServer : public CMessageBody
	{
	public:
		CRequestRegisterServer();
		~CRequestRegisterServer();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iServerID;
		int8_t m_cServerType;

		int32_t m_iLocalIP;
		int16_t m_nLocalPort;

	protected:

	private:
	};

	//response
	class CResponseRegisterServer : public CMessageBody
	{
	public:
		CResponseRegisterServer();
		~CResponseRegisterServer();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

		int16_t m_nResultID;

		int16_t m_nFlag;				//һЩ��ʶ��Ŀǰʹ�ã���0��1 bit��ʾѹ���㷨����Ϊ�գ����ʾ��ѹ����1����ʾ7Zipѹ��

	//	int16_t m_nServerConfigDataSize;
		int32_t m_iServerConfigDataSize; //�޸�Ϊ32λ 20090622 by ankye	
		char m_szServerConfigData[max_server_config_data_size];		//������ͨ������

	//	int16_t m_nServerRoomDataSize;
		int32_t m_iServerRoomDataSize;  //�޸�Ϊ32λ 20090622 by ankye	
		char m_szServerRoomData[max_server_room_data_size];			//��������������

		//int16_t m_nDirtyWordsSize;
		//char m_szDirtyWords[max_dirty_words_size];					//���дʿ�

	protected:
	private:
	};

	/**	
	@brief  SS_MSG_REPORTSTATICS �ϱ�������Ϣ
	*/
	struct stRoomStatics
	{
		int32_t m_iRoomID;
		int16_t m_nMMCount;		//������MM����Ŀ
		int16_t m_nOtherCount;	//��MM�⣬�����˸���
	};

	int32_t encode_room_statics(char** pOut, stRoomStatics& statics);
	int32_t decode_room_statics(char** pOut, stRoomStatics& statics);
	//��*pOut����iByteCount���ֽ�
	//void skip_byte_of(char** pOut, int32_t iByteCount);

	class CSSNotifyReportStatics : public CMessageBody
	{
	public:
		CSSNotifyReportStatics();
		~CSSNotifyReportStatics();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iServerID;
		int8_t m_cServerType;
		int16_t m_nRoomCount;
		stRoomStatics m_stRoomStatics[MAX_ROOM_COUNT_PER_SERVER];

	protected:
	private:
	};


	//////////////////////////////////////////////////////////////////////////
	//State Server VS Catalog Server
	//////////////////////////////////////////////////////////////////////////

	class CResponseRegisterStateServer : public CMessageBody
	{
	public:
		CResponseRegisterStateServer();
		~CResponseRegisterStateServer();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

		int16_t m_nResultID;

		int16_t m_nFlag;				//һЩ��ʶ��Ŀǰʹ�ã���0��1 bit��ʾѹ���㷨����Ϊ�գ����ʾ��ѹ����1����ʾ7Zipѹ��

		int16_t m_nServerConfigDataSize;
		char m_szServerConfigData[max_state_server_data_size];		//state server������ͨ������

	protected:
	private:
	};

	/**	
	@brief  state server�ϱ�������Ϣ
	*/
	class CSSNotifyReportStateStatics : public CMessageBody
	{
	public:
		CSSNotifyReportStateStatics();
		~CSSNotifyReportStateStatics();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iServerID;
		int8_t m_cServerType;
		int32_t m_nMMCount;		//������MM����Ŀ
		int32_t m_nOtherCount;	//��MM�⣬�����˸���

	protected:
	private:
	};

	/**	
	@brief  WS_MSG_REPORTSTATICS web�ڵ��ϱ�����Э��
	*/
	struct stWebRoomStatics
	{
		int32_t m_iRoomID;
		int32_t m_nMMCount;		//������MM����Ŀ
		int32_t m_nOtherCount;	//��MM�⣬�����˸���
	};

	int32_t encode_web_room_statics(char** pOut, stWebRoomStatics& statics);
	int32_t decode_web_room_statics(char** pOut, stWebRoomStatics& statics);
	//��*pOut����iByteCount���ֽ�
	//void skip_byte_of(char** pOut, int32_t iByteCount);

	class CWSNotifyReportStatics : public CMessageBody
	{
	public:
		CWSNotifyReportStatics();
		~CWSNotifyReportStatics();

		virtual int32_t  encode(char *pszOut, int32_t& iOutLength);
		virtual int32_t  decode(const char *pszIn, const int32_t iInLength);
		virtual void dump();

		int32_t m_iServerID;
		int8_t m_cServerType;
		int16_t m_nRoomCount;
		stWebRoomStatics m_stRoomStatics[MAX_ROOM_COUNT_PER_SERVER];

	protected:
	private:
	};

}}//namespace KingNet { namespace Server {
#endif

