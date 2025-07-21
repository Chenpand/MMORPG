#include "FinalEquipScript.h"
#include "Player.h"
#include "Creature.h"

FinalEquipScript::FinalEquipScript(Avalon::ScriptState* state)
	: Avalon::ScriptFile(state)
{
}

FinalEquipScript::~FinalEquipScript()
{
	for(FinalEquipEffectMap::iterator iter = m_mapFinalEquipEffect.begin(); 
		iter != m_mapFinalEquipEffect.end(); iter++)
	{
		delete iter->second;
	}
	m_mapFinalEquipEffect.clear();
}

void FinalEquipScript::Init()
{
	Call<void>("Init", this);
}

void FinalEquipScript::AddEffect(UInt32 uItemId, FINALEQUIP_EFFECT_TYPE effect, Int32 param1, Int32 param2, Int32 param3)
{
	FinalEquipEffectMap::iterator	iter = m_mapFinalEquipEffect.find(uItemId);
	if(iter != m_mapFinalEquipEffect.end())
		return;

	FinalEquipEffect	*pEffect = new FinalEquipEffect;
	pEffect->effect = effect;
	pEffect->param[0] = param1;
	pEffect->param[1] = param2;
	pEffect->param[2] = param3;

	m_mapFinalEquipEffect[uItemId] = pEffect;
}

bool FinalEquipScript::IsFinalEquip(UInt32 uItemId)
{
	if(m_mapFinalEquipEffect.find(uItemId) != m_mapFinalEquipEffect.end())
		return true;

	return false;
}

void FinalEquipScript::OnAttack(Player *pAtk, Creature *pDfd)
{
// 	ItemPos	pos;
// 	pos.pack = PACK_EQUIP;
// 
// 	for(Int32 type = EQUIP_TYPE_AMULET; type < EQUIP_TYPE_WEAPON_MAX; type++)
// 	{
// 		pos.gridIndex = GetGridByEquipType(type);
// 
// 		Item	*pItem = pAtk->FindItem(pos);
// 		if(NULL == pItem)
// 			continue;
// 		
// 		FinalEquipEffectMap::iterator	iter = m_mapFinalEquipEffect.find(pItem->GetDataID());
// 		if(iter == m_mapFinalEquipEffect.end())
// 			continue;
// 
// 		FinalEquipEffect	*pEffect = iter->second;
// 		if(FEE_ADDBUFF == pEffect->effect)
// 		{
// 			if(Avalon::Random::SelectInTenThousand(pEffect->param[1]))
// 			{
// 				pDfd->AddBuff(UInt16(pEffect->param[0]), UInt32(pEffect->param[2]), pAtk->GetID());
// 			}
// 		}
// 	}
}

void FinalEquipScript::InitEquip(Item *pItem)
{
	FinalEquipEffectMap::iterator	iter = m_mapFinalEquipEffect.find(pItem->GetDataID());
	if(iter == m_mapFinalEquipEffect.end())
		return;

	FinalEquipEffect	*pEffect = iter->second;
	if(FEE_SCRIPTINIT == pEffect->effect)
	{
		Call<void>("InitFinalEquip", pItem);
	}
}

bool FinalEquipScript::GetEquipAttr(Item *pItem)
{
// 	FinalEquipEffectMap::iterator	iter = m_mapFinalEquipEffect.find(pItem->GetDataID());
// 	if(iter == m_mapFinalEquipEffect.end())
// 		return false;
// 
// 	FinalEquipEffect	*pEffect = iter->second;
// 	if(FEE_MODATTR == pEffect->effect)
// 	{
// 		pItem->AddFinalEquipAttr(PkValueID(pEffect->param[0]), ModPkValueType(pEffect->param[1]), Int32(pEffect->param[2]));
// 	}

	return true;
}

UInt32 FinalEquipScript::GetExpAddFactor(Player *pPlayer)
{
	ItemPos	pos;
	pos.pack = PACK_EQUIP;

	UInt32	uExpAddFactor = 0;
// 	for(Int32 type = EQUIP_TYPE_AMULET; type < EQUIP_TYPE_WEAPON_MAX; type++)
// 	{
// 		pos.gridIndex = GetGridByEquipType(type);
// 
// 		Item	*pItem = pPlayer->FindItem(pos);
// 		if(NULL == pItem)
// 			continue;
// 		
// 		FinalEquipEffectMap::iterator	iter = m_mapFinalEquipEffect.find(pItem->GetDataID());
// 		if(iter == m_mapFinalEquipEffect.end())
// 			continue;
// 
// 		FinalEquipEffect	*pEffect = iter->second;
// 		if(FEE_ADDEXP == pEffect->effect)
// 		{
// 			uExpAddFactor += pEffect->param[0];
// 		}
// 	}

	return uExpAddFactor;
}

