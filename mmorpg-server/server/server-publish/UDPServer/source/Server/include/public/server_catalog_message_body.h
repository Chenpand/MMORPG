/**
@file   server_catalog_message_body.h
@brief  定义了和Logic server, state server client 通讯的包头和包体结构

@author sjij@mail.51.com
@date Begin 2008.5
@date Dnd 2008.5
*/

#ifndef _SERVER_MESSAGE_H_
#define _SERVER_MESSAGE_H_

#include <stdarg.h>
#include "Common/common_base.h"  // for (fail/sucess)等定义
#include "server_message.h"
#include "gameplatform_message.h"

namespace KingNet { namespace Server { 

//////////////////////////////////////////////////////////////////////////
//Client VS Catalog Server
//////////////////////////////////////////////////////////////////////////

const int32_t MAX_ROOM_COUNT_PER_SERVER = 100;
const int32_t HASH_CODE_LEN = 16;

//最大模块版本数
const int32_t MAX_CS_MODULE_VERSION_COUNT = 128;
//cs端配置文件
const int32_t MAX_CS_CONFIG_FILE_COUNT = 64;
//最大块个数
const int32_t MAX_CS_BLOCK_COUNT = 256;
//最大统计数据
const int32_t MAX_CS_STATISTIC_COUNT = 8;
//节点数
const int32_t MAX_NODE_POPULATION_COUNT = 1024;
const int32_t MAX_CONFIG_DATA_SIZE = 0x8000; //32K

const int32_t BLOCK_ITEM_COUNT = 256;				 //256个块
const int32_t REQUEST_BLOCK_ITEM_COUNT = 64;
const int32_t BLOCK_CONTENT_MAX_LENGTH = 64*1024;    //最大块大小 64K
const int32_t ROOM_ITEM_COUNT = 2*10240;             //2万游戏房间
const int32_t ONE_ROOM_BLOCK_COUNT = 16;             //每个房间出现的最大块数
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
		int16_t shModuleID;			//ModuleID列表 0：大厅 1：斗地主 2：牌类公共包
		int32_t shLocalVersion;
	};

	struct stCSConfigHash
	{
		int16_t shConfigID;         //配置文件ID 0：Dynamic.cfg	1：Catalog.svr2：Logic.svr
		//3：Download.svr 4：Status.svr 5：Personal.svr
		char shLocalHash[HASH_CODE_LEN];
	};

	struct stCSBlockHash
	{
		int16_t shBlockID;          //本地目录树缓存 block id
		char  BlockHash[HASH_CODE_LEN];//hash code
	};

	struct stCSStatistic
	{
		int32_t nFileLen;           //统计数据
		int32_t nActualFileLen;
		char  FileContent[max_cs_file_content_length];
	};


	/**	
	@brief  登录拉取初始数据块消息体
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
		//需要更新版本信息
		int16_t shModuleID;			//ModuleID列表 0：大厅 1：斗地主 2：牌类公共包
		int16_t shServerVersion;	
		int16_t shUpdateType;       //1：必要更新 2：建议更新	3：新增功能
	};

	struct stCSConfigData
	{
		//需要更新版本信息
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
	@brief  登录拉取初始数据块消息体 响应
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
	@brief  登录拉取初始数据块消息体 响应
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
			int8_t  byVerCLen;            //为兼容版本使用的长度
			stCSVersionControl pVersionControl[MAX_CS_MODULE_VERSION_COUNT];

			//配置信息
			int16_t  shConfigIDCount;
			stCSConfigData pConfigData[MAX_CS_CONFIG_FILE_COUNT];

			//节点信息
			int16_t  shValidBlockCount;
			int16_t  pValidBlock[MAX_CS_BLOCK_COUNT];        //节点信息


			int32_t     nHallMMOnlineCount;
			int32_t     nHallOtherOnlineCount;
			int16_t     shPopulationBlockCount;
			stNodeOnline  pPopulationBlockData[MAX_NODE_POPULATION_COUNT];


			//是否有根节点
			int8_t    byHaveRoot;
			//根节点
			int16_t   shBlockID;
			int16_t   shNodeBlockLen;
			int32_t   shDecompressedLen;
			char      NodeBlockData[max_cs_file_content_length];			
		};

		stCSResponseGetInitInfo m_stBody;

	private:
	};

	/**	
	@brief  得到目录块数据包体
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
			int8_t  byType;      //0：NodeBlock	1：PopulationBlock
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
	@brief  得到目录块数据数包体 响应
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
		int8_t  byType;      //0：NodeBlock	1：PopulationBlock
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
	@brief  查询结点所在块
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

		int16_t m_nFlag;				//一些标识，目前使用：第0，1 bit表示压缩算法，若为空，则表示不压缩。1：表示7Zip压缩

	//	int16_t m_nServerConfigDataSize;
		int32_t m_iServerConfigDataSize; //修改为32位 20090622 by ankye	
		char m_szServerConfigData[max_server_config_data_size];		//服务器通用配置

	//	int16_t m_nServerRoomDataSize;
		int32_t m_iServerRoomDataSize;  //修改为32位 20090622 by ankye	
		char m_szServerRoomData[max_server_room_data_size];			//服务器房间配置

		//int16_t m_nDirtyWordsSize;
		//char m_szDirtyWords[max_dirty_words_size];					//敏感词库

	protected:
	private:
	};

	/**	
	@brief  SS_MSG_REPORTSTATICS 上报人数信息
	*/
	struct stRoomStatics
	{
		int32_t m_iRoomID;
		int16_t m_nMMCount;		//房间内MM的数目
		int16_t m_nOtherCount;	//除MM外，其它人个数
	};

	int32_t encode_room_statics(char** pOut, stRoomStatics& statics);
	int32_t decode_room_statics(char** pOut, stRoomStatics& statics);
	//从*pOut跳过iByteCount个字节
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

		int16_t m_nFlag;				//一些标识，目前使用：第0，1 bit表示压缩算法，若为空，则表示不压缩。1：表示7Zip压缩

		int16_t m_nServerConfigDataSize;
		char m_szServerConfigData[max_state_server_data_size];		//state server服务器通用配置

	protected:
	private:
	};

	/**	
	@brief  state server上报人数信息
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
		int32_t m_nMMCount;		//大厅内MM的数目
		int32_t m_nOtherCount;	//除MM外，其它人个数

	protected:
	private:
	};

	/**	
	@brief  WS_MSG_REPORTSTATICS web节点上报人数协议
	*/
	struct stWebRoomStatics
	{
		int32_t m_iRoomID;
		int32_t m_nMMCount;		//房间内MM的数目
		int32_t m_nOtherCount;	//除MM外，其它人个数
	};

	int32_t encode_web_room_statics(char** pOut, stWebRoomStatics& statics);
	int32_t decode_web_room_statics(char** pOut, stWebRoomStatics& statics);
	//从*pOut跳过iByteCount个字节
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

