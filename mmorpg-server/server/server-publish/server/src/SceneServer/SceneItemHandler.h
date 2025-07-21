#ifndef _SCENE_ITEM_HANDLER_H_
#define _SCENE_ITEM_HANDLER_H_

#include <CLItemProtocol.h>
#include <CLMallProtocol.h>
#include <CLShopProtocol.h>

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
void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceUpgrade& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceClear& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceRed& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipEnhanceChg& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEnhanceMaterialCombo& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneShopQuery& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneShopBuy& protocol);
void HandleProtocol(CLProtocol::WorldAccountShopItemBuyedNotify& protocol);

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
void HandleProtocol(CLProtocol::SceneGetCreditPoint& req);
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
void HandleProtocol(Player* player, CLProtocol::SceneBeadConvertReq& protocol);

void HandleProtocol(CLProtocol::WorldIncreasePlayerExpReq& protocol);

void HandleProtocol(CLProtocol::SceneCheckItemReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneArtifactJarBuyCntReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneEquieUpdateReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneItemDepositReq& req);
void HandleProtocol(Player* player, CLProtocol::SceneItemDepositGetReq& req);
void HandleProtocol(CLProtocol::SceneItemDepositSync& protocol);

void HandleProtocol(CLProtocol::WorldCheckRechargePushReq& protocol);
void HandleProtocol(CLProtocol::WorldSceneBuyRechargePushItemReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneMagicCardUpgradeReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneMagicCardCompOneKeyReq& protocol);

void HandleProtocol(CLProtocol::SceneCurrencyFrozenApplyReq& protocol);
void HandleProtocol(CLProtocol::SceneCurrencyFrozenOpReq& protocol);


void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionOpenReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionMountReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionExtractReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionSynthesisReq& protocol);
void HandleProtocol(Player* player, CLProtocol::SceneEquipInscriptionDestroyReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneMallFashionLimitedSuitStatusReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneEquipSchemeWearReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneEquipConvertReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneActivePlantReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneActivePlantAppraReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneUnlockRoleStorageReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneWeaponBuildReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneCreditPointRecordReq& protocol);

void HandleProtocol(Player* player, CLProtocol::SceneSecretCoinReq& protocol);


#endif
