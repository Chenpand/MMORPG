#ifndef __CHARGE_MANAGER_H__
#define __CHARGE_MANAGER_H__

#include <CLDefine.h>
#include <AvalonSingleton.h>
#include <CLBillingProtocol.h>
#include <AvalonSimpleTimer.h>
#include "ChargeOrderContext.h"

enum ChargeResult
{
	// �ɹ�
	CHARGE_SUCCESS,
	// ��������
	CHARGE_INVALID_PARAM,
	// ��Ҳ�����
	CHARGE_PLAYER_UNEXIST,
	// ��Ϸ��������
	CHARGE_INVALID_SERVER_ID,
	// �����Ѵ���
	CHARGE_REPEAT_ORDER,
	// ͸����Ϣ����
	CHARGE_INVALID_EXTRA_DATA,
	// ǩ��У�����
	CHARGE_INVALID_SIGN,
	// ���ݿ����
	CHARGE_DB_ERROR,
	// ��������
	CHARGE_OTHER,
};

class WSPlayer;
class ChargeManager : public Avalon::Singleton<ChargeManager>
{
public:
	/**
	*@brief ��ʼ��
	*/
	bool Init();

	/**
	*@brief ��ʼ��
	*/
	void OnTick(const Avalon::Time& now);

	/**
	*@brief ���س�ֵ����
	*/
	bool LoadChargeGoods();

	/**
	*@brief ���س�ֵ����
	*/
	bool AddChargePacket(const ChargePacket& packet);

	/**
	*@brief ���������Ʒ�б�
	*/
	void OnPlayerQueryChargeGoods(WSPlayer* player);

	/**
	*@brief ���������Ʒ�б�
	*/
	void OnPlayerQueryChargePacketGoods(WSPlayer* player);

	/**
	*@brief �ж��ܷ����ֵ��Ʒ
	*/
	UInt32 OnPlayerCanBuyChargeGoods(WSPlayer* player, UInt32 goodsId);

	/**
	*@brief �ж��ܷ����ֵ���
	*/
	UInt32 OnPlayerCanBuyChargePacket(WSPlayer* player, UInt32 goodsId);

	/**
	*@brief ��ֵ����
	*/
	void ReCharge(UInt32 connid, const std::string& orderId);

	/**
	*@brief ��ʼ��ֵ
	*/
	void StartCharge(UInt32 connid, const std::string& channel, const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime, ChargeMallType mallType = ChargeMallType::CHARGE_MALL_MONEY_MANAGE );

	/**
	*@brief ������������
	*/
	void BuyChargePacket(UInt32 connid, const std::string& channel, const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime);

	/**
	*@brief ��������(scene -> world)
	*/
	void OnSceneSendGoodsRes(const std::string& orderId, UInt32 result);

public:
	// ��ֵ��֤����

	/**
	*@brief ��֤��������
	*/
	void VerifyOrderRet(ChargeOrderContext* context, ChargeOrderStatus status);

	/**
	*@brief ��֤����������������
	*/
	void VerifyChargePacketOrderRet(ChargeOrderContext* context, ChargeOrderStatus status);

	/**
	*@brief ��֤��ҷ��أ���Ҵ��ڣ�
	*/
	void CreateOrderRecord(ChargeOrderContext* context);

	/**
	*@brief ��ʼ����
	*/
	void StartSendGoods(ChargeOrderContext* context);

public:
	/**
	*@brief ��ȡ��������
	*/
	ChargePacket* FindChargePacket(UInt32 goodsId);
	
	/**
	*@brief ��ֵ����
	*/
	ChargeGoods* FindChargeGoods(UInt32 goodsId);

public:

	/**
	*@brief ���س�ֵ���
	*/
	void SendChargeResult(UInt32 connid, ChargeResult result);

protected:
	/**
	*@brief ��֤����ǩ��
	*/
	bool _VerifyOrderSign(const std::string& orderId, const std::string& token, UInt32 accid, ObjID_t roleId, UInt8 mallType, UInt32 goodsId, UInt32 chargeMoney, UInt32 orderTime);

	/**
	*@brief ������ڽ����еĶ���
	*/
	void _AddContext(ChargeOrderContext* context);

	/**
	*@brief ��ѯ���ڽ����еĶ���
	*/
	ChargeOrderContext* _FindContext(const std::string& orderId);

	/**
	*@brief ɾ�����ڽ����еĶ���
	*/
	void _DelContext(const std::string& orderId);

	/**
	*@brief ɾ�����ڽ����еĶ���
	*/
	void _DelContext(ChargeOrderContext* context);

private:
	// ���ڽ����еĳ�ֵ����
	HashMap<std::string, ChargeOrderContext*>		m_ChargingOrders;

	// ��ֵ��Ʒ
	std::vector<ChargeGoods>						m_ChargeGoods;

	// ��������
	std::vector<ChargePacket>						m_ChargePackets;

	// ��ֵ������ⶨʱ��
	Avalon::SimpleTimer								m_ChargeCheckTimer;

	friend class ReChargeQueryCallback;
	friend class QueryChargeOrderCallback;
	friend class QueryChargePlayerCallback;
	friend class QueryAccountCallback;
};

#endif