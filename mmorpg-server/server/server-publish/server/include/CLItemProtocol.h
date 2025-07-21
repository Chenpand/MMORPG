#ifndef _CL_ITEM_PROTOCOL_H_
#define _CL_ITEM_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLItemDefine.h>
#include <CLEquipSchemeDefine.h>
#include <CLCreditPointDefine.h>

namespace CLProtocol
{
	/**
	*@brief client->server ������Ϣ
	*/
	struct ItemInfo
	{
		ItemInfo()
		{
			memset(this, 0, sizeof(*this));
		}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & num;
		}

		ObjID_t uid;
		UInt32 num;
	};

	/**
	 *@brief client->server ʹ�õ���
	 */
	class SceneUseItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & useAll & param1 & param2;
		}

	public:
		//����guid
		ObjID_t	itemid;
		//�Ƿ�һ��ʹ��ȫ��
		UInt8 useAll;
		//Ԥ������1
		UInt32 param1;
		//Ԥ������2
		UInt32 param2;
	};

	// ʹ�õ��߷���
	class SceneUseItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errcode;
		}

	public:
		UInt32 errcode;
	};

	// ���۵���
	class SceneSellItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELL_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & num;
		}

	public:
		//����guid
		ObjID_t	itemid;
		UInt16	num;
	};

	// ���۵��߷���
	class SceneSellItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELL_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errcode;
		}

	public:
		UInt32	errcode;
	};

	// ������
	class SceneEnlargePackage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENLARGE_PACKAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	// �����ӷ���
	class SceneEnlargePackageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENLARGE_PACKAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;
	};

	// ���ֿ����
	class SceneEnlargeStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENLARGE_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	//��ֿ�
	class ScenePushStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PUSH_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & storageType & itemid & num;
		}
	public:
		//�ֿ�����
		UInt8 storageType;
		//����guid
		ObjID_t	itemid;
		UInt16 num;
	};

	//��ֿⷵ��
	class ScenePushStorageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PUSH_STORAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
	public:
		UInt32 code;
	};

	//ȡ�ֿ�
	class ScenePullStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PULL_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & num;
		}
	public:
		//����guid
		ObjID_t	itemid;
		UInt16 num;
	};

	//ȡ�ֿⷵ��
	class ScenePullStorageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PULL_STORAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
	public:
		UInt32 code;
	};

	//һ����ֿ�
	class SceneOneKeyPushStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ONEKEY_PUSH_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	//һ����ֿⷵ��
	class SceneOneKeyPushStorageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ONEKEY_PUSH_STORAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
	public:
		UInt32 code;

	};

	//��չ�ֿⷵ��
	class SceneEnlargeStorageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENLARGE_STORAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
	public:
		UInt32 code;

	};

	//�����µ��߱�ʶ
	class SceneUpdateNewItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPDATE_NEW_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & packageType;
		}
	public:
		UInt8 packageType;
	};
	
	// װ�����л�
	class ItemSerialization : public Avalon::INetSerializable
	{
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errcode;
		}

	public:
		UInt32	errcode;
	};


	/**
	 *@brief client->server �ƶ�����
	 */
	class SceneMoveItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MOVE_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & num & targetpos;
		}

	public:
		//����guid
		ObjID_t	itemid;
		//�ƶ�����
		UInt16	num;
		//Ŀ��λ�� 
		ItemPos	targetpos;
	};

	//�ƶ����߷���
	class SceneMoveItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MOVE_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
	public:
		UInt32 code;
	};

	/**
	 *@brief server->client ͬ������
	 *		��ʽ��[Guid + itemid + data][Guid + itemid + data][Guid + itemid + data][Guid + itemid + data]...[0]
	 *		data : [ItemAttr(UInt8) + value][ItemAttr(UInt8) + value]...[0]
	 */
	class SceneSyncItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief server->client ͬ����������
	 *		��ʽ: Guid1 + Data1 + Guid2 + Data2 + ... + Guid[0]
	 *		Data: [ItemAttr(UInt8) + value][ItemAttr(UInt8) + value]...[0];
	 */
	class SceneSyncItemProperty : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_ITEM_PROPERTY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief server->client ֪ͨɾ��
	 */
	class SceneNotifyDeleteItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_DELETE_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

	public:
		//Ψһguid
		ObjID_t id;
	};

	/**
	 *@brief client->server����װ��
	 */
	class SceneRemakeItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REMAKE_EQUIP)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & remakeType & equip & mainMaterial & materials & bindFirst & param1 & param2 & param3;
		}

	public:
		//��������(EquipRemakeType)
		UInt8	remakeType;
		//װ��
		ObjID_t	equip;
		//������
		ObjID_t	mainMaterial;
		//��������
		std::vector<ObjID_t> materials;
		//�Ƿ����Ȱ�
		UInt8	bindFirst;
		//�������1
		UInt32	param1;
		//�������2
		UInt32	param2;
		//�������3
		UInt8	param3;
	};

	/**
	 *@brief server->client ����װ�����
	 */
	class SceneRemakeItemResult : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REMAKE_EQUIP_RESULT)
	public:
		SceneRemakeItemResult()
			:remakeType(0), result(0), param1(0), param2(0), param3(0){}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & remakeType & result & param1 & param2 & param3 & rewards;
		}

	public:
		//��������(EquipRemakeType)
		UInt8	remakeType;
		//��� 0��ʾʧ�� 1��ʾ�ɹ�
		UInt8	result;
		//����1
		UInt32	param1;
		//����2
		UInt32	param2;
		//����3
		UInt32	param3;
		//�����б�
		std::vector<ItemReward>	rewards;
	};

	struct TransItemData
	{
		TransItemData() : itemNum(0), isEqualItem(false) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemNum & isEqualItem;
		}

		// ��������
		UInt32 itemNum;
		// �Ƿ�ʹ�õȼ���
		UInt32 isEqualItem;
	};

	/**
	 *@brief world->scene �������������
	 */
	class SceneCheckItemNumReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_ITEMNUM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & serial & bindSilver & silver & gold & bindGold & reviveCoin & 
				guildContri & items & nobindItems & counterName & maxCount & needCount & detailItems 
				& param & isWeeklySetTask & requestServerType;
		}

		SceneCheckItemNumReq()
		{
			param = 0;
			isWeeklySetTask = 0;
			requestServerType = 0;
		}
	public:
		//���
		ObjID_t	playerId;
		//��������
		UInt16	serial;
		//����
		UInt32	bindSilver;
		//����
		UInt32	silver;
		//��
		UInt32	gold;
		//���
		UInt32  bindGold;
		//�����
		UInt32  reviveCoin;
		//���ṱ��
		UInt32	guildContri;
		//�����б�
		std::map<UInt32, TransItemData>	items;
		//�ǰ�����б�
		std::map<UInt32, UInt32>	nobindItems;
		//��������
		std::string	counterName;
		//������
		UInt32	maxCount;
		//��Ҫ�Ĵ���
		UInt32	needCount;
		//����ĵ���
		std::vector<DetailItem>	detailItems;
		//dungeonServer��raceId
		ObjID_t param;
		//�Ƿ����ܳ������Ͻ����߼��
		UInt8  isWeeklySetTask;
		//��������Դ��������
		UInt8  requestServerType;
	};

	/**
	 *@brief scene->world ��������������
	 */
	class SceneCheckItemNumRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_ITEMNUM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & serial & counterTimes & result & param;
		}

	public:
		//���
		ObjID_t	playerId;
		//�������
		UInt16	serial;
		// ��ǰ�����������ѯ�����Ļ���
		UInt32	counterTimes;
		//��� ��ӦErrorCode
		UInt32	result;
		//dungeonServer��raceId
		ObjID_t param;
	};

	/**
	 *@brief world->scene �۳�����
	 */
	class SceneRemoveItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REMOVEITEM_REQ)
	public:
		SceneRemoveItemReq():playerId(0), bindSilver(0), silver(0), gold(0), bindGold(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & reason & bindSilver & silver & gold & bindGold & reviveCoin & 
				guildContri & items & nobindItems & counterName & needCount & detailItems;
		}

	public:
		//���
		ObjID_t	playerId;
		//ԭ��
		std::string	reason;
		//����
		UInt32	bindSilver;
		//����
		UInt32	silver;
		//��
		UInt32	gold;
		//�󶨽�
		UInt32	bindGold;
		//�����
		UInt32	reviveCoin;
		//���ṱ��
		UInt32	guildContri;
		//�����б�
		std::map<UInt32, TransItemData>	items;
		//�ǰ�����б�
		std::map<UInt32, UInt32> nobindItems;
		//��������
		std::string	counterName;
		//��Ҫ�Ĵ���
		UInt32	needCount;
		//�������
		std::vector<DetailItem> detailItems;
	};

	/**
	 *@brief world->scene ֪ͨ��������������
	 */
	class SceneUnlockItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UNLOCKITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId;
		}

	public:
		//���
		ObjID_t	playerId;
	};

	/**
	 *@brief world->scene ͨ��Ψһid�Ƴ�����
	 */
	class SceneRemoveItemObjReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REMOVE_ITEMOBJ_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & reason & itemId;
		}

	public:
		//���
		ObjID_t	playerId;
		//ԭ��
		std::string	reason;
		//����
		ObjID_t	itemId;
	};

	/**
	 *@brief scene->client ͬ�����۲���װ��
	 *	 	��ʽ��playerId(ObjID_t) + [Guid + itemid + data][Guid + itemid + data][Guid + itemid + data][Guid + itemid + data]...[0]
	 *		data : [ItemAttr(UInt8) + value][ItemAttr(UInt8) + value]...[0]
	 */
	class SceneSyncWatchEquips : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_WATCH_EQUIPS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	 *@brief  client->server �ϳɵ��ߣ�������
	 */
	class SceneComposeItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_COMPOSE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & quality & num & materialNum & bind;
		}

		//�ϳ�id
		UInt32	itemId;
		//�ϳ�Ʒ��
		UInt8	quality;
		//�ϳ�����
		UInt16	num;
		//����1����
		UInt16	materialNum;
		//�Ƿ����Ȱ�  1���Ȱ� 0ֻ�÷ǰ�
		UInt8	bind;
	};

	/**
	 *@brief client->server �������
	 */
	class SceneTrimItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRIM_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pack;
		}

		// �������
		UInt8 pack;
	};

	/**
	*@brief client->server �������
	*/
	class SceneTrimItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRIM_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// ����ֵ
		UInt32 code;
	};

	/**
	 *@brief server->client ��ʾʹ�õ�����Ч��������
	 */
	class SceneShowUseItemEffect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOW_USEITEM_EFFECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId & itemId;
		}

		//Ŀ��id
		ObjID_t	targetId;
		//����id
		UInt32	itemId;
	};

	/**
	 *@brief server->client ֪ͨ��õ���
	 */
	class SceneNotifyGetItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_GETITEM)
	public:
		SceneNotifyGetItem()
		{
			notifyType = 1;
		}
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & sourceType & notifyType & items;
		}

		//��Դ����
		UInt32  sourceType;
		//������ʽ
		UInt8  notifyType; //0:�ɵ�, 1:�µ�
		//�õ��ĵ���
		std::vector<ItemReward> items;
	};

	/**
	*@brief client->server �ֽ�װ��
	*/
	class SceneEquipDecompose : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_DECOMPOSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{		
			stream & uidList;
		}

		// װ��uid
		std::vector<ObjID_t>	uidList;
	};

	/**
	*@brief client->server ǿ��װ��
	*/
	class SceneEquipStrengthen : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_STRENGTHEN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & useUnbreak & strTicket;
		}

		// װ��uid
		ObjID_t equipUid;
		// �Ƿ�ʹ�ñ�����  0Ϊ��ʹ�� 1Ϊʹ�� 2Ϊʹ��һ����ǿ���� 3Ϊͬʱʹ�ñ���ȯ��һ����ǿ����
		UInt8 useUnbreak;
		// ʹ�õ�ǿ��ȯ
		ObjID_t strTicket;
	};

	// ǿ������
	class SceneEquipStrengthenRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_STRENGTHEN_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & matNums;
		}

		// ����ֵ
		UInt32 code;
		// ��������
		std::vector<ItemReward> matNums;
	};

	/**
	*@brief client->server װ������
	*/
	class SceneEquipEnhanceUpgrade : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_UPGRADE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & useUnbreak & strTicket;
		}

		// װ��uid
		ObjID_t equipUid;
		// �Ƿ�ʹ�ñ����� 0Ϊ��ʹ�� 1Ϊʹ�� 2Ϊʹ��һ����ǿ���� 3Ϊͬʱʹ�ñ���ȯ��һ����ǿ����
		UInt8 useUnbreak;
		// ʹ�õ�����ȯ
		ObjID_t strTicket;
	};

	// װ����������
	class SceneEquipEnhanceUpgradeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_UPGRADE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & matNums;
		}

		// ����ֵ
		UInt32 code;
		// ��������
		std::vector<ItemReward> matNums;
	};

	//scene->client��Ϣװ�������Ϣ
	class SceneEquipEnhanceClear : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CLEAR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & stuffID;
		}

		// װ��uid
		ObjID_t equipUid;
		//ʹ�õĲ��ϵ���id
		UInt32	stuffID;
	};

	// ��Ϣװ�������Ϣ����
	class SceneEquipEnhanceClearRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CLEAR_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// ����ֵ
		UInt32 code;
	};

	//scene->client��Ϣװ�������ɺ���װ��
	class SceneEquipEnhanceRed : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_RED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & stuffID;
		}

		// װ��uid
		ObjID_t equipUid;
		//ʹ�õĲ��ϵ���id
		UInt32	stuffID;
	};

	// ��Ϣװ�������ɺ���װ������
	class SceneEquipEnhanceRedRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_RED_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// ����ֵ
		UInt32 code;
	};

	//scene->client����װ������ת��
	class SceneEquipEnhanceChg : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CHG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & enhanceType & stuffID;
		}
		// װ��uid
		ObjID_t equipUid;
		// ת��·��
		UInt8 enhanceType;
		//ʹ�õĲ��ϵ���id
		UInt32	stuffID;
	};

	// ����װ������ת������
	class SceneEquipEnhanceChgRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CHG_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// ����ֵ
		UInt32 code;
	};


	// �������Ϻϳ�
	class SceneEnhanceMaterialCombo : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_MATERIAL_COMBO)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goalId & goalNum;
		}
		UInt32 goalId;
		UInt32 goalNum;
	};

	// �������Ϻϳɷ���
	class SceneEnhanceMaterialComboRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_MATERIAL_COMBO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// ����ֵ
		UInt32 code;
	};


	// �ֽⷵ��
	class SceneEquipDecomposeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_DECOMPOSE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & getItems;
		}

		// ����ֵ
		UInt32 code;
		// �õ��ֽ���Ʒ
		std::vector<ItemReward> getItems;
	};
	

	/**
	*@brief client->server �̵��ѯ
	*/
	class SceneShopQuery : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_QUERY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & cache;
		}

		//�̵�ID
		UInt8 shopId;
		//�Ƿ��л���
		UInt8 cache;
	};
	
	/**
	*@brief server->client �̵��ѯ����
	*/
	class SceneShopQueryRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_QUERY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		//�Ƿ���Ҫ����
		UInt32 code;
	};

	/**
	*@brief client->server �̵깺��
	*/
	class SceneShopBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & shopItemId & num & costExtraItems;
		}

		//�̵�ID
		UInt8 shopId;
		//��ƷID
		UInt32 shopItemId;
		//����
		UInt16 num;
		//�����������
		std::vector<ItemInfo> costExtraItems;
	};
	
	/**
	*@brief server->client �̵깺�򷵻�
	*/
	class SceneShopBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & shopItemId & newNum & leaseEndTimeStamp;
		}

		//����ֵ
		UInt32 code;
		//��Ʒid
		UInt32 shopItemId;
		//��������
		UInt16 newNum;
		//����ʱ���
		UInt32 leaseEndTimeStamp;
	};

	/**
	*@brief client->server �̵�ˢ��
	*/
	class SceneShopRefresh : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_REFRESH)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId;
		}

		UInt8 shopId;
	};

	/**
	*@brief client->server �����̵�ʣ��ˢ�´���
	*/
	class SceneShopRefreshNumReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_REFRESHNUM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId;
		}

		UInt8 shopId;
	};
	
	/**
	*@brief server->client �����̵�ʣ��ˢ�´�������
	*/
	class SceneShopRefreshNumRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_REFRESHNUM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & restRefreshNum & maxRefreshNum;
		}

		UInt8 shopId;
		UInt8 restRefreshNum;
		UInt8 maxRefreshNum;
	};

	/**
	*@brief client->server ����ʱװѡ������
	*/
	class SceneFashionAttrSelReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FASHION_ATTR_SEL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & attrid;
		}

		UInt64 uid;
		UInt32 attrid;
	};
	
	/**
	*@brief server->client ����ʱװѡ�����Է���
	*/
	class SceneFashionAttrSelRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FASHION_ATTR_SEL_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		UInt32 result;
	};


	/**
	*@brief client->server ����ʹ�õ���
	*/
	class SceneDungeonUseItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DUNGEON_USE_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & num;
		}

		UInt32 itemId;
		UInt16 num;
	};

	/**
	*@brief server->client �̵�ˢ�·���
	*/
	class SceneShopRefreshRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_REFRESH_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;
	};

	/**
	*@brief client->server һ���ֽ�װ��
	*/
	class SceneOneKeyDecompose : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ONEKEY_DECOMPOSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & color;
		}

		UInt8 color;		// ��ɫ����λȡ��1<<0�ǰף�1<<1�������Դ�����
	};

	/**
	*@brief client->server һ���ֽ�װ������
	*/
	class SceneOneKeyDecomposeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ONEKEY_DECOMPOSE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;
	};

	/**
	*@brief client->server ��װ����
	*/
	class SceneSealEquipReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SEAL_EQUIP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid;
		}

		ObjID_t uid;		// ��װװ��uid
	};

	/**
	*@brief server->client ��װ���󷵻�
	*/
	class SceneSealEquipRet: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SEAL_EQUIP_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & inscriptionIds;
		}

		UInt32 code;		// ������
		std::vector<UInt32> inscriptionIds;	//����ID����
	};
	
	/**
	*@brief client->server ����װ����
	*/
	class SceneCheckSealEquipReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_SEAL_EQUIP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid;
		}

		ObjID_t uid;		// ��װװ��uid
	};
	
	/**
	*@brief server->client ����װ����
	*/
	class SceneCheckSealEquipRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_SEAL_EQUIP_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & needMatId & needNum;
		}

		UInt32 code;		// ������
		UInt16 needNum;		// ��Ҫ��������
		UInt32 needMatId;	// ��Ҫ����id
	};

	/**
	*@brief client->server ��������װ��Ʒ������
	*/
	class SceneRandEquipQlvReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RAND_EQUIP_QLV_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & autoUsePont & usePerfect;
		}

		ObjID_t uid;			// װ��uid
		UInt8	autoUsePont;	// �Ƿ��Զ�ʹ�õ�ȯ
		UInt8   usePerfect;     // �Ƿ�ʹ������ϴ���� 
	};


	/**
	*@brief client->server ��ħ����
	*/
	class SceneAddMagicReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADDMAGIC_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & magicUid & itemUid;
		}

		ObjID_t magicUid;		// ��ħ��uid
		ObjID_t itemUid;		// ����uid
	};

	/**
	*@brief server->client ��ħ����
	*/
	class SceneAddMagicRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADDMAGIC_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & itemUid & cardId & cardLev;
		}

		UInt32 code;
		ObjID_t itemUid;
		UInt32 cardId;
		UInt8	cardLev;
	};
	
	/**
	*@brief server->client ��ħ���ϳ�
	*/
	class SceneMagicCardCompReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGICCARD_COMP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & card_1 & card_2;
		}

		ObjID_t card_1;
		ObjID_t card_2;
	};

	/**
	*@brief server->client ��ħ���ϳɷ���
	*/
	class SceneMagicCardCompRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGICCARD_COMP_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & cardId & cardLev;
		}

		UInt32 code;
		UInt32 cardId;
		UInt8  cardLev;
	};

	
	
	/**
	*@brief server->client ��������װ��Ʒ������
	*/
	class SceneRandEquipQlvRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RAND_EQUIP_QLV_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;		// ������
	};
	
	/**
	*@brief server->client ʹ��ħ��������
	*/
	class SceneUseMagicJarReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_MAGICJAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & combo & opActId & param;
		}

		UInt32	id;		//����id
		UInt8	combo;	//�Ƿ�����
		UInt32 opActId;	//��Ӫ�id
		UInt32 param;
	};

	/**
	*@brief client->server ������ӻ���
	*/
	class SceneJarPointReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_JAR_POINT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief server->client ������ӻ��ַ���
	*/
	class SceneJarPointRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_JAR_POINT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & goldJarPoint & magJarPoint;
		}

		UInt32 goldJarPoint;
		UInt32 magJarPoint;
	};

	/**
	*@brief client->world ���󿪹��Ӽ�¼
	*/
	class WorldOpenJarRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_OPEN_JAR_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId;
		}

		UInt32 jarId;
	};

	/**
	*@brief world->client ���󿪹��Ӽ�¼����
	*/
	class WorldOpenJarRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_OPEN_JAR_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId & records;
		}

		UInt32 jarId;
		std::vector<OpenJarRecord> records;
	};

	/**
	*@brief client->world װ�����ղ鿴���޼�¼
	*/
	class WorldEquipRecoOpenJarsRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_EQRECO_OPENJARS_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief world->client ���󿪹��Ӽ�¼����
	*/
	class WorldEquipRecoOpenJarsRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_EQRECO_OPENJARS_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & records;
		}

		std::vector<OpenJarRecord> records;
	};

	/**
	*@brief scene->world ���󿪹��Ӽ�¼
	*/
	class WorldAddJarRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ADD_JAR_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerUId & jarId & jarName & name & itemId & num & isActive & isEqReco;
		}

		UInt64	playerUId;
		UInt32 jarId;
		std::string jarName;
		std::string name;
		UInt32 itemId;
		UInt32 num;
		UInt8 isActive;
		UInt8 isEqReco;
	};
	
	/**
	*@brief scene->world ����˺Ż�������
	*/
	class WorldGotAccountMoneyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GOT_ACCOUNT_MONEY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleid & accid & type & num & reason;
		}

		UInt64			roleid;
		UInt32			accid;
		UInt32			type;
		UInt32			num;
		std::string		reason;
	};

	/**
	*@brief client->server ʹ��ħ��������
	*/
	class SceneQuickUseItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICK_USEITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & idx & dungenId;
		}

		UInt8	idx;	//�����б��index
		UInt32	dungenId;
	};

	/**
	*@brief client->server ʹ��ħ��������
	*/
	class SceneQuickUseItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICK_USEITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32	code;	
	};

	/**
	*@brief server->client ʹ��ħ���޷���
	*/
	class SceneUseMagicJarRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_MAGICJAR_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & jarId & getItems & baseItem & getPointId & getPoint & getPointTime;
		}

		UInt32 code;							//������
		UInt32	jarId;
		std::vector<OpenJarResult> getItems;		//�鵽�ĵ���
		ItemReward	baseItem;				//��������
		UInt32	getPointId;					//��û�������
		UInt32	getPoint;						//��û�������
		UInt32	getPointTime;					//��û��ֱ�������
	};
	

	/**
	*@brief client->server ʱװ�ϳ�����
	*/
	class SceneFashionComposeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FASHION_MERGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fashionA & fashionB & composer & skySuitID & selComFashionID;
		}

		UInt64 fashionA;		
		UInt64 fashionB;		 
		UInt64 composer;
		UInt32 skySuitID;
		UInt32 selComFashionID;
	};

	/**
	*@brief server->client ʱװ�ϳɷ���
	*/
	class SceneFashionComposeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FASHION_MERGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & type & itemAId & numA & itemBId & numB & wingId;
		}

		UInt32 code;
		UInt8 type;
		UInt32 itemAId;
		UInt32 numA;
		UInt32 itemBId;
		UInt32 numB;
		UInt32 wingId;
	};

	/**
	*@brief client->server ʱװ�ϳɼ�¼����
	*/
	class SceneFashionMergeRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FASHION_MERGE_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & handleType;
		}

		UInt32 handleType;
	};

	class SceneEquipMakeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_MAKE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipId;
		}

		UInt32 equipId;
	};

	class SceneEquipMakeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_MAKE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;
	};

	

	/**
	 *@brief client->server �ͻ����������ͬ��������������������
	 */
	class SceneSyncCoins : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_COINS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & coins;
		}

		//����������
		UInt32	coins;
	};

	/**
	 *@brief server->client ֪ͨ�ͻ������ĵ���
	 */
	class SceneNotifyCostItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_COSTITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & quality & num;
		}

		//����id
		UInt32	itemId;
		//Ʒ��
		UInt8	quality;
		//����
		UInt16	num;
	};

	/**
	 *@brief �ϳɵ��߽����������
	 */
	class SceneComposeItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_COMPOSE_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//��� 1Ϊ�ɹ� 0Ϊʧ��
		UInt8	result;
	};

	/**
	 *@brief ��ʾ����ʹ�öԻ��򣨷�����
	 */
	class SceneShowItemDialog : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOW_ITEM_DIALOG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & word & options;
		}

		//����id
		ObjID_t	itemId;
		//��ʾ
		std::string	word;
		//ѡ���б�
		std::vector<ItemDialogOption> options;
	};

	/**
	 *@brief server->client ֪ͨ��ֵ�ɹ���������                                                             
	 */
	class SceneNotifyChargeSucc : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_CHARGE_SUCC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & point;
		}

	public:
		UInt32 point;
	};

	/**
	 *@brief  client->server ����һ������б�������                                                                
	 */
	class SceneExchangeItemsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXCHANGE_ITEMS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pos;
		}

	public:
		UInt32	pos;
	};

	/** 
	 *@brief  server->client ���ضһ������б�������                                                                  
	 */
	class SceneExchangeItemsRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXCHANGE_ITEMS_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pos & items;
		}

	public:
		UInt32	pos;
		std::vector<ItemExchangeInfo> items;
	};

	/**
	 *@brief client->server �һ����ߣ�������                                                                 
	 */
	class SceneExchangeItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXCHANGE_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pos & index & itemNum & bBind;
		}

	public:
		UInt32 pos;
		UInt32	index;
		UInt16	itemNum;
		UInt8	bBind;
	};

	/**
	*@brief client -> server ���߼�����ز���(����)
	*/
	class SceneItemLockReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_LOCK_REQ)
	
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemLockType & passwd & firstPasswd 
				   & secondPasswd  & lockRange & question & answer;
		}

	public:
		//���߼�����������(ItemLockType)
		UInt8 itemLockType;

		//ԭ����
		std::string passwd;

		//��һ��������
		std::string firstPasswd;

		//�ڶ���������
		std::string secondPasswd;

		//������Χ
		UInt32 lockRange;

		//��ʾ����
		std::string question;

		//��
		std::string answer;
	};

	/**
	*@brief server -> client ���߼�����ز��������������
	*/
	class SceneItemLockRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_LOCK_RET)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemLockType & result;
		}

	public:
		//���߼�����������(ItemLockType)
		UInt8 itemLockType;

		//���(0:ʧ�� 1:�ɹ�)
		UInt8 result;
	};

	/**
	*@brief server -> client ���߼�����ť״̬��������
	*/
	class SceneItemLockButtonState : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_LOCK_BUTTON_STATE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buttonState;
		}

	public:
		//���߼�����ť״̬(ItemLockButtonType)
		UInt8 buttonState;
	};

	/**
	*@brief server -> client ���߼���֪ͨ�ͻ����������������������
	*/
	class SceneItemLockInputPasswd : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_LOCK_INPUT_PASSWD)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};

	/**
	*@brief server -> client ͬ��������Χ��������
	*/
	class SceneItemLockRange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_LOCK_RANGE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & lockRange;
		}

	public:
		//���߼�����Χ
		UInt32 lockRange;
	};

	/**
	*@brief server -> client ͬ����ʾ���⣨������
	*/
	class SceneItemLockQuestion : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_LOCK_QUESTION)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & question;
		}

	public:
		//��ʾ����
		std::string question;
	};
	
	/**
	*@brief ���װ���̵�������
	*/
	struct RandEquipEntry
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & price & quality;
		}
		// װ��id
		UInt32	id;
		// �۸�
		UInt32	price;
		// Ʒ��
		UInt8	quality;
	};
	/**
	*@brief server->client ͬ�����װ���̵꣨������
	*/
	class SceneRandEquipShopSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RAND_EQUIP_SHOP_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uNextRefreshTime & vecNormal & vecVip;
		}
	public:
		// �´�ˢ��ʱ��
		UInt32	uNextRefreshTime;
		// ��ͨ�̵��б�
		std::vector<RandEquipEntry>	vecNormal;
		// vip�̵��б�
		std::vector<RandEquipEntry>	vecVip;
	};

	/**
	*@brief server->client ͬ�����װ���̵� add by jermaine
	*/
	struct ProtoShopItem
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopItemId & grid & restNum & vipLv & vipDiscount & discountRate & leaseEndTimeStamp;
		}
		//�̵����id
		UInt32	shopItemId;
		//����
		UInt8	grid;
		//ʣ������
		UInt16	restNum;
		//vip�ȼ�
		UInt8	vipLv;
		//vip�ۿ�
		UInt8	vipDiscount;
		//�ۿ���
		UInt32  discountRate;
		//���޽���ʱ���
		UInt32	leaseEndTimeStamp;
	};

	// ͬ���̵�
	class SceneShopSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & refreshCost & shopItemList & restRefreshTime & restRefreshNum & maxRefreshNum & restWeekRefreshTime & restMonthRefreshTime;
		}
	public:
		//�̵�id
		UInt32 shopId;
		// ˢ�»���
		UInt16 refreshCost;
		// ��ͨ�̵��б�
		std::vector<ProtoShopItem>	shopItemList;
		// ÿ��ˢ��ʣ��ʱ��
		UInt32 restRefreshTime;
		// ʣ��ˢ�´���
		UInt8 restRefreshNum;
		// ���ˢ�´���
		UInt8 maxRefreshNum;
		// ÿ��ˢ��ʣ��ʱ��
		UInt32 restWeekRefreshTime;
		// ÿ��ˢ��ʣ��ʱ��
		UInt32 restMonthRefreshTime;
	};

	// ͬ����Ʒ
	class SceneShopItemSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOPITEM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}
	};
	
	

	/**
	*@brief client->server �������װ����������
	*/
	class SceneRandEquipShopBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RAND_EQUIP_SHOP_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & index;
		}
	public:
		// �̵����ͣ�0 ��ͨ�̵꣬1 vip�̵�
		UInt8	type;
		// װ��������0 ��ʼ
		UInt8	index;
	};

	/**
	*@brief server->client �����֪ͨ�ͻ���ɾ��һ�����װ����������
	*/
	class SceneRandEquipShopDel : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RAND_EQUIP_SHOP_DEL)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & index;
		}
	public:
		// �̵����ͣ�0 ��ͨ�̵꣬1 vip�̵�
		UInt8	type;
		// װ��������0 ��ʼ
		UInt8	index;
	};

	/**
	 *@brief world->scene ֪ͨ�������ӻ��ҵ���                                                                     
	 */
	class SceneAddMoneyItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_MONEYITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & reason & type & num;
		}

	public:
		ObjID_t		playerId;
		std::string	reason;
		UInt8		type;
		UInt32		num;
	};

	/**
	*@brief scene->world ��չ��������
	*/
	class SWEnlargeStorageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SW_ENLARGE_STORAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & accId & newSize & cost & costItemId;
		}

	public:
		ObjID_t		playerId;
		UInt32		accId;
		UInt32		newSize;
		UInt32		cost;
		UInt32		costItemId;
	};

	/**
	*@brief world->scene ��չ�������ӷ���
	*/
	class WSEnlargeStorageRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WS_ENLARGE_STORAGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & code & cost & costItemId & newSize;
		}

	public:
		ObjID_t	playerId;
		UInt32 code;
		UInt32 cost;
		UInt32 costItemId;
		UInt32 newSize;
	};

	/**
	*@brief client->scene ���õ��³�ҩˮ����
	*/
	class CSSetDungeonPotionReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_DUNGEON_POTION_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & potionId & pos;
		}

	public:
		UInt32	potionId;
		UInt8	pos;
	};

	/**
	*@brief scene->client ���õ��³�ҩˮ���󷵻�
	*/
	class SCSetDungeonPotionRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_DUNGEON_POTION_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

	public:
		UInt32	code;
	};

	/**
	*@brief client->scene ������������
	*/
	class CSRenewTimeItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RENEW_TIME_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUid & durationDay;
		}

	public:
		UInt64	itemUid;				//����uid
		UInt32	durationDay;			//����ʱ��(day),0������
	};
	
	/**
	*@brief client->scene �����������󷵻�
	*/
	class SCRenewTimeItemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RENEW_TIME_ITEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

	public:
		UInt32	code;
	};

	/**
	*@brief scene->client ֪ͨ�ͻ������ѵ���״̬�仯
	*/
	class SCNotifyTimeItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_TIME_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items;
		}

	public:
		std::vector<TimeItem>	items;
	};

	/**
	*@brief client->scene ������ħ��
	*/
	class CSOpenMagicBoxReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OPEN_MAG_BOX_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & isBatch;
		}

	public:
		UInt64	uid;
		UInt8	isBatch;
	};

	/**
	*@brief scene->client ������ħ�з���
	*/
	class SCOpenMagicBoxRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_OPEN_MAG_BOX_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & rewards;
		}

	public:
		UInt32	retCode;
		std::vector<OpenJarResult>	rewards;
	};

	/**
	*@brief client->scene ������ӱ���
	*/
	class SceneAddPreciousBeadReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_PRECIOUS_BEAD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & preciousBeadUid & itemUid;
		}

	public:
		// ����uid
		UInt64 preciousBeadUid;
		// ����uid
		UInt64 itemUid;
	};

	/**
	*@brief scene->client ��ӱ��鷵��
	*/
	class SceneAddPreciousBeadRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_PRECIOUS_BEAD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & preciousBeadId & itemUid;
		}

	public:
		// ������
		UInt32 code;
		// ����id
		UInt32 preciousBeadId;
		// ����uid
		UInt64 itemUid;
	};

	/**
	*@brief client->scene �������۵�������
	*/
	class SceneSellItemBatReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELL_ITEM_BAT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUids;
		}

	public:
		// ��������uid
		std::vector<UInt64> itemUids;
	};
	
	/**
	*@brief scene->client �������۵��߷���
	*/
	class SceneSellItemBatRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELL_ITEM_BAT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

	public:
		UInt32 code;
	};

	/**
	*@brief client->scene �齱
	*/
	class SceneDrawPrizeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DRAW_PRIZE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		// �齱id
		UInt32 id;
	};

	/**
	*@brief scene->client �齱����
	*/
	class SceneDrawPrizeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DRAW_PRIZE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & drawPrizeId & rewardId & retCode;
		}

		// �齱id
		UInt32 drawPrizeId;
		// ��Ʒid
		UInt32 rewardId;
		// ������
		UInt32 retCode;
	};

	/*
	*@brief client->scene װ�������ύ����
	*/
	class SceneEquipRecSubcmtReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_SUBCMT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUids;
		}

		// ��������uid
		std::vector<UInt64> itemUids;
	};

	/*
	*@brief scene->client װ�������ύ����
	*/
	class SceneEquipRecSubcmtRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_SUBCMT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & scoreItems & score;
		}

		//������
		UInt32 code;
		//װ�����ջ���
		std::vector<EqRecScoreItem>	scoreItems;
		//��ǰ�ܻ���
		UInt32 score;
	};

	/*
	*@brief client->scene װ���������
	*/
	class SceneEquipRecRedeemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_REDEEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equid;
		}

		//װ��uid
		UInt64 equid;
	};

	/*
	*@brief svrver->client װ����ط���
	*/
	class SceneEquipRecRedeemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_REDEEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & consScore;
		}

		//������
		UInt32 code;
		//�۳�����
		UInt32 consScore;
	};

	/*
	*@brief client->server װ�����ջ�����������
	*/
	class SceneEquipRecUpscoreReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_UPSCORE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equid;
		}

		//װ��uid
		UInt64 equid;
	};

	/*
	*@brief server->client װ�����ջ�����������
	*/
	class SceneEquipRecUpscoreRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_UPSCORE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & upscore;
		}

		//������
		UInt32 code;
		//��������ֵ
		UInt32 upscore;
	};

	/*
	*@brief client->server װ�����ˢ��ʱ�������
	*/
	class SceneEquipRecRedeemTmReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_REDEMTM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/*
	*@brief server->client װ�����ˢ��ʱ�������
	*/
	class SceneEquipRecRedeemTmRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_REDEMTM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & timestmap;
		}

		//ˢ��ʱ���
		UInt64 timestmap;
	};


	/*
	*@brief server->client ֪ͨװ����������
	*/
	class SceneEquipRecNotifyReset : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_NOTIFY_RSET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/*
	*@brief client->server װ��ת������
	*/
	class SceneEquipTransferReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_TRANSFER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equid & transferId;
		}

		//װ��uid
		UInt64	equid;
		//ת��ʯ���߶���id
		UInt64	transferId;
	};

	/*
	*@brief client->server װ��ת�Ʒ���
	*/
	class SceneEquipTransferRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_TRANSFER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		//������
		UInt32 code;
	};

	/*
	*@brief scene->client ͬ����������
	*/
	class SceneSyncMysticalMerchant : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_MYSTICAL_MERCHANT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		UInt32 id;
	};

	/**
	*@brief client->scene �ϴ���ħֵ
	*/
	class SceneSyncResistMagicReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_RESIST_MAGIC_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & resist_magic;
		}

		UInt32 resist_magic;
	};

	/**
	*@brief scene->world ͬ����ħֵ
	*/
	class WorldSyncResistMagic : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_SYNC_RESIST_MAGIC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & resist_magic;
		}
		ObjID_t roleId;
		UInt32 resist_magic;
	};

	/**
	*@brief world->scene �������ӵ���
	*/
	class SceneAddRewardReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_REWARD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & reason & notice & rewards;
		}

	public:
		ObjID_t roleId;
		std::string reason;
		UInt8 notice;
		ItemRewardVec rewards;
	};

	/**
	*@brief scene->world �������ӵ��߷���
	*/
	class SceneAddRewradRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_REWARD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}
	public:
		UInt32 result;
	};

	/**
	*@brief client->scene ���߼�����������
	*/
	class SceneItemOpLockReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_OP_LOCK_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & opType & itemUid;
		}

	public:
		// �������ͣ�0�������������
		UInt32 opType;
		// ����uid
		UInt64 itemUid;
	};

	/**
	*@brief scene->client ���߼�����������
	*/
	class SceneItemOpLockRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_OP_LOCK_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUid & ret;
		}
	public:
		// ����uid
		UInt64 itemUid;
		// ����ֵ
		UInt32 ret;
	};

	/**
	*@brief client->scene ����ʱװ������ʾ����
	*/
	class SceneSetFashionWeaponShowReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SHOW_FASHIONWEAPON_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isShow;
		}
	public:
		// �Ƿ�ʵ��ʱװ���� 0������ʾ 1:��ʾ
		UInt8 isShow;
	};

	/**
	*@brief scene->client ����ʱװ������ʾ����
	*/
	class SceneSetFashionWeaponShowRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SHOW_FASHIONWEAPON_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret;
		}
	public:
		// ����ֵ
		UInt32 ret;
	};

	/**
	*@brief client->scene ��װ�ڻʱװ�ϳ�����
	*/
	class SceneFashionChangeActiveMergeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FASHION_CHANGEACTIVE_MERGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & fashionId & ticketId &	choiceComFashionId;
		}
	public:
		UInt64 fashionId; //����ʱװ����id
		UInt64 ticketId;  //��װ�����id��ѡ�񱳰����������Ļ�װ��
		UInt32 choiceComFashionId; //ѡ��ض��ϳ�ʱװ����id
	};

	/**
	*@brief scene->client ��װ�ڻʱװ�ϳɷ���
	*/
	class SceneFashionChangeActiveMergeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_FASHION_CHANGEACTIVE_MERGE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & ret & type & commonId & advanceId & allMerged;
		}
	public:
		UInt32 ret;		  //���
		UInt8 type;       //[1]:δ�ϳ���������ʿʱװ [2]�ϳ���������ʿʱװ
		UInt32 commonId;  //�ϳ���ͨʱװid
		UInt32 advanceId; //�ϳ��Ľ�������ʿʱװid
		UInt8 allMerged;  //��װȫ���ϳ�
	};

	/**
	*@brief client->scene ǿ��ȯ�ϳ�����
	*/
	class SceneStrengthenTicketSynthesisReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_STRENGTHEN_TICKET_SYNTHESIS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & synthesisPlan;
		}
	public:
		// �ϳɷ���
		UInt32 synthesisPlan;
	};

	/**
	*@brief scene->client ǿ��ȯ�ϳɷ���
	*/
	class SceneStrengthenTicketSynthesisRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_STRENGTHEN_TICKET_SYNTHESIS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}
	public:
		// ������
		UInt32 retCode;
	};

	/**
	*@brief client->scene ǿ��ȯ�ں�����
	*/
	class SceneStrengthenTicketFuseReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_STRENGTHEN_TICKET_FUSE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pickTicketA & pickTicketB;
		}
	public:
		// ѡ��Ҫ�ںϵ�ǿ��ȯ
		ObjID_t pickTicketA;
		ObjID_t pickTicketB;
	};

	/**
	*@brief scene->client ǿ��ȯ�ںϷ���
	*/
	class SceneStrengthenTicketFuseRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_STRENGTHEN_TICKET_FUSE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}
	public:
		// ������
		UInt32 retCode;
	};

	/**
	*@brief client->scene ������Ƕ����
	*/
	class SceneMountPreciousBeadReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MOUNT_PRECIOUSBEAD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & preciousBeadUid & itemUid & holeIndex;
		}
	public:
		// ����uid
		UInt64 preciousBeadUid;
		// ����uid
		UInt64 itemUid;
		// ������
		UInt8  holeIndex;
	};

	/**
	*@brief scene->client ������Ƕ����
	*/
	class SceneMountPreciousBeadRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MOUNT_PRECIOUSBEAD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & preciousBeadId & itemUid & holeIndex;
		}
	public:
		// ������
		UInt32 code;
		// ����id
		UInt32 preciousBeadId;
		// ����uid
		UInt64 itemUid;
		// ������
		UInt8  holeIndex;
	};

	/**
	*@brief client->scene ����ժ������
	*/
	class SceneExtirpePreciousBeadReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXTIRPE_PRECIOUSBEAD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUid & holeIndex & pestleId;
		}
	public:
		//����uid
		UInt64 itemUid;
		//������
		UInt8 holeIndex;
		//��id
		UInt32 pestleId;
	};

	/**
	*@brief server->client ����ժ������
	*/
	class SceneExtirpePreciousBeadRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EXTIRPE_PRECIOUSBEAD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}
	public:
		// ������
		UInt32 code;
	};

	/**
	*@brief client->scene ������������
	*/
	class SceneUpgradePreciousbeadReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPGRADE_PRECIOUSBEAD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & mountedType & precGuid & equipGuid & eqPrecHoleIndex & precId & consumePrecId;
		}
	public:
		//���� [0]:δ��Ƕ [1]:����Ƕ
		UInt8		mountedType;
		//����guid ����0ʱ����
		ObjID_t		precGuid;
		//װ��guid ����1ʱ����
		ObjID_t		equipGuid;
		//װ����������� ����1ʱ����
		UInt8		eqPrecHoleIndex;
		//����id ����1ʱ����
		UInt32		precId;
		//ѡ�����ı���id 
		UInt32		consumePrecId;
	};

	/**
	*@brief scene->client �������鷵��
	*/
	class SceneUpgradePreciousbeadRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UPGRADE_PRECIOUSBEAD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & mountedType & equipGuid & precId & addBuffId & newPrebeadUid;
		}
	public:
		// ������
		UInt32	retCode;
		// ���� [0]:δ��Ƕ [1]:����Ƕ
		UInt8 mountedType;
		//װ��guid ����1ʱ����
		ObjID_t	equipGuid;
		// �����ɹ�����id
		UInt32	precId;
		// ����buff id
		UInt32  addBuffId;
		// �������µı���id ����[0]
		ObjID_t newPrebeadUid;
	};

	/**
	*@brief world->scene ���Ӿ�������
	*/
	class WorldIncreasePlayerExpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_INCREASE_PLAYER_EXP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & playerId & reason & incExp;
		}

	public:
		//���
		ObjID_t	playerId;
		//ԭ��
		std::string	reason;
		//���ӵľ���
		UInt64 incExp;
	};

	/**
	*@brief client->scene �����滻����
	*/
	class SceneReplacePreciousBeadReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLACE_PRECIOUSBEAD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUid & preciousBeadUid & holeIndex;
		}
	public:
		//����uid
		UInt64 itemUid;
		//�±���Id
		UInt64 preciousBeadUid;
		//������
		UInt8 holeIndex;
	};

	/**
	*@brief server->client �����滻����
	*/
	class SceneReplacePreciousBeadRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REPLACE_PRECIOUSBEAD_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & preciousBeadId & itemUid & holeIndex;
		}
	public:
		// ������
		UInt32 retCode;
		//�±���Id
		UInt32 preciousBeadId;
		//����uid
		UInt64 itemUid;
		//������
		UInt8 holeIndex;
	};

	/**
	*@brief world->scene  ����������
	*/
	class SceneCheckItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & serialId & type & owerId & itemId & param;
		}
	public:
		//��������id
		UInt32	serialId;
		//����
		UInt8	type;
		//����ӵ�������id
		ObjID_t owerId;
		//����id
		ObjID_t itemId;
		//���Ӳ���
		ObjID_t param;
	};

	/**
	*@brief client->scene ������޹����������
	*/
	class SceneArtifactJarBuyCntReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_BUY_CNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct ArtifactJarBuy
	{
		ArtifactJarBuy() :jarId(0), buyCnt(0), totalCnt(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId & buyCnt & totalCnt;
		}

		// ����id
		UInt32 jarId;
		// �������
		UInt32 buyCnt;
		// �ܴ���
		UInt32 totalCnt;
	};

	/**
	*@brief scene->client ������޹����������
	*/
	class SceneArtifactJarBuyCntRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ARTIFACT_JAR_BUY_CNT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & artifactJarBuyVec;
		}

		std::vector<ArtifactJarBuy> artifactJarBuyVec;
	};

	/**
	*@brief scene->globalActivity �����޹���齱�ʸ��ϱ�
	*/
	class GASArtifactJarLotterySignReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_SIGN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarIdVec;
		}

		// �齱�ʸ�Ĺ���ID
		std::vector<UInt32> jarIdVec;
	};

	/**
	*@brief client->globalActivity �����޻�齱��¼����
	*/
	class GASArtifactJarLotteryRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId;
		}
	
		// ����id
		UInt32 jarId;
	};

	struct ArtifactJarLotteryRecord
	{
		ArtifactJarLotteryRecord() : recordTime(0), itemId(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & serverName & playerName & recordTime & itemId;
		}

		bool operator < (const ArtifactJarLotteryRecord& rhs) const
		{
			return rhs.recordTime < recordTime;
		}

		// ����������
		std::string serverName;
		// �������
		std::string playerName;
		// ��¼ʱ��
		UInt64 recordTime;
		// ����id
		UInt32 itemId;
	};

	/**
	*@brief globalActivity->client �����޻�齱��¼����
	*/
	class GASArtifactJarLotteryRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId & lotteryRecordVec;
		}
		
		// ����id
		UInt32 jarId;
		// ��¼�б�
		std::vector<ArtifactJarLotteryRecord> lotteryRecordVec;
	};

	/**
	*@brief globalActivity->client �����޳齱֪ͨ
	*/
	class GASArtifactJarLotteryNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	/**
	*@brief client->globalActivity �����޳齱��������
	*/
	class GASArtifactJarLotteryCfgReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_CFG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	
	// �������ӽ�������
	struct ArtifactJarLotteryCfg
	{
		ArtifactJarLotteryCfg() : jarId(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId & rewardVec;
		}

		// ����id
		UInt32 jarId;
		// �����б�
		std::vector<ItemReward>	rewardVec;
	};

	/**
	*@brief globalActivity->client �����޻�齱��¼����
	*/
	class GASArtifactJarLotteryCfgRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_CFG_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & artifactJarCfgVec;
		}

		std::vector<ArtifactJarLotteryCfg> artifactJarCfgVec;
	};

	/**
	*@brief client->server װ������
	*/
	class SceneEquieUpdateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIE_UPDATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid;
		}

		// װ��uid
		ObjID_t equipUid;
	};

	// װ����������
	class SceneEquieUpdateRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIE_UPDATE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & equipUid;
		}

		// ����ֵ
		UInt32 code;
		// װ��uid
		ObjID_t equipUid;
	};

	/**
	*@brief client->scene ��Ʒ�Ĵ�����
	*/
	class SceneItemDepositReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	struct depositItem
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & createTime & itemReward;
		}

		depositItem() : guid(0), createTime(0), dungeonId(0){}
		// guid
		UInt64 guid;
		// ����ʱ��
		UInt32 createTime;
		// ���³�id
		UInt32 dungeonId;
		// ��Ʒ
		ItemReward itemReward;
	};

	/**
	*@brief scene->client ��Ʒ�Ĵ淵��
	*/
	class SceneItemDepositRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & expireTime & depositItemVec;
		}

		// ����ʱ��
		UInt32 expireTime;
		// ��Ʒ�б�
		std::vector<depositItem> depositItemVec;
	};

	/**
	*@brief client->scene ��Ʒ�Ĵ���ȡ����
	*/
	class SceneItemDepositGetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_GET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid;
		}

		// ��Ʒid
		UInt64 guid;
	};

	/**
	*@brief scene->client ��Ʒ�Ĵ���ȡ����
	*/
	class SceneItemDepositGetRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_GET_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief world->scene ��Ʒ�Ĵ���
	*/
	class SceneItemDepositSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId & rewardVec;
		}

		// ��ɫ
		UInt64 roleId;
		// ���³�id
		UInt32 dungeonId;
		// �Ĵ���Ʒ
		ItemRewardVec rewardVec;
	};

	/**
	*@brief scene->client��Ʒ�Ĵ浽��
	*/
	class SceneItemDepositExpire : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_EXPIRE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & oddExpireTime;
		}

		// ʣ�ൽ��ʱ��
		UInt32 oddExpireTime;
	};

	// ��ħ����������
	class SceneMagicCardUpgradeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGIC_CARD_UPGREDE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & upgradeUid & materialUid & equipUid & cardId;
		}

		// ����id
		ObjID_t upgradeUid;
		// ����id
		ObjID_t materialUid;
		// װ��id
		ObjID_t equipUid;
		// װ���ϸ�ħ��id
		UInt32	cardId;
	};

	// ��ħ����������
	class SceneMagicCardUpgradeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGIC_CARD_UPGREDE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & cardTypeId & cardLev & cardGuid & equipUid;
		}
		// ����ֵ
		UInt32 code;
		// ��ħ��typeId
		UInt32 cardTypeId;
		// ��ħ���ȼ�
		UInt8  cardLev;
		// ��ħ��guid
		ObjID_t	cardGuid;
		// װ��guid
		ObjID_t equipUid;
	};

	/**
	*@brief client->server ��ħ��һ���ϳ�
	*/
	class SceneMagicCardCompOneKeyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGICCARD_COMP_ONEKEY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & isCompWhite & isCompBlue & autoUseGold & compNotBind;
		}

		UInt8 isCompWhite;
		UInt8 isCompBlue;
		UInt8 autoUseGold;
		UInt8 compNotBind;
	};

	/**
	*@brief server->client ��ħ��һ���ϳɷ���
	*/
	class SceneMagicCardCompOneKeyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGICCARD_COMP_ONEKEY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & endReason & compTimes & consumeBindGolds & comsumeGolds & items;
		}
		// ����ֵ
		UInt32 code;
		// ����ԭ��
		UInt8 endReason;
		// �ϳɴ���
		UInt32 compTimes;
		// ���İ󶨽��
		UInt32	consumeBindGolds;
		// ���Ľ��
		UInt32  comsumeGolds;
		//�õ��ĵ���
		std::vector<ItemReward> items;
	};

	/**
	*@brief client->server �����ʽ𶳽�
	*/
	class SceneCurrencyFrozenApplyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CURRENCY_FROZEN_APPLY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & itemId & num & reason;
		}

		// ��ɫID
		UInt64 roleId;
		// ��ƷId
		UInt32 itemId;
		// ��Ʒ����
		UInt32 num;
		// ԭ��
		std::string reason;
	};

	/**
	*@brief server->client
	*/
	class SceneCurrencyFrozenApplyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CURRENCY_FROZEN_APPLY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & roleId & retCode;
		}

		// ���ᶩ����
		UInt64 guid;
		// ��ɫID
		UInt64 roleId;
		// ������
		UInt32 retCode;
	};

	/**
	*@brief client->server �ʽ𶳽����
	*/
	class SceneCurrencyFrozenOpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CURRENCY_FROZEN_OP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & opType;
		}

		// ���ᶩ����
		UInt64 guid;
		// 0�����1�۳�
		UInt32 opType;
	};

	/**
	*@brief server->client
	*/
	class SceneCurrencyFrozenOpRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CURRENCY_FROZEN_OP_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & retCode;
		}

		// ���ᶩ����
		UInt64 guid;
		// ������
		UInt32 retCode;
	};

	/**
	*@brief server->client  ����״̬
	*/
	class SceneSyncFlyUpStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_FLY_UP_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status;
		}

		//����״̬
		UInt8 status;
	};

	/**
	*@brief client->server ���Ŀ���
	*/
	class SceneEquipInscriptionOpenReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_OPEN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
	};

	/**
	*@brief server->client
	*/
	class SceneEquipInscriptionOpenRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_OPEN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & retCode;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
		// ������
		UInt32 retCode;
	};

	/**
	*@brief client->server ������Ƕ
	*/
	class SceneEquipInscriptionMountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_MOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionGuid & inscriptionId;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
		// ����GUID
		UInt64 inscriptionGuid;
		// ����ID
		UInt32 inscriptionId;
	};

	/**
	*@brief server->client
	*/
	class SceneEquipInscriptionMountRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_MOUNT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionGuid & inscriptionId & retCode;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
		// ����GUID
		UInt64 inscriptionGuid;
		// ����ID
		UInt32 inscriptionId;
		// ������
		UInt32 retCode;
	};

	/**
	*@brief client->server ����ժȡ
	*/
	class SceneEquipInscriptionExtractReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_EXTRACT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionId;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
		// ����ID
		UInt32 inscriptionId;
	};

	/**
	*@brief server->client
	*/
	class SceneEquipInscriptionExtractRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_EXTRACT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionId & retCode;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
		// ����ID
		UInt32 inscriptionId;
		// ������
		UInt32 retCode;
	};

	/**
*@brief client->server ���ĺϳ�
*/
	class SceneEquipInscriptionSynthesisReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_SYNTHESIS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemIDVec;
		}

		// ����id
		std::vector<UInt32> itemIDVec;
	};

	/**
	*@brief server->client ���ĺϳɷ���
	*/
	class SceneEquipInscriptionSynthesisRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_SYNTHESIS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & items & retCode;
		}
		ItemRewardVec items;
		UInt32 retCode;
	};

	/**
	*@brief client->server ��������
	*/
	class SceneEquipInscriptionDestroyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_DESTROY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionId;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
		// ����ID
		UInt32 inscriptionId;
	};

	/**
	*@brief server->client
	*/
	class SceneEquipInscriptionDestroyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_DESTROY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionId & retCode;
		}

		// װ��GUID
		UInt64 guid;
		// ������
		UInt32 index;
		// ����ID
		UInt32 inscriptionId;
		// ������
		UInt32 retCode;
	};


	/**
	*@brief client->world  ��ȡ�������͵�����Ϣ����
	*/
	class WorldGetItemFriendPresentInfosReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_ITEM_FRIEND_PRESENT_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dataId;
		}

		// ��������id
		UInt32 dataId;
	};

	/**
	*@brief client->world  ��ȡ�������͵�����Ϣ����
	*/
	class WorldGetItemFriendPresentInfosRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_ITEM_FRIEND_PRESENT_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dataId & presentInfos & recvedTotal & recvedTotalLimit;
		}
		
		// ��������id
		UInt32 dataId;
		// ��������
		std::vector<FriendPresentInfo> presentInfos;
		// �������ܴ���
		UInt32	recvedTotal;
		// �������ܴ�������
		UInt32	recvedTotalLimit;
	};

	/**
	*@brief client->world  �������͵�����������
	*/
	class WorldItemFriendPresentReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ITEM_FRIEND_PRESENT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & itemTypeId & friendId;
		}

		// ����id
		UInt64  itemId;
		// ��������id
		UInt32	itemTypeId;
		// ����id
		UInt64	friendId;
	};

	/**
	*@brief world->client  �������͵������ͷ���
	*/
	class WorldItemFriendPresentRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ITEM_FRIEND_PRESENT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemId & itemTypeId & presentInfo;
		}

		// ������
		UInt32 retCode;
		// ����id
		UInt64 itemId;
		// ��������id
		UInt32	itemTypeId;
		// ���µ�������Ϣ
		FriendPresentInfo  presentInfo;
	};

	/**
	*@biref scene->client	ͬ��װ����������
	*/
	class SceneEquipSchemeSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_SCHEME_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & schemes;
		}

		std::vector<EquipSchemeInfo>	schemes;
	};

	/**
	*@brief client->scene   ����װ����������
	*/
	class SceneEquipSchemeWearReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_SCHEME_WEAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & isSync;
		}
		//����id
		UInt8	type;
		//����id
		UInt32	id;
		//�Ƿ�ͬ������
		UInt8	isSync;
	};

	/**
	*@brief scene->client   ����װ����������
	*/
	class SceneEquipSchemeWearRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_SCHEME_WEAR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & type & id & isSync & overdueIds;
		}
		//���
		UInt32		code;
		//����id
		UInt8		type;
		//����id
		UInt32		id;
		//�Ƿ�ͬ������
		UInt8		isSync;
		//���ڵ�װ��
		std::vector<UInt64>	overdueIds;
	};

	/**
	*@brief client->scene  ����ת������
	*/
	class SceneBeadConvertReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BEAD_CONVERT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & beadGuid & materialGuid;
		}

		// ת���ı���
		UInt64 beadGuid;
		// ʹ�õĲ���(û�в���ʱĬ��ʹ�ý��)
		UInt64 materialGuid;
	};

	/**
	*@brief scene->client  ����ת������
	*/
	class SceneBeadConvertRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BEAD_CONVERT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief client->scene  װ��ת������
	*/
	class SceneEquipConvertReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_CONVERT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & sourceEqGuid & targetEqTypeId & convertorGuid;
		}

		// ����
		UInt8	type;
		// ԭװ��id
		UInt64 sourceEqGuid;
		// Ŀ��װ������id
		UInt32 targetEqTypeId;
		//  ת����guid
		UInt64	convertorGuid;
	};

	/**
	*@brief scene->client  װ��ת������
	*/
	class SceneEquipConvertRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_CONVERT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & eqGuid;
		}

		UInt32 retCode;
		UInt64 eqGuid;
	};

	/**
	*@brief client->scene  ֲ������������
	*/
	class SceneActivePlantReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTVE_PLANT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
		
	};

	/**
	*@brief scene->client  ֲ������������
	*/
	class SceneActivePlantRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTVE_PLANT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	/**
	*@brief client->scene  ֲ���ڼ�������
	*/
	class SceneActivePlantAppraReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTVE_PLANT_APPRA_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}

	};

	/**
	*@brief scene->client  ֲ���ڼ�������
	*/
	class SceneActivePlantAppraRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ACTVE_PLANT_APPRA_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode;
		}

		UInt32 retCode;
	};

	// ������ɫ�ֿ�����
	class SceneUnlockRoleStorageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UNLOCK_ROLE_STORAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	// ������ɫ�ֿⷵ��
	class SceneUnlockRoleStorageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UNLOCK_ROLE_STORAGE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & curOpenNum;
		}

		UInt32 retCode;
		UInt32 curOpenNum;
	};


	/**
	*@brief client->world ��ȡϵͳ��¼����
	*/
	class WorldGetSysRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_SYS_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & behavoir & role & param;
		}

		UInt32 behavoir;
		UInt32 role;
		UInt32 param;
	};

	/**
	*@brief world->client ��ȡϵͳ��¼����
	*/
	class WorldGetSysRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_SYS_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & behavoir & role & param & value;
		}

		UInt32 behavoir;
		UInt32 role;
		UInt32 param;
		UInt32 value;
	};

	class SceneWeaponBuildReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEAPON_BUILD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & extraMaterial1 & extraMaterial2;
		}

		UInt32 id;
		UInt8 extraMaterial1;
		UInt8 extraMaterial2;
	};

	class SceneWeaponBuildRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_WEAPON_BUILD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & errorCode & guid;
		}

		UInt32 errorCode;
		UInt64 guid;
	};

	/**
	 *@brief ֪ͨscene����һ�����õ�ȯ
	 */
	class SceneGetCreditPoint : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_GET_CREDIT_POINT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & accId & orderId & creditPoint & onlineId & reason;
		}

		UInt64 roleId;
		UInt32 accId;
		UInt64 orderId;
		UInt32 creditPoint;
		UInt64 onlineId;
		std::string reason;
	};

	/**
	 *@brief ���õ�ȯ��¼
	 */
	class SceneCreditPointRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CREDIT_POINT_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};



	class SceneCreditPointRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CREDIT_POINT_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & recordVec;
		}

		// ���õ�ȯ��ü�¼
		std::vector<CreditPointRecordEntry> recordVec;
	};

	/**
	 *@brief ��������Ӳ�Ҳ�ѯ
	 */
	class SceneSecretCoinReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SECRET_COIN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{

		}
	};



	class SceneSecretCoinRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SECRET_COIN_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & coin;
		}

		UInt32 coin;
	};
}
#endif
