#ifndef _CL_BLACKMARKET_DEFINE_H_
#define _CL_BLACKMARKET_DEFINE_H_

#include <CLDefine.h>
#include <CLItemDefine.h>
#include <CLSystemValueDataEntry.h>
/**
*@brief ������������
*/
enum BLACKMARKET_TYPE
{
	BMT_INVALID		  = 0,
	BMT_FIXED_PRICE   = 1, //�̶��۸�
	BMT_AUCTION_PRICE = 2, //���ļ۸�
};

/**
*@brief ������������
*/
enum BLACKMARKET_SUBTYPE
{
	BMST_NORNAL = 0,	  //����
	BMST_FIXED_UPLIMIT = 1,  //�̶��չ�����
};

/**
*@brief ���Ľ��
*/
enum BLACKMARKET_AUTION_RESULT
{
	BMTAR_INVAIL = 0, //δ����
	BMTAR_FALI   = 1, //ʧ��,�˻�����
	BMTAR_SUCC   = 2, //�ɹ�,���Ž��
};

/**
*@brief ������Ʒ����״̬
*/
enum BLACKMARKET_AUCTION_ITEM_STATE
{
	BMAIS_INVALID     = 0,  //��Ч 
	BMAIS_CAN_AUCTION = 1,  //���Ծ���
	BMAIS_APPLYED	  = 2,  //������
	BMAIS_TRANSED	  = 3,  //�ѽ���
	BMAIS_MAX, 
};

enum BLACKMARKET_AUCTION_GROUP_TYPE
{
	BMAGT_NONE	= 0,	
	BMAGT_ONE	= 1,	//1,20����װ
	BMAGT_TWO	= 2,	//2,30����װ
	BMAGT_THR	= 3,	//3,40����װ
	BMAGT_FOUR	= 4,	//4,50����װ
	BMAGT_FIR	= 5,	//5,55����װ
	BMAGT_SIX	= 6,	//6,50����װ
	BMAGT_SVEN	= 7,	//7,55����װ
};

/**
*@brief ���������չ���Ʒ����Ϣ
*/
struct BlackMarketAuctionInfo
{
	BlackMarketAuctionInfo()
	{
		//memset(this, 0, sizeof(*this));
		guid = 0;
		op_activity_id = 0;
		back_buy_item_id = 0;
		back_buy_type = 0;
		price = 0;
		begin_time = 0;
		end_time = 0;
		off_sale = 0;
		recommend_price = 0;
		price_lower_limit = 0;
		price_upper_limit = 0;
		state = 0;
		auctioner_guid = 0;
		auction_price = 0;
	}

	AVALON_DEFINE_NETSERIALIZATION()
	{
		stream & guid & back_buy_item_id & back_buy_type & price & begin_time & end_time & recommend_price 
			& price_lower_limit & price_upper_limit & state & auctioner_guid & auctioner_name & auction_price;
	}

	// Ψһid
	ObjID_t		guid;			
	// ������Ӫ�id
	UInt32		op_activity_id;
	// �ع���Ʒid
	UInt32		back_buy_item_id;
	// �ع�����
	UInt8		back_buy_type;
	// �̶��չ��۸�
	UInt32		price;
	// ���Ŀ�ʼʱ��
	UInt32		begin_time;
	// ���Ľ���ʱ��
	UInt32		end_time;
	// �Ƿ��¼�
	UInt8		off_sale;
	// �Ƽ��۸�
	UInt32		recommend_price;
	// �۸�����
	UInt32		price_lower_limit;
	// �۸�����
	UInt32		price_upper_limit;
	// ״̬(ö��BLACKMARKET_ITEM_AUCTION_STATE)
	UInt8		state;
	// ���Ľ�ɫid
	ObjID_t		auctioner_guid;
	// ����������
	std::string auctioner_name;
	// ����
	UInt32		auction_price;
};

#define GAME_PARAM_BLACKMARKET_TYPE "game_param_blackmarket_type"

#define GAME_PARAM_BLACKMARKET_BUYLIMIT  "game_param_blackmarket_buylimit"

#define BLACKMARKET_BUYLIMIT_RATE 80

#define CHECK_TIME_FIXED_PRICE 3*24*3600

#define SERVER_OPEN_TIME 2*30*24*3600

#define AUCTION_END_TIME_FIX  5*60

//�Ѿ�����Ķ�������ʱ��
#define HANDLED_TRANS_PERSISIT_TIME  14*24*3600


#endif