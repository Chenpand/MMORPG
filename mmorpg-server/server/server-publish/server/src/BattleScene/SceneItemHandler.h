#ifndef _SCENE_ITEM_HANDLER_H_
#define _SCENE_ITEM_HANDLER_H_

#include <CLItemProtocol.h>
#include <CLMallProtocol.h>

class Player;

void HandleProtocol(Player* player, CLProtocol::SceneMoveItem& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneUseItem& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneSellItem& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEnlargePackage& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEnlargeStorage& protocol);
void HandleProtocol(Player* player, CLProtocol::ScenePushStorage& protocol);
void HandleProtocol(Player* player, CLProtocol::ScenePullStorage& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneOneKeyPushStorage& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneTrimItem& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipDecompose& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipStrengthen& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneShopQuery& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneShopBuy& protocol);
//void HandleProtocol(Player* player, CLProtocol::SceneEnlargeStorage& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneUpdateNewItem& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneShopRefresh& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneDungeonUseItem& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneOneKeyDecompose& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneSealEquipReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneCheckSealEquipReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneRandEquipQlvReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneAddMagicReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneMagicCardCompReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneUseMagicJarReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneQuickUseItemReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneFashionComposeReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneFashionMergeRecordReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipMakeReq& protocol);


void HandleProtocol(Player* player, CLProtocol::SceneRemakeItem& protocol);

void HandleProtocol(CLProtocol::SceneCheckItemNumReq& protocol);

void HandleProtocol(CLProtocol::SceneRemoveItemReq& protocol);

void HandleProtocol(CLProtocol::SceneUnlockItemReq& protocol);

void HandleProtocol(CLProtocol::SceneRemoveItemObjReq& protocol);

void HandleProtocol(CLProtocol::WorldMallCheckBuy &protocol);

void HandleProtocol(CLProtocol::SceneAddMoneyItemReq& protocol);
void HandleProtocol(CLProtocol::WSRefreshLimiteBuyNum &req);
void HandleProtocol(CLProtocol::WolrdLoadConditionActivateDBNotify& ret);

void HandleProtocol(Player* player, CLProtocol::SceneShopRefreshNumReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneFashionAttrSelReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneComposeItemReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneAddPreciousBeadReq& protocol);


void HandleProtocol(Player* player, CLProtocol::SceneExchangeItemsReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneExchangeItem& protocol);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneItemLockReq& protocol);

void HandleProtocol(Player* pPlayer, CLProtocol::SceneRandEquipShopBuy& protocol);
void HandleProtocol(CLProtocol::WSEnlargeStorageRet& ret);
void HandleProtocol(Player* player, CLProtocol::SceneJarPointReq& protocol);
void HandleProtocol(Player* player, CLProtocol::CSSetDungeonPotionReq& req);
void HandleProtocol(Player* player, CLProtocol::CSRenewTimeItemReq& req);
void HandleProtocol(Player* player, CLProtocol::CSOpenMagicBoxReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneSellItemBatReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneDrawPrizeReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecSubcmtReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecRedeemReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecUpscoreReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneEquipRecRedeemTmReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneEquipTransferReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneSyncResistMagicReq& req);

void HandleProtocol(CLProtocol::SceneAddRewardReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneItemOpLockReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneSetFashionWeaponShowReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneFashionChangeActiveMergeReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneStrengthenTicketSynthesisReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneStrengthenTicketFuseReq& req);

void HandleProtocol(Player* player, CLProtocol::SceneMountPreciousBeadReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneExtirpePreciousBeadReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneUpgradePreciousbeadReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneReplacePreciousBeadReq& req);

void HandleProtocol(CLProtocol::WorldIncreasePlayerExpReq& protocol);

void HandleProtocol(CLProtocol::SceneCheckItemReq& protcol);

void HandleProtocol(Player* player, CLProtocol::SceneArtifactJarBuyCntReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneEquieUpdateReq& protocol);

void HandleProtocol(CLProtocol::WorldCheckRechargePushReq& protocol);
void HandleProtocol(CLProtocol::WorldSceneBuyRechargePushItemReq& protocol);

#endif
