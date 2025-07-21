#include "Vip.h"
#include "Player.h"
#include <bitset>
#include <CLSystemValueDataEntry.h>

Vip::Vip()
{
	m_pDataEntry = NULL;
}

Vip::~Vip()
{
}

void Vip::Init(UInt8 level, UInt32 exp)
{
	m_Level = level;
	m_Exp = exp;
	m_pDataEntry = VipDataEntryMgr::Instance()->FindEntry(m_Level);
}

void Vip::AddExp(const char* reason, UInt32 exp)
{
	if (m_pDataEntry == NULL) return;
	//if (exp == 0) return;
	//if (m_pDataEntry->exp == 0) return;
	UInt32 oldExp = m_Exp;
	UInt8 oldLevel = m_Level;
	m_Exp = m_Exp + exp;

	LevelUp();

	if (m_Exp != oldExp || m_Level != oldLevel)
	{
		SetDirty();
		SaveToDB();

		DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") " << reason 
		<<" VIP AddExp:" << exp << "||" << oldExp << "==>" << m_Exp << " || " << oldLevel << "==>" << m_Level << " succeed!" << LogStream::eos;

		GetOwner()->OnVipExpChanged();
		GetOwner()->SendAssetUdpLog(AssetType::ASSET_VIP_EXP, reason, (UInt64)oldExp, (Int64)exp, (Int64)exp, (UInt64)m_Exp);

		if (m_Level != oldLevel)
		{
			GetOwner()->OnVipExpChanged();
			GetOwner()->SendAssetUdpLog(AssetType::ASSET_VIP_LEVEL, reason, (UInt64)oldLevel, (Int64)m_Level - (Int64)oldLevel, (Int64)m_Level - (Int64)oldLevel, (UInt64)m_Level);
		}
	}
}


void Vip::LevelUp()
{
	if (m_pDataEntry == NULL) return;

	UInt32 maxLevel = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_VIPLEVEL_MAX);
	if (m_Level >= maxLevel) return;

	while (m_Exp >= m_pDataEntry->exp && m_pDataEntry->exp > 0)
	{
		VipDataEntry* vipDataEntry = VipDataEntryMgr::Instance()->FindEntry(m_Level + 1);
		if (vipDataEntry == NULL) break;

		/*
		if (vipDataEntry->exp == 0)
		{
			m_Exp = m_pDataEntry->exp;
		}
		else
		{
			m_Exp = m_Exp - m_pDataEntry->exp;
		}
		*/

		m_Exp = m_Exp - m_pDataEntry->exp;

		m_Level = m_Level + 1;
		OnLevelUp();
		m_pDataEntry = vipDataEntry;
	}
}

void Vip::OnLevelUp()
{
	/*
	UInt32 newValue = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_PRIVILEGE_STORAGE_MAX, dataEntry->level);
	UInt32 nowValue = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(VIP_PRIVILEGE_STORAGE_MAX, m_pDataEntry->level);
	if (newValue > nowValue)
	{
		UInt32 result = GetOwner()->EnlargeStorage(newValue-nowValue);
		if (result != ErrorCode::SUCCESS)
		{
			DebugLogStream << "player(" << GetOwner()->GetAccID() << "," << GetOwner()->GetID() << "," << GetOwner()->GetName() << ") "
				<< " On Level Up: enlarge storage error" << result << LogStream::eos;
		}
	}
	*/

	m_Owner->OnVipLvlChanged();
}

void Vip::GetValue(VipPrivilegeType type, UInt32& value)
{
	value = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(type, m_Level);
}

void Vip::GetValue(VipPrivilegeType type, bool& value)
{
	UInt32 tmpValue = VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValue(type, m_Level);
	value = tmpValue == 0 ? false : true;
}

UInt32 Vip::GetValueById(UInt32 id)
{
	return VipPrivilegeDataEntryMgr::Instance()->FindPrivilegeValueById(id, m_Level);
}

UInt32 Vip::BuyGiftItem(UInt8 vipLevel)
{
	if (vipLevel > GetLevel()) return ErrorCode::PLAYER_VIP_BUY_LEVEL_ERROR;
	VipDataEntry* dataEntry = VipDataEntryMgr::Instance()->FindEntry(vipLevel);
	if (dataEntry == NULL) return ErrorCode::PLAYER_VIP_BUY_LEVEL_ERROR;
	if (dataEntry->giftItems.empty()) return ErrorCode::PLAYER_VIP_GIFT_EMPTY;
	//if (GetOwner()->GetPointBoth() < dataEntry->giftDiscountPrice) return ErrorCode::PLAYER_VIP_BUY_GIFT_ENOUGH_POINT;
	if (GetOwner()->GetPoint() < dataEntry->giftDiscountPrice) return ErrorCode::PLAYER_VIP_BUY_GIFT_ENOUGH_POINT;
	
	std::bitset<32> vipBuyBit = GetOwner()->GetCounter(COUNTER_VIP_GIFT_BUY_BIT);
	if (vipBuyBit.test(dataEntry->giftId)) return ErrorCode::PLAYER_VIP_BUY_ALREADY;

	RewardGroup* rewardGroup = GetOwner()->GetRewardGroup();
	rewardGroup->GetFitRewards(GetOwner(), dataEntry->giftItems);

	//检查包裹
	if (ErrorCode::SUCCESS != GetOwner()->CheckAddRewards(rewardGroup)) return ErrorCode::PLAYER_VIP_BUY_NOT_ENOUGH_PACKSIZE;

	// 安全锁开启，不允许消耗点券
	if (!GetOwner()->SecurityLockCheck())
		return ErrorCode::SECURITY_LOCK_FORBID_OP;

	//扣点卷
	//if (!GetOwner()->RemovePointBoth(GetReason(SOURCE_TYPE_VIP_BUY_GIFT, vipLevel).c_str(), dataEntry->giftDiscountPrice)) return ErrorCode::PLAYER_VIP_BUY_REMOVE_POINT_ERROR;
	GetOwner()->RemovePoint(GetReason(SOURCE_TYPE_VIP_BUY_GIFT, dataEntry->giftId).c_str(), dataEntry->giftDiscountPrice);
	
	//发礼包
	GetOwner()->AddRewards(GetReason(SOURCE_TYPE_VIP_BUY_GIFT, dataEntry->giftId).c_str(), rewardGroup, true);

	vipBuyBit.set(dataEntry->giftId);
	GetOwner()->SetCounter(COUNTER_VIP_GIFT_BUY_BIT, (UInt32)vipBuyBit.to_ulong());

	return ErrorCode::SUCCESS;
}

void Vip::SaveToDB()
{
#ifdef _SAVE_ACCOUNT_INFO_BY_SCENE_
	CLUpdateCommand* cmd = CLRecordClient::Instance()->PrepareUpdateCommand(m_Owner->GetSrcZoneId(), "t_account", m_Owner->GetAccGuid(), false);
	cmd->PutData("viplvl", m_Level);
	cmd->PutData("vipexp", m_Exp);
	CLRecordClient::Instance()->SendCommand(cmd);
#endif
}

void Vip::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	stream & m_Level & m_Exp;
}

void Vip::Decode(Avalon::NetInputStream& stream)
{
	stream & m_Level & m_Exp;
}