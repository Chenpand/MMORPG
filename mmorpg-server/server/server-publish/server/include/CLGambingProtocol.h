#ifndef _CL_GAMBING_PROTOCOL_H_
#define _CL_GAMBING_PROTOCOL_H_


#include "CLProtocolDefine.h"
#include "CLItemDefine.h"


namespace CLProtocol
{
	/**
	*@brief �ᱦ����������
	*/
	struct GambingParticipantInfo
	{
		GambingParticipantInfo()
		{
			participantId = 0;
			gambingItemId = 0;
			groupId = 0;
			investCopies = 0;
			investCurrencyId = 0;
			investCurrencyNum = 0;
			status = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & participantId & participantENPlatform & participantPlatform & participantServerName
				& participantName & gambingItemId & groupId & investCopies & investCurrencyId & investCurrencyNum 
				& gambingRate & status;
		}

		// roleid
		ObjID_t participantId;
		// ������ƽ̨(Ӣ��)
		std::string participantENPlatform;
		// ������ƽ̨(����)
		std::string participantPlatform;
		// �����߷�����
		std::string participantServerName;
		// ����������
		std::string participantName;
		// �ᱦ��Ʒid
		UInt32 gambingItemId;
		// �ᱦ�����
		UInt16 groupId;
		// Ͷ�����
		UInt32 investCopies;
		// Ͷ�����id
		UInt32 investCurrencyId;
		// Ͷ�����
		UInt32 investCurrencyNum;
		// �ᱦ����
		std::string gambingRate;
		// ״̬(��Ӧö�� GambingMineStatus)
		UInt8  status;
	};

	/**
	*@brief �ᱦ��Ʒ��Ϣ
	*/
	struct GambingItemInfo
	{
		GambingItemInfo()
		{
			gambingItemId = 0;
			gambingItemNum = 0;
			sortId = 0;
			itemDataId = 0;
			costCurrencyId = 0;
			unitPrice = 0;
			restGroups = 0;
			totalGroups = 0;
			curGroupId = 0;
			statusOfCurGroup = 0;
			soldCopiesInCurGroup = 0;
			totalCopiesOfCurGroup = 0;
			sellBeginTime = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & gambingItemNum & sortId & itemDataId & costCurrencyId & unitPrice & restGroups & totalGroups & rewardsPerCopy & curGroupId
				& statusOfCurGroup & soldCopiesInCurGroup & totalCopiesOfCurGroup & sellBeginTime & mineGambingInfo;
		}

		// �ᱦ��Ʒid
		UInt32 gambingItemId;
		// �ᱦ��Ʒ����
		UInt32 gambingItemNum;
		// ����
		UInt16 sortId;
		// ���߱�id
		UInt32 itemDataId;
		// ���ѻ���id
		UInt32 costCurrencyId;
		// ����(һ��)
		UInt32 unitPrice;
		// ʣ������
		UInt16 restGroups;
		// �ܵ�����
		UInt16 totalGroups;
		// ÿ�ݽ���
		std::vector<ItemReward> rewardsPerCopy;

		// ��ǰ�����
		UInt16 curGroupId;
		// ��ǰ��״̬(��Ӧö�� GambingItemStatus)
		UInt8  statusOfCurGroup;
		// ��ǰ�����۷���
		UInt32 soldCopiesInCurGroup;
		// ��ǰ���ܷ���
		UInt32 totalCopiesOfCurGroup;
		// ��ǰ�鿪��ʱ��
		UInt32 sellBeginTime;

		// �ҵĶᱦ����
		GambingParticipantInfo mineGambingInfo;
	};

	/**
	*@brief �ҵĶᱦ��Ϣ
	*/
	struct GambingMineInfo
	{
		GambingMineInfo()
		{
			gambingItemId = 0;
			gambingItemNum = 0;
			sortId = 0;
			itemDataId = 0;
			groupId = 0;
			soldCopies = 0;
			totalCopies = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & gambingItemNum & sortId & itemDataId & groupId & soldCopies & totalCopies
				& mineGambingInfo & gainersGambingInfo & participantsGambingInfo;
		}

		// ��Ʒid
		UInt32 gambingItemId;
		// �ᱦ��Ʒ����
		UInt32 gambingItemNum;
		// ����
		UInt16 sortId;
		// ���߱�id
		UInt32 itemDataId;
		// �����
		UInt16 groupId;
		// ���۳�����
		UInt32 soldCopies;
		// �ܷ���
		UInt32 totalCopies;

		// �ҵĶᱦ����
		GambingParticipantInfo mineGambingInfo;

		// ����߶ᱦ����
		GambingParticipantInfo gainersGambingInfo;

