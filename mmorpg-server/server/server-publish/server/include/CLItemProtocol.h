#ifndef _CL_ITEM_PROTOCOL_H_
#define _CL_ITEM_PROTOCOL_H_

#include <CLProtocolDefine.h>
#include <CLItemDefine.h>
#include <CLEquipSchemeDefine.h>
#include <CLCreditPointDefine.h>

namespace CLProtocol
{
	/**
	*@brief client->server 道具信息
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
	 *@brief client->server 使用道具
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
		//道具guid
		ObjID_t	itemid;
		//是否一键使用全部
		UInt8 useAll;
		//预留参数1
		UInt32 param1;
		//预留参数2
		UInt32 param2;
	};

	// 使用道具返回
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

	// 出售道具
	class SceneSellItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SELL_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & num;
		}

	public:
		//道具guid
		ObjID_t	itemid;
		UInt16	num;
	};

	// 出售道具返回
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

	// 开格子
	class SceneEnlargePackage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENLARGE_PACKAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	// 开格子返回
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

	// 开仓库格子
	class SceneEnlargeStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ENLARGE_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	//存仓库
	class ScenePushStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PUSH_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & storageType & itemid & num;
		}
	public:
		//仓库类型
		UInt8 storageType;
		//道具guid
		ObjID_t	itemid;
		UInt16 num;
	};

	//存仓库返回
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

	//取仓库
	class ScenePullStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_PULL_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemid & num;
		}
	public:
		//道具guid
		ObjID_t	itemid;
		UInt16 num;
	};

	//取仓库返回
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

	//一键存仓库
	class SceneOneKeyPushStorage : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ONEKEY_PUSH_STORAGE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	//一键存仓库返回
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

	//扩展仓库返回
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

	//更新新道具标识
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
	
	// 装备序列化
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
	 *@brief client->server 移动道具
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
		//道具guid
		ObjID_t	itemid;
		//移动数量
		UInt16	num;
		//目标位置 
		ItemPos	targetpos;
	};

	//移动道具返回
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
	 *@brief server->client 同步道具
	 *		格式：[Guid + itemid + data][Guid + itemid + data][Guid + itemid + data][Guid + itemid + data]...[0]
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
	 *@brief server->client 同步道具属性
	 *		格式: Guid1 + Data1 + Guid2 + Data2 + ... + Guid[0]
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
	 *@brief server->client 通知删除
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
		//唯一guid
		ObjID_t id;
	};

	/**
	 *@brief client->server改造装备
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
		//改造类型(EquipRemakeType)
		UInt8	remakeType;
		//装备
		ObjID_t	equip;
		//主材料
		ObjID_t	mainMaterial;
		//辅助材料
		std::vector<ObjID_t> materials;
		//是否优先绑定
		UInt8	bindFirst;
		//额外参数1
		UInt32	param1;
		//额外参数2
		UInt32	param2;
		//额外参数3
		UInt8	param3;
	};

	/**
	 *@brief server->client 改造装备结果
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
		//改造类型(EquipRemakeType)
		UInt8	remakeType;
		//结果 0表示失败 1表示成功
		UInt8	result;
		//参数1
		UInt32	param1;
		//参数2
		UInt32	param2;
		//参数3
		UInt32	param3;
		//参数列表
		std::vector<ItemReward>	rewards;
	};

	struct TransItemData
	{
		TransItemData() : itemNum(0), isEqualItem(false) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemNum & isEqualItem;
		}

		// 道具数量
		UInt32 itemNum;
		// 是否使用等价物
		UInt32 isEqualItem;
	};

	/**
	 *@brief world->scene 请求检查道具数量
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
		//玩家
		ObjID_t	playerId;
		//请求序列
		UInt16	serial;
		//绑银
		UInt32	bindSilver;
		//银贝
		UInt32	silver;
		//金贝
		UInt32	gold;
		//绑金
		UInt32  bindGold;
		//复活币
		UInt32  reviveCoin;
		//公会贡献
		UInt32	guildContri;
		//道具列表
		std::map<UInt32, TransItemData>	items;
		//非绑道具列表
		std::map<UInt32, UInt32>	nobindItems;
		//次数类型
		std::string	counterName;
		//最大次数
		UInt32	maxCount;
		//需要的次数
		UInt32	needCount;
		//具体的道具
		std::vector<DetailItem>	detailItems;
		//dungeonServer的raceId
		ObjID_t param;
		//是否是周常任务上交道具检查
		UInt8  isWeeklySetTask;
		//请求检查来源服务类型
		UInt8  requestServerType;
	};

	/**
	 *@brief scene->world 检查道具数量返回
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
		//玩家
		ObjID_t	playerId;
		//请求序号
		UInt16	serial;
		// 当前次数（如果查询次数的话）
		UInt32	counterTimes;
		//结果 对应ErrorCode
		UInt32	result;
		//dungeonServer的raceId
		ObjID_t param;
	};

	/**
	 *@brief world->scene 扣除道具
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
		//玩家
		ObjID_t	playerId;
		//原因
		std::string	reason;
		//绑银
		UInt32	bindSilver;
		//银贝
		UInt32	silver;
		//金贝
		UInt32	gold;
		//绑定金贝
		UInt32	bindGold;
		//复活币
		UInt32	reviveCoin;
		//公会贡献
		UInt32	guildContri;
		//道具列表
		std::map<UInt32, TransItemData>	items;
		//非绑道具列表
		std::map<UInt32, UInt32> nobindItems;
		//次数类型
		std::string	counterName;
		//需要的次数
		UInt32	needCount;
		//具体道具
		std::vector<DetailItem> detailItems;
	};

	/**
	 *@brief world->scene 通知场景服务器解锁
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
		//玩家
		ObjID_t	playerId;
	};

	/**
	 *@brief world->scene 通过唯一id移除道具
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
		//玩家
		ObjID_t	playerId;
		//原因
		std::string	reason;
		//道具
		ObjID_t	itemId;
	};

	/**
	 *@brief scene->client 同步被观察者装备
	 *	 	格式：playerId(ObjID_t) + [Guid + itemid + data][Guid + itemid + data][Guid + itemid + data][Guid + itemid + data]...[0]
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
	 *@brief  client->server 合成道具（废弃）
	 */
	class SceneComposeItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_COMPOSE_ITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & quality & num & materialNum & bind;
		}

		//合成id
		UInt32	itemId;
		//合成品质
		UInt8	quality;
		//合成数量
		UInt16	num;
		//材料1数量
		UInt16	materialNum;
		//是否优先绑定  1优先绑定 0只用非绑
		UInt8	bind;
	};

	/**
	 *@brief client->server 整理道具
	 */
	class SceneTrimItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRIM_ITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pack;
		}

		// 包裹类别
		UInt8 pack;
	};

	/**
	*@brief client->server 整理道具
	*/
	class SceneTrimItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_TRIM_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// 返回值
		UInt32 code;
	};

	/**
	 *@brief server->client 显示使用道具特效（废弃）
	 */
	class SceneShowUseItemEffect : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOW_USEITEM_EFFECT)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & targetId & itemId;
		}

		//目标id
		ObjID_t	targetId;
		//道具id
		UInt32	itemId;
	};

	/**
	 *@brief server->client 通知获得道具
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

		//来源类型
		UInt32  sourceType;
		//发放新式
		UInt8  notifyType; //0:旧的, 1:新的
		//得到的道具
		std::vector<ItemReward> items;
	};

	/**
	*@brief client->server 分解装备
	*/
	class SceneEquipDecompose : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_DECOMPOSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{		
			stream & uidList;
		}

		// 装备uid
		std::vector<ObjID_t>	uidList;
	};

	/**
	*@brief client->server 强化装备
	*/
	class SceneEquipStrengthen : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_STRENGTHEN)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & useUnbreak & strTicket;
		}

		// 装备uid
		ObjID_t equipUid;
		// 是否使用保护劵  0为不使用 1为使用 2为使用一次性强化器 3为同时使用保护券和一次性强化器
		UInt8 useUnbreak;
		// 使用的强化券
		ObjID_t strTicket;
	};

	// 强化返回
	class SceneEquipStrengthenRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_STRENGTHEN_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & matNums;
		}

		// 返回值
		UInt32 code;
		// 返还材料
		std::vector<ItemReward> matNums;
	};

	/**
	*@brief client->server 装备增幅
	*/
	class SceneEquipEnhanceUpgrade : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_UPGRADE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & useUnbreak & strTicket;
		}

		// 装备uid
		ObjID_t equipUid;
		// 是否使用保护劵 0为不使用 1为使用 2为使用一次性强化器 3为同时使用保护券和一次性强化器
		UInt8 useUnbreak;
		// 使用的增幅券
		ObjID_t strTicket;
	};

	// 装备增幅返回
	class SceneEquipEnhanceUpgradeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_UPGRADE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & matNums;
		}

		// 返回值
		UInt32 code;
		// 返还材料
		std::vector<ItemReward> matNums;
	};

	//scene->client气息装备清除气息
	class SceneEquipEnhanceClear : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CLEAR)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & stuffID;
		}

		// 装备uid
		ObjID_t equipUid;
		//使用的材料道具id
		UInt32	stuffID;
	};

	// 气息装备清除气息返回
	class SceneEquipEnhanceClearRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CLEAR_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// 返回值
		UInt32 code;
	};

	//scene->client气息装备激活变成红字装备
	class SceneEquipEnhanceRed : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_RED)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & stuffID;
		}

		// 装备uid
		ObjID_t equipUid;
		//使用的材料道具id
		UInt32	stuffID;
	};

	// 气息装备激活变成红字装备返回
	class SceneEquipEnhanceRedRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_RED_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// 返回值
		UInt32 code;
	};

	//scene->client红字装备属性转化
	class SceneEquipEnhanceChg : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CHG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid & enhanceType & stuffID;
		}
		// 装备uid
		ObjID_t equipUid;
		// 转化路线
		UInt8 enhanceType;
		//使用的材料道具id
		UInt32	stuffID;
	};

	// 红字装备属性转化返回
	class SceneEquipEnhanceChgRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_CHG_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// 返回值
		UInt32 code;
	};


	// 增幅材料合成
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

	// 增幅材料合成返回
	class SceneEnhanceMaterialComboRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_ENHANCE_MATERIAL_COMBO_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		// 返回值
		UInt32 code;
	};


	// 分解返回
	class SceneEquipDecomposeRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_DECOMPOSE_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & getItems;
		}

		// 返回值
		UInt32 code;
		// 得到分解物品
		std::vector<ItemReward> getItems;
	};
	

	/**
	*@brief client->server 商店查询
	*/
	class SceneShopQuery : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_QUERY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & cache;
		}

		//商店ID
		UInt8 shopId;
		//是否有缓存
		UInt8 cache;
	};
	
	/**
	*@brief server->client 商店查询返回
	*/
	class SceneShopQueryRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_QUERY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		//是否需要更新
		UInt32 code;
	};

	/**
	*@brief client->server 商店购买
	*/
	class SceneShopBuy : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_BUY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & shopItemId & num & costExtraItems;
		}

		//商店ID
		UInt8 shopId;
		//商品ID
		UInt32 shopItemId;
		//数量
		UInt16 num;
		//额外道具消耗
		std::vector<ItemInfo> costExtraItems;
	};
	
	/**
	*@brief server->client 商店购买返回
	*/
	class SceneShopBuyRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_BUY_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & shopItemId & newNum & leaseEndTimeStamp;
		}

		//返回值
		UInt32 code;
		//商品id
		UInt32 shopItemId;
		//买后的数量
		UInt16 newNum;
		//租赁时间戳
		UInt32 leaseEndTimeStamp;
	};

	/**
	*@brief client->server 商店刷新
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
	*@brief client->server 请求商店剩余刷新次数
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
	*@brief server->client 请求商店剩余刷新次数返回
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
	*@brief client->server 请求时装选择属性
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
	*@brief server->client 请求时装选择属性返回
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
	*@brief client->server 单局使用道具
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
	*@brief server->client 商店刷新返回
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
	*@brief client->server 一键分解装备
	*/
	class SceneOneKeyDecompose : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ONEKEY_DECOMPOSE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & color;
		}

		UInt8 color;		// 颜色，按位取，1<<0是白，1<<1是蓝，以此类推
	};

	/**
	*@brief client->server 一键分解装备返回
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
	*@brief client->server 封装请求
	*/
	class SceneSealEquipReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SEAL_EQUIP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid;
		}

		ObjID_t uid;		// 封装装备uid
	};

	/**
	*@brief server->client 封装请求返回
	*/
	class SceneSealEquipRet: public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SEAL_EQUIP_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & inscriptionIds;
		}

		UInt32 code;		// 返回码
		std::vector<UInt32> inscriptionIds;	//铭文ID数组
	};
	
	/**
	*@brief client->server 检查封装请求
	*/
	class SceneCheckSealEquipReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_SEAL_EQUIP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid;
		}

		ObjID_t uid;		// 封装装备uid
	};
	
	/**
	*@brief server->client 检查封装返回
	*/
	class SceneCheckSealEquipRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHECK_SEAL_EQUIP_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & needMatId & needNum;
		}

		UInt32 code;		// 返回码
		UInt16 needNum;		// 需要材料数量
		UInt32 needMatId;	// 需要材料id
	};

	/**
	*@brief client->server 重新生成装备品级请求
	*/
	class SceneRandEquipQlvReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RAND_EQUIP_QLV_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & uid & autoUsePont & usePerfect;
		}

		ObjID_t uid;			// 装备uid
		UInt8	autoUsePont;	// 是否自动使用点券
		UInt8   usePerfect;     // 是否使用完美洗练卷 
	};


	/**
	*@brief client->server 附魔请求
	*/
	class SceneAddMagicReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADDMAGIC_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & magicUid & itemUid;
		}

		ObjID_t magicUid;		// 附魔卡uid
		ObjID_t itemUid;		// 道具uid
	};

	/**
	*@brief server->client 附魔返回
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
	*@brief server->client 附魔卡合成
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
	*@brief server->client 附魔卡合成返回
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
	*@brief server->client 重新生成装备品级返回
	*/
	class SceneRandEquipQlvRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RAND_EQUIP_QLV_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		UInt32 code;		// 返回码
	};
	
	/**
	*@brief server->client 使用魔法罐请求
	*/
	class SceneUseMagicJarReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_MAGICJAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & combo & opActId & param;
		}

		UInt32	id;		//罐子id
		UInt8	combo;	//是否连抽
		UInt32 opActId;	//运营活动id
		UInt32 param;
	};

	/**
	*@brief client->server 请求罐子积分
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
	*@brief server->client 请求罐子积分返回
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
	*@brief client->world 请求开罐子记录
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
	*@brief world->client 请求开罐子记录返回
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
	*@brief client->world 装备回收查看开罐记录
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
	*@brief world->client 请求开罐子记录返回
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
	*@brief scene->world 请求开罐子记录
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
	*@brief scene->world 获得账号货币请求
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
	*@brief client->server 使用魔法罐请求
	*/
	class SceneQuickUseItemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_QUICK_USEITEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & idx & dungenId;
		}

		UInt8	idx;	//道具列表的index
		UInt32	dungenId;
	};

	/**
	*@brief client->server 使用魔法罐请求
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
	*@brief server->client 使用魔法罐返回
	*/
	class SceneUseMagicJarRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_MAGICJAR_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & jarId & getItems & baseItem & getPointId & getPoint & getPointTime;
		}

		UInt32 code;							//返回码
		UInt32	jarId;
		std::vector<OpenJarResult> getItems;		//抽到的道具
		ItemReward	baseItem;				//基本道具
		UInt32	getPointId;					//获得积分类型
		UInt32	getPoint;						//获得积分数量
		UInt32	getPointTime;					//获得积分暴击倍数
	};
	

	/**
	*@brief client->server 时装合成请求
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
	*@brief server->client 时装合成返回
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
	*@brief client->server 时装合成记录请求
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
	 *@brief client->server 客户端向服务器同步二级货币数（废弃）
	 */
	class SceneSyncCoins : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_COINS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & coins;
		}

		//二级货币数
		UInt32	coins;
	};

	/**
	 *@brief server->client 通知客户端消耗道具
	 */
	class SceneNotifyCostItem : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_COSTITEM)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & quality & num;
		}

		//道具id
		UInt32	itemId;
		//品质
		UInt8	quality;
		//数量
		UInt16	num;
	};

	/**
	 *@brief 合成道具结果（废弃）
	 */
	class SceneComposeItemRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_COMPOSE_ITEM_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

		//结果 1为成功 0为失败
		UInt8	result;
	};

	/**
	 *@brief 显示道具使用对话框（废弃）
	 */
	class SceneShowItemDialog : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOW_ITEM_DIALOG)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & word & options;
		}

		//道具id
		ObjID_t	itemId;
		//提示
		std::string	word;
		//选项列表
		std::vector<ItemDialogOption> options;
	};

	/**
	 *@brief server->client 通知充值成功（废弃）                                                             
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
	 *@brief  client->server 请求兑换道具列表（废弃）                                                                
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
	 *@brief  server->client 返回兑换道具列表（废弃）                                                                  
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
	 *@brief client->server 兑换道具（废弃）                                                                 
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
	*@brief client -> server 道具加锁相关操作(废弃)
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
		//道具加锁操作类型(ItemLockType)
		UInt8 itemLockType;

		//原密码
		std::string passwd;

		//第一次新密码
		std::string firstPasswd;

		//第二次新密码
		std::string secondPasswd;

		//锁定范围
		UInt32 lockRange;

		//提示问题
		std::string question;

		//答案
		std::string answer;
	};

	/**
	*@brief server -> client 道具加锁相关操作结果（废弃）
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
		//道具加锁操作类型(ItemLockType)
		UInt8 itemLockType;

		//结果(0:失败 1:成功)
		UInt8 result;
	};

	/**
	*@brief server -> client 道具加锁按钮状态（废弃）
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
		//道具加锁按钮状态(ItemLockButtonType)
		UInt8 buttonState;
	};

	/**
	*@brief server -> client 道具加锁通知客户端输入密码解锁（废弃）
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
	*@brief server -> client 同步加锁范围（废弃）
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
		//道具加锁范围
		UInt32 lockRange;
	};

	/**
	*@brief server -> client 同步提示问题（废弃）
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
		//提示问题
		std::string question;
	};
	
	/**
	*@brief 随机装备商店数据项
	*/
	struct RandEquipEntry
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & price & quality;
		}
		// 装备id
		UInt32	id;
		// 价格
		UInt32	price;
		// 品质
		UInt8	quality;
	};
	/**
	*@brief server->client 同步随机装备商店（废弃）
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
		// 下次刷新时间
		UInt32	uNextRefreshTime;
		// 普通商店列表
		std::vector<RandEquipEntry>	vecNormal;
		// vip商店列表
		std::vector<RandEquipEntry>	vecVip;
	};

	/**
	*@brief server->client 同步随机装备商店 add by jermaine
	*/
	struct ProtoShopItem
	{
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopItemId & grid & restNum & vipLv & vipDiscount & discountRate & leaseEndTimeStamp;
		}
		//商店道具id
		UInt32	shopItemId;
		//格子
		UInt8	grid;
		//剩余数量
		UInt16	restNum;
		//vip等级
		UInt8	vipLv;
		//vip折扣
		UInt8	vipDiscount;
		//折扣率
		UInt32  discountRate;
		//租赁结束时间戳
		UInt32	leaseEndTimeStamp;
	};

	// 同步商店
	class SceneShopSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOP_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & shopId & refreshCost & shopItemList & restRefreshTime & restRefreshNum & maxRefreshNum & restWeekRefreshTime & restMonthRefreshTime;
		}
	public:
		//商店id
		UInt32 shopId;
		// 刷新花费
		UInt16 refreshCost;
		// 普通商店列表
		std::vector<ProtoShopItem>	shopItemList;
		// 每天刷新剩余时间
		UInt32 restRefreshTime;
		// 剩余刷新次数
		UInt8 restRefreshNum;
		// 最多刷新次数
		UInt8 maxRefreshNum;
		// 每周刷新剩余时间
		UInt32 restWeekRefreshTime;
		// 每月刷新剩余时间
		UInt32 restMonthRefreshTime;
	};

	// 同步商品
	class SceneShopItemSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SHOPITEM_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			
		}
	};
	
	

	/**
	*@brief client->server 购买随机装备（废弃）
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
		// 商店类型，0 普通商店，1 vip商店
		UInt8	type;
		// 装备索引，0 开始
		UInt8	index;
	};

	/**
	*@brief server->client 购买后通知客户端删除一个随机装备（废弃）
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
		// 商店类型，0 普通商店，1 vip商店
		UInt8	type;
		// 装备索引，0 开始
		UInt8	index;
	};

	/**
	 *@brief world->scene 通知场景服加货币道具                                                                     
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
	*@brief scene->world 扩展背包格子
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
	*@brief world->scene 扩展背包格子返回
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
	*@brief client->scene 设置地下城药水请求
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
	*@brief scene->client 设置地下城药水请求返回
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
	*@brief client->scene 道具续费请求
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
		UInt64	itemUid;				//道具uid
		UInt32	durationDay;			//续费时长(day),0是永久
	};
	
	/**
	*@brief client->scene 道具续费请求返回
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
	*@brief scene->client 通知客户端续费道具状态变化
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
	*@brief client->scene 请求开启魔盒
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
	*@brief scene->client 请求开启魔盒返回
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
	*@brief client->scene 请求添加宝珠
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
		// 宝珠uid
		UInt64 preciousBeadUid;
		// 道具uid
		UInt64 itemUid;
	};

	/**
	*@brief scene->client 添加宝珠返回
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
		// 错误码
		UInt32 code;
		// 宝珠id
		UInt32 preciousBeadId;
		// 道具uid
		UInt64 itemUid;
	};

	/**
	*@brief client->scene 批量出售道具请求
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
		// 批量道具uid
		std::vector<UInt64> itemUids;
	};
	
	/**
	*@brief scene->client 批量出售道具返回
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
	*@brief client->scene 抽奖
	*/
	class SceneDrawPrizeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DRAW_PRIZE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id;
		}

		// 抽奖id
		UInt32 id;
	};

	/**
	*@brief scene->client 抽奖返回
	*/
	class SceneDrawPrizeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_DRAW_PRIZE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & drawPrizeId & rewardId & retCode;
		}

		// 抽奖id
		UInt32 drawPrizeId;
		// 奖品id
		UInt32 rewardId;
		// 错误码
		UInt32 retCode;
	};

	/*
	*@brief client->scene 装备回收提交请求
	*/
	class SceneEquipRecSubcmtReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_SUBCMT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemUids;
		}

		// 批量道具uid
		std::vector<UInt64> itemUids;
	};

	/*
	*@brief scene->client 装备回收提交返回
	*/
	class SceneEquipRecSubcmtRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_SUBCMT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & scoreItems & score;
		}

		//错误码
		UInt32 code;
		//装备回收积分
		std::vector<EqRecScoreItem>	scoreItems;
		//当前总积分
		UInt32 score;
	};

	/*
	*@brief client->scene 装备赎回请求
	*/
	class SceneEquipRecRedeemReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_REDEEM_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equid;
		}

		//装备uid
		UInt64 equid;
	};

	/*
	*@brief svrver->client 装备赎回返回
	*/
	class SceneEquipRecRedeemRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_REDEEM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & consScore;
		}

		//错误码
		UInt32 code;
		//扣除积分
		UInt32 consScore;
	};

	/*
	*@brief client->server 装备回收积分提升请求
	*/
	class SceneEquipRecUpscoreReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_UPSCORE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equid;
		}

		//装备uid
		UInt64 equid;
	};

	/*
	*@brief server->client 装备回收积分提升返回
	*/
	class SceneEquipRecUpscoreRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_UPSCORE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & upscore;
		}

		//错误码
		UInt32 code;
		//提升积分值
		UInt32 upscore;
	};

	/*
	*@brief client->server 装备赎回刷新时间戳请求
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
	*@brief server->client 装备赎回刷新时间戳返回
	*/
	class SceneEquipRecRedeemTmRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_REC_REDEMTM_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & timestmap;
		}

		//刷新时间戳
		UInt64 timestmap;
	};


	/*
	*@brief server->client 通知装备回收重置
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
	*@brief client->server 装备转移请求
	*/
	class SceneEquipTransferReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_TRANSFER_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equid & transferId;
		}

		//装备uid
		UInt64	equid;
		//转移石道具对象id
		UInt64	transferId;
	};

	/*
	*@brief client->server 装备转移返回
	*/
	class SceneEquipTransferRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_TRANSFER_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code;
		}

		//错误码
		UInt32 code;
	};

	/*
	*@brief scene->client 同步神秘商人
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
	*@brief client->scene 上传抗魔值
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
	*@brief scene->world 同步抗魔值
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
	*@brief world->scene 请求增加道具
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
	*@brief scene->world 请求增加道具返回
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
	*@brief client->scene 道具加锁解锁请求
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
		// 操作类型，0解锁，否则加锁
		UInt32 opType;
		// 道具uid
		UInt64 itemUid;
	};

	/**
	*@brief scene->client 道具加锁解锁返回
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
		// 道具uid
		UInt64 itemUid;
		// 返回值
		UInt32 ret;
	};

	/**
	*@brief client->scene 设置时装武器显示请求
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
		// 是否实现时装武器 0：不显示 1:显示
		UInt8 isShow;
	};

	/**
	*@brief scene->client 设置时装武器显示返回
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
		// 返回值
		UInt32 ret;
	};

	/**
	*@brief client->scene 换装节活动时装合成请求
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
		UInt64 fashionId; //被合时装对象id
		UInt64 ticketId;  //换装卷对象id（选择背包中数量最大的换装卷）
		UInt32 choiceComFashionId; //选择必定合成时装道具id
	};

	/**
	*@brief scene->client 换装节活动时装合成返回
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
		UInt32 ret;		  //结果
		UInt8 type;       //[1]:未合出进击的勇士时装 [2]合出进击的勇士时装
		UInt32 commonId;  //合出普通时装id
		UInt32 advanceId; //合出的进击的勇士时装id
		UInt8 allMerged;  //套装全部合出
	};

	/**
	*@brief client->scene 强化券合成请求
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
		// 合成方案
		UInt32 synthesisPlan;
	};

	/**
	*@brief scene->client 强化券合成返回
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
		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief client->scene 强化券融合请求
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
		// 选择要融合的强化券
		ObjID_t pickTicketA;
		ObjID_t pickTicketB;
	};

	/**
	*@brief scene->client 强化券融合返回
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
		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief client->scene 宝珠镶嵌请求
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
		// 宝珠uid
		UInt64 preciousBeadUid;
		// 道具uid
		UInt64 itemUid;
		// 孔索引
		UInt8  holeIndex;
	};

	/**
	*@brief scene->client 宝珠镶嵌返回
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
		// 错误码
		UInt32 code;
		// 宝珠id
		UInt32 preciousBeadId;
		// 道具uid
		UInt64 itemUid;
		// 孔索引
		UInt8  holeIndex;
	};

	/**
	*@brief client->scene 宝珠摘除请求
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
		//道具uid
		UInt64 itemUid;
		//孔索引
		UInt8 holeIndex;
		//杵id
		UInt32 pestleId;
	};

	/**
	*@brief server->client 宝珠摘除返回
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
		// 错误码
		UInt32 code;
	};

	/**
	*@brief client->scene 升级宝珠请求
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
		//类型 [0]:未镶嵌 [1]:已镶嵌
		UInt8		mountedType;
		//宝珠guid 类型0时设置
		ObjID_t		precGuid;
		//装备guid 类型1时设置
		ObjID_t		equipGuid;
		//装备宝珠孔索引 类型1时设置
		UInt8		eqPrecHoleIndex;
		//宝珠id 类型1时设置
		UInt32		precId;
		//选择消耗宝珠id 
		UInt32		consumePrecId;
	};

	/**
	*@brief scene->client 升级宝珠返回
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
		// 返回码
		UInt32	retCode;
		// 类型 [0]:未镶嵌 [1]:已镶嵌
		UInt8 mountedType;
		//装备guid 类型1时设置
		ObjID_t	equipGuid;
		// 升级成功后宝珠id
		UInt32	precId;
		// 附加buff id
		UInt32  addBuffId;
		// 升级后新的宝珠id 类型[0]
		ObjID_t newPrebeadUid;
	};

	/**
	*@brief world->scene 增加经验请求
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
		//玩家
		ObjID_t	playerId;
		//原因
		std::string	reason;
		//增加的经验
		UInt64 incExp;
	};

	/**
	*@brief client->scene 宝珠替换请求
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
		//道具uid
		UInt64 itemUid;
		//新宝珠Id
		UInt64 preciousBeadUid;
		//孔索引
		UInt8 holeIndex;
	};

	/**
	*@brief server->client 宝珠替换返回
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
		// 错误码
		UInt32 retCode;
		//新宝珠Id
		UInt32 preciousBeadId;
		//道具uid
		UInt64 itemUid;
		//孔索引
		UInt8 holeIndex;
	};

	/**
	*@brief world->scene  检查道具请求
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
		//请求序列id
		UInt32	serialId;
		//类型
		UInt8	type;
		//道具拥有者玩家id
		ObjID_t owerId;
		//道具id
		ObjID_t itemId;
		//附加参数
		ObjID_t param;
	};

	/**
	*@brief client->scene 活动神器罐购买次数请求
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

		// 罐子id
		UInt32 jarId;
		// 购买次数
		UInt32 buyCnt;
		// 总次数
		UInt32 totalCnt;
	};

	/**
	*@brief scene->client 活动神器罐购买次数返回
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
	*@brief scene->globalActivity 神器罐购买抽奖资格上报
	*/
	class GASArtifactJarLotterySignReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_SIGN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarIdVec;
		}

		// 抽奖资格的罐子ID
		std::vector<UInt32> jarIdVec;
	};

	/**
	*@brief client->globalActivity 神器罐活动抽奖记录请求
	*/
	class GASArtifactJarLotteryRecordReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_RECORD_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId;
		}
	
		// 罐子id
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

		// 服务器名字
		std::string serverName;
		// 玩家名字
		std::string playerName;
		// 记录时间
		UInt64 recordTime;
		// 道具id
		UInt32 itemId;
	};

	/**
	*@brief globalActivity->client 神器罐活动抽奖记录返回
	*/
	class GASArtifactJarLotteryRecordRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_RECORD_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId & lotteryRecordVec;
		}
		
		// 罐子id
		UInt32 jarId;
		// 记录列表
		std::vector<ArtifactJarLotteryRecord> lotteryRecordVec;
	};

	/**
	*@brief globalActivity->client 神器罐抽奖通知
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
	*@brief client->globalActivity 神器罐抽奖配置请求
	*/
	class GASArtifactJarLotteryCfgReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(GAS_ARTIFACT_JAR_LOTTERY_CFG_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};
	
	// 神器罐子奖励配置
	struct ArtifactJarLotteryCfg
	{
		ArtifactJarLotteryCfg() : jarId(0) {}

		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & jarId & rewardVec;
		}

		// 罐子id
		UInt32 jarId;
		// 奖励列表
		std::vector<ItemReward>	rewardVec;
	};

	/**
	*@brief globalActivity->client 神器罐活动抽奖记录返回
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
	*@brief client->server 装备升级
	*/
	class SceneEquieUpdateReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIE_UPDATE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & equipUid;
		}

		// 装备uid
		ObjID_t equipUid;
	};

	// 装备升级返回
	class SceneEquieUpdateRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIE_UPDATE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & equipUid;
		}

		// 返回值
		UInt32 code;
		// 装备uid
		ObjID_t equipUid;
	};

	/**
	*@brief client->scene 物品寄存请求
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
		// 创建时间
		UInt32 createTime;
		// 地下城id
		UInt32 dungeonId;
		// 物品
		ItemReward itemReward;
	};

	/**
	*@brief scene->client 物品寄存返回
	*/
	class SceneItemDepositRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & expireTime & depositItemVec;
		}

		// 过期时间
		UInt32 expireTime;
		// 物品列表
		std::vector<depositItem> depositItemVec;
	};

	/**
	*@brief client->scene 物品寄存领取请求
	*/
	class SceneItemDepositGetReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_GET_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid;
		}

		// 物品id
		UInt64 guid;
	};

	/**
	*@brief scene->client 物品寄存领取返回
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
	*@brief world->scene 物品寄存获得
	*/
	class SceneItemDepositSync : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_SYNC)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & dungeonId & rewardVec;
		}

		// 角色
		UInt64 roleId;
		// 地下城id
		UInt32 dungeonId;
		// 寄存物品
		ItemRewardVec rewardVec;
	};

	/**
	*@brief scene->client物品寄存到期
	*/
	class SceneItemDepositExpire : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ITEM_DEPOSIT_EXPIRE)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & oddExpireTime;
		}

		// 剩余到期时间
		UInt32 oddExpireTime;
	};

	// 附魔卡升级请求
	class SceneMagicCardUpgradeReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGIC_CARD_UPGREDE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & upgradeUid & materialUid & equipUid & cardId;
		}

		// 主卡id
		ObjID_t upgradeUid;
		// 副卡id
		ObjID_t materialUid;
		// 装备id
		ObjID_t equipUid;
		// 装备上附魔卡id
		UInt32	cardId;
	};

	// 附魔卡升级返回
	class SceneMagicCardUpgradeRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGIC_CARD_UPGREDE_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & cardTypeId & cardLev & cardGuid & equipUid;
		}
		// 返回值
		UInt32 code;
		// 附魔卡typeId
		UInt32 cardTypeId;
		// 附魔卡等级
		UInt8  cardLev;
		// 附魔卡guid
		ObjID_t	cardGuid;
		// 装备guid
		ObjID_t equipUid;
	};

	/**
	*@brief client->server 附魔卡一键合成
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
	*@brief server->client 附魔卡一键合成返回
	*/
	class SceneMagicCardCompOneKeyRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_MAGICCARD_COMP_ONEKEY_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & endReason & compTimes & consumeBindGolds & comsumeGolds & items;
		}
		// 返回值
		UInt32 code;
		// 结束原因
		UInt8 endReason;
		// 合成次数
		UInt32 compTimes;
		// 消耗绑定金币
		UInt32	consumeBindGolds;
		// 消耗金币
		UInt32  comsumeGolds;
		//得到的道具
		std::vector<ItemReward> items;
	};

	/**
	*@brief client->server 请求资金冻结
	*/
	class SceneCurrencyFrozenApplyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CURRENCY_FROZEN_APPLY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & roleId & itemId & num & reason;
		}

		// 角色ID
		UInt64 roleId;
		// 物品Id
		UInt32 itemId;
		// 物品数量
		UInt32 num;
		// 原因
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

		// 冻结订单号
		UInt64 guid;
		// 角色ID
		UInt64 roleId;
		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief client->server 资金冻结操作
	*/
	class SceneCurrencyFrozenOpReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CURRENCY_FROZEN_OP_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & opType;
		}

		// 冻结订单号
		UInt64 guid;
		// 0解除，1扣除
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

		// 冻结订单号
		UInt64 guid;
		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief server->client  飞升状态
	*/
	class SceneSyncFlyUpStatus : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SYNC_FLY_UP_STATUS)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & status;
		}

		//飞升状态
		UInt8 status;
	};

	/**
	*@brief client->server 铭文开孔
	*/
	class SceneEquipInscriptionOpenReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_OPEN_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index;
		}

		// 装备GUID
		UInt64 guid;
		// 孔索引
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

		// 装备GUID
		UInt64 guid;
		// 孔索引
		UInt32 index;
		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief client->server 铭文镶嵌
	*/
	class SceneEquipInscriptionMountReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_MOUNT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionGuid & inscriptionId;
		}

		// 装备GUID
		UInt64 guid;
		// 孔索引
		UInt32 index;
		// 铭文GUID
		UInt64 inscriptionGuid;
		// 铭文ID
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

		// 装备GUID
		UInt64 guid;
		// 孔索引
		UInt32 index;
		// 铭文GUID
		UInt64 inscriptionGuid;
		// 铭文ID
		UInt32 inscriptionId;
		// 返回码
		UInt32 retCode;
	};

	/**
	*@brief client->server 铭文摘取
	*/
	class SceneEquipInscriptionExtractReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_EXTRACT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionId;
		}

		// 装备GUID
		UInt64 guid;
		// 孔索引
		UInt32 index;
		// 铭文ID
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

		// 装备GUID
		UInt64 guid;
		// 孔索引
		UInt32 index;
		// 铭文ID
		UInt32 inscriptionId;
		// 返回码
		UInt32 retCode;
	};

	/**
*@brief client->server 铭文合成
*/
	class SceneEquipInscriptionSynthesisReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_SYNTHESIS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemIDVec;
		}

		// 材料id
		std::vector<UInt32> itemIDVec;
	};

	/**
	*@brief server->client 铭文合成返回
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
	*@brief client->server 铭文碎裂
	*/
	class SceneEquipInscriptionDestroyReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_INSCRIPTION_DESTROY_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & guid & index & inscriptionId;
		}

		// 装备GUID
		UInt64 guid;
		// 孔索引
		UInt32 index;
		// 铭文ID
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

		// 装备GUID
		UInt64 guid;
		// 孔索引
		UInt32 index;
		// 铭文ID
		UInt32 inscriptionId;
		// 返回码
		UInt32 retCode;
	};


	/**
	*@brief client->world  拉取好友赠送道具信息请求
	*/
	class WorldGetItemFriendPresentInfosReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_ITEM_FRIEND_PRESENT_INFO_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dataId;
		}

		// 道具类型id
		UInt32 dataId;
	};

	/**
	*@brief client->world  拉取好友赠送道具信息返回
	*/
	class WorldGetItemFriendPresentInfosRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_GET_ITEM_FRIEND_PRESENT_INFO_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & dataId & presentInfos & recvedTotal & recvedTotalLimit;
		}
		
		// 道具类型id
		UInt32 dataId;
		// 赠送数据
		std::vector<FriendPresentInfo> presentInfos;
		// 被赠送总次数
		UInt32	recvedTotal;
		// 被赠送总次数上限
		UInt32	recvedTotalLimit;
	};

	/**
	*@brief client->world  好友赠送道具赠送请求
	*/
	class WorldItemFriendPresentReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ITEM_FRIEND_PRESENT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & itemId & itemTypeId & friendId;
		}

		// 道具id
		UInt64  itemId;
		// 道具类型id
		UInt32	itemTypeId;
		// 好友id
		UInt64	friendId;
	};

	/**
	*@brief world->client  好友赠送道具赠送返回
	*/
	class WorldItemFriendPresentRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(WORLD_ITEM_FRIEND_PRESENT_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & retCode & itemId & itemTypeId & presentInfo;
		}

		// 返回码
		UInt32 retCode;
		// 道具id
		UInt64 itemId;
		// 道具类型id
		UInt32	itemTypeId;
		// 更新的赠送信息
		FriendPresentInfo  presentInfo;
	};

	/**
	*@biref scene->client	同步装备方案数据
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
	*@brief client->scene   穿戴装备方案请求
	*/
	class SceneEquipSchemeWearReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_SCHEME_WEAR_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & id & isSync;
		}
		//类型id
		UInt8	type;
		//方案id
		UInt32	id;
		//是否同步方案
		UInt8	isSync;
	};

	/**
	*@brief scene->client   穿戴装备方案返回
	*/
	class SceneEquipSchemeWearRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_SCHEME_WEAR_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & code & type & id & isSync & overdueIds;
		}
		//结果
		UInt32		code;
		//类型id
		UInt8		type;
		//方案id
		UInt32		id;
		//是否同步方案
		UInt8		isSync;
		//到期的装备
		std::vector<UInt64>	overdueIds;
	};

	/**
	*@brief client->scene  宝珠转换请求
	*/
	class SceneBeadConvertReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BEAD_CONVERT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & beadGuid & materialGuid;
		}

		// 转换的宝珠
		UInt64 beadGuid;
		// 使用的材料(没有材料时默认使用金币)
		UInt64 materialGuid;
	};

	/**
	*@brief scene->client  宝珠转换返回
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
	*@brief client->scene  装备转换请求
	*/
	class SceneEquipConvertReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_EQUIP_CONVERT_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & sourceEqGuid & targetEqTypeId & convertorGuid;
		}

		// 类型
		UInt8	type;
		// 原装备id
		UInt64 sourceEqGuid;
		// 目标装备类型id
		UInt32 targetEqTypeId;
		//  转换器guid
		UInt64	convertorGuid;
	};

	/**
	*@brief scene->client  装备转换返回
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
	*@brief client->scene  植树节种树请求
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
	*@brief scene->client  植树节种树返回
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
	*@brief client->scene  植树节鉴定请求
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
	*@brief scene->client  植树节鉴定返回
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

	// 解锁角色仓库请求
	class SceneUnlockRoleStorageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_UNLOCK_ROLE_STORAGE_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
		}
	};

	// 解锁角色仓库返回
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
	*@brief client->world 拉取系统记录请求
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
	*@brief world->client 拉取系统记录返回
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
	 *@brief 通知scene，玩家获得信用点券
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
	 *@brief 信用点券记录
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

		// 信用点券获得记录
		std::vector<CreditPointRecordEntry> recordVec;
	};

	/**
	 *@brief 神秘卖场硬币查询
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
