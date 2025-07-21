#ifndef _CL_BILLING_PROTOCOL_H_
#define _CL_BILLING_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLItemDefine.h"
#include "CLBillingDefine.h"

namespace CLProtocol
{
	/**
	 *@brief billing->admin billing���󷢻�
	 */
	class AdminBillingGiveItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_GIVEITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & accId & friendId & itemId & num & price & orderId & token & amt & payamt_coins & moneyType & taocanId;
		}

		//���id
		UInt32	accId;
		//����id
		std::string	friendId;
		//����id
		UInt32	itemId;
		//��������
		UInt32	num;
		//�۸�
		UInt32	price;
		//������
		std::string	orderId;
		//����
		std::string	token;
		//���������ܶ�
		std::string	amt;
		//�������Ѷ������ҵ���
		std::string	payamt_coins;
		//��������
		std::string	moneyType;
		//�ײ�id
		std::string	taocanId;
	};

	/**
	 *@brief admin->billing �������ظ�billing
	 */
	class AdminBillingGiveItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_GIVEITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result & level & isqqvip & qqviplvl;
		}

		//���
		UInt8	result;
		//��ҵȼ�
		UInt16	level;
		//�Ƿ����
		UInt16	isqqvip;
		//qq vip�ȼ�
		UInt16	qqviplvl;
	};

	/**
	 *@brief admin->world billing���󷢻�
	 */
	class WorldBillingGiveItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GIVEITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & friendId & itemId & num & price & orderId & token & amt & payamt_coins & moneyType & taocanId;
		}

		//����id
		UInt32	nodeId;
		//���id
		UInt32	accId;
		//����id
		ObjID_t	friendId;
		//����id
		UInt32	itemId;
		//��������
		UInt32	num;
		//�۸�
		UInt32	price;
		//������
		std::string	orderId;
		//����
		std::string	token;
		//���������ܶ�
		std::string	amt;
		//�������Ѷ���������
		std::string	payamt_coins;
		//��������
		std::string	moneyType;
		//�ײ�id
		std::string	taocanId;
	};

	/**
	 *@brief billing->admin �����ɫ��Ϣ                                                                     
	 */
	class AdminBillingRoleInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_ROLEINFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId;
		}

	public:
		//����
		UInt32	zoneId;
		//�˺�id
		UInt32	accId;
	};

	/**
	 *@brief admin->billing ��ɫ��Ϣ����                                                                     
	 */
	class AdminBillingRoleInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(ADMIN_BILLING_ROLEINFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & name & level & sex & gold;
		}

	public:
		//�����
		std::string	name;
		//�ȼ�
		UInt16	level;
		//�Ա�
		UInt8	sex;
		//Ԫ����
		UInt32	gold;
	};

	/**
	 *@brief server->billing ��������
	 */
	class WorldBillingGiveItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GIVEITEM_RET)
	public:
		WorldBillingGiveItemRet():nodeId(0), accId(0), result(0), level(0), isqqvip(0), qqviplvl(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & accId & result & level & isqqvip & qqviplvl;
		}

		//����id
		UInt32	nodeId;
		//����˺�
		UInt32	accId;
		//���
		UInt8	result;
		//��ҵȼ�
		UInt16	level;
		//�Ƿ����
		UInt16	isqqvip;
		//qq vip�ȼ�
		UInt16	qqviplvl;
	};

	/**
	 *@brief client->server ����ǩ��
	 */
	class WorldBillingSigReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_SIG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & openId & friendId & pid & num & price & zoneId;
		}

		//openid
		std::string	openId;
		//����id
		ObjID_t		friendId;
		//����id
		UInt32		pid;
		//����
		UInt32		num;
		//�۸�
		UInt32		price;
		//����
		UInt32		zoneId;
	};

	/**
	 *@brief server->client ����ǩ��
	 */
	class WorldBillingSigRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_SIG_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & friendId & pid & num & price & time & sig;
		}

		//����id
		std::string	friendId;
		//����id
		UInt32		pid;
		//����
		UInt32		num;
		//�۸�
		UInt32		price;
		//ʱ���
		UInt32		time;
		//ǩ��
		std::string	sig;
	};

	/**
	 *@brief ֧����ɫ��Ϣ����                                                                     
	 */
	class WorldBillingRoleInfoReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_ROLEINFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & zoneId & accId;
		}

	public:
		UInt32	zoneId;
		UInt32	accId;
	};

	/**
	 *@brief ֧����ɫ��Ϣ����                                                                     
	 */
	class WorldBillingRoleInfoRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_ROLEINFO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & nodeId & name & level & sex & gold;
		}

	public:
		//�ڵ�id
		UInt32	nodeId;
		//�����
		std::string	name;
		//�ȼ�
		UInt16	level;
		//�Ա�
		UInt8	sex;
		//Ԫ����
		UInt32	gold;
	};

	/**
	*@brief ֧����ɫ��Ϣ����
	*/
	class WorldBillingGoodsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GOODS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}
	};

	/**
	*@brief ֧����ɫ��Ϣ����
	*/
	class WorldBillingGoodsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_GOODS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodsList;
		}

		std::vector<ChargeGoods>	goodsList;
	};

	/**
	*@brief ��ȡ��ֵ�����Ʒ
	*/
	class WorldBillingChargePacketReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_PACKET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief ���س�ֵ�����Ʒ
	*/
	class WorldBillingChargePacketRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_PACKET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goodsList;
		}

		std::vector<ChargePacket>	goodsList;
	};

	/**
	*@brief world -> scene ֪ͨscene��ʼ����
	*/
	class SceneBillingSendGoods : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BILLING_SEND_GOODS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderId & accid & roleId & mallType & chargeGoodsId & chargeItemId & chargeItemNum & vipScore & chargeMoney & 
				time & sendGoodTimeoutTime & rewards & onlinePlayerID & vipLevel & vipExp & totalChargeNum & point;
		}
		// ������
		std::string		orderId;
		// �˺�ID
		UInt32			accid;
		// ��ɫID
		ObjID_t			roleId;
		//�̳�����
		UInt8			mallType;
		//��ֵ��ƷID
		UInt32			chargeGoodsId;
		//��ֵ����ID
		UInt32			chargeItemId;
		//��������
		UInt32			chargeItemNum;
		// VIP����
		UInt32			vipScore;
		// ��ֵ���
		UInt32			chargeMoney;
		// ʱ���
		UInt32			time;
		// ������ʱʱ��
		UInt32			sendGoodTimeoutTime;
		// ��Ʒ
		ItemRewardVec	rewards;
		// ͬ�˻��µ�ǰ���ߵ����
		ObjID_t			onlinePlayerID;
		// vip�ȼ�
		UInt8			vipLevel;
		// vip����
		UInt32			vipExp;
		// �ܳ�ֵ��
		UInt32			totalChargeNum;
		// �ㄻ
		UInt32			point;
	};
	
	/**
	*@brief scene -> world ֪ͨworld�������
	*/
	class SceneBillingSendGoodsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BILLING_SEND_GOODS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & orderId & result;
		}

		// ������
		std::string		orderId;
		// ���
		UInt32			result;
	};

	/**
	*@brief scene->client ֪ͨ�ͻ��˷�����
	*/
	class SceneBillingSendGoodsNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BILLING_SEND_GOODS_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	

	/**
	*@brief client -> world ��������Ʒ(����ֻ�ж��ܲ�����)
	*/
	class WorldBillingChargeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mallType & goodsId;
		}

		// �̳�����
		UInt8			mallType;
		// ��ƷID
		UInt32			goodsId;
	};
	
	/**
	*@brief world->client �����ܷ�����Ʒ
	*/
	class WorldBillingChargeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_BILLING_CHARGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		// ���
		UInt32			result;
	};
};

#endif