		// �����߶ᱦ����
		std::vector<GambingParticipantInfo> participantsGambingInfo;
	};

	/**
	*@brief �ᱦ���¼
	*/
	struct GambingGroupRecordData
	{
		GambingGroupRecordData()
		{
			groupId = 0;
			gainerId = 0;
			investCurrencyId = 0;
			investCurrencyNum = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & groupId & gainerENPlatform & gainerPlatform & gainerServerName
				& gainerId & gainerName & investCurrencyId & investCurrencyNum;
		}

		// ��id
		UInt16 groupId;
		// ������ƽ̨(Ӣ��)
		std::string gainerENPlatform;
		// Ӯ��ƽ̨
		std::string gainerPlatform;
		// Ӯ�ҷ�����
		std::string gainerServerName;
		// Ӯ��id
		ObjID_t gainerId;
		// Ӯ������
		std::string gainerName;
		// Ӯ��Ͷ�����id
		UInt32 investCurrencyId;
		// Ӯ��Ͷ���������
		UInt32 investCurrencyNum;
	};

	/**
	*@brief �ᱦ��Ʒ��¼
	*/
	struct GambingItemRecordData
	{
		GambingItemRecordData()
		{
			gambingItemId = 0;
			gambingItemNum = 0;
			sortId = 0;
			itemDataId = 0;
			soldOutTimestamp = 0;
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & gambingItemNum & sortId & itemDataId & soldOutTimestamp & groupRecordData;
		}

		// ��Ʒid
		UInt32 gambingItemId;
		// �ᱦ��Ʒ����
		UInt32 gambingItemNum;
		// ����
		UInt16 sortId;
		// ���߱�id
		UInt32 itemDataId;
		// ����ʱ��
		UInt32 soldOutTimestamp;

		// ���¼
		std::vector<GambingGroupRecordData>  groupRecordData;
	};

	/**
	*@brief client->server ����ᱦ��Ʒ����
	*/
	class PayingGambleReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_BUY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItemId & groupId & investCopies & bBuyAll;
		}

		// �ᱦ��Ʒid
		UInt32 gambingItemId;
		// �����
		UInt16 groupId;
		// Ͷ�����
		UInt32 investCopies;
		// �Ƿ���ȫ��ʣ�����
		UInt8  bBuyAll;
	};

	/**
	*@brief server->client ����ᱦ��Ʒ����
	*/
	class PayingGambleRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_BUY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & gambingItemId & groupId & investCopies & costCurrencyId & costCurrencyNum & itemInfo;
		}

		// ������
		UInt32 retCode;
		// �ᱦ��Ʒid
		UInt32 gambingItemId;
		// �����
		UInt16 groupId;
		// Ͷ�����
		UInt32 investCopies;
		// ���ѻ���id
		UInt32 costCurrencyId;
		// ���ѻ�����
		UInt32 costCurrencyNum;

		GambingItemInfo itemInfo;
	};

	/**
	*@brief client->server �ᱦ��Ʒ���ݲ�ѯ����
	*/
	class GambingItemQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_ITEM_QUERY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client �ᱦ��Ʒ���ݲ�ѯ����
	*/
	class GambingItemQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_ITEM_QUERY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingItems;
		}

		// ȫ���ᱦ��Ʒ
		std::vector<GambingItemInfo> gambingItems;
	};

	/**
	*@brief client->server �ҵĶᱦ���ݲ�ѯ����
	*/
	class GambingMineQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_MINE_QUERY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client �ҵĶᱦ���ݲ�ѯ����
	*/
	class GambingMineQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_MINE_QUERY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mineGambingInfo;
		}

		// �ҵĶᱦ��Ʒ��Ϣ
		std::vector<GambingMineInfo> mineGambingInfo;
	};

	/**
	*@brief client->server �ᱦ��¼��ѯ
	*/
	class GambingRecordQueryReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_RECORD_QUERY_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief server->client �ᱦ��¼��ѯ����
	*/
	class GambingRecordQueryRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_RECORD_QUERY_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gambingRecordDatas;
		}

		// �ᱦ��¼
		std::vector<GambingItemRecordData> gambingRecordDatas;
	};

	/**
	*@brief server->client �ᱦ�н�ͬ��
	*/
	class GambingLotterySync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_GAMBING_LOTTERY_SYNC)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & gainersGambingInfo & participantsGambingInfo;
		}

		// ����߶ᱦ����
		GambingParticipantInfo gainersGambingInfo;

		// �����߶ᱦ����
		std::vector<GambingParticipantInfo> participantsGambingInfo;
	};
}


#endif