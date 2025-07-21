#include "ActivityAttributeMgr.h"

#include <CLActivityAttributeDefine.h>
#include <CLMsgParser.h>

#include "PlayerMgr.h"
#include "ActivityAttribute.h"
#include "SceneSysNotify.h"


ActivityAttributeMgr::ActivityAttributeMgr()
{

}

ActivityAttributeMgr::~ActivityAttributeMgr()
{
	for (auto elem : m_OpActAttrVec)
	{
		delete elem;
	}
	m_OpActAttrVec.clear();
}

void ActivityAttributeMgr::LoadOpActivityAttributeFromDB(CLRecordCallback* callback)
{
	if (!callback) return;

	while (callback->NextRow())
	{
		UInt32 opActId = callback->GetData("activity_id");
		auto opActAttr = FindOpActivityAttribute(opActId);
		if (opActAttr != NULL)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Repeat opactivity(" << opActId << ") attribute!" << LogStream::eos;
			continue;
		}

		opActAttr = new OpActivityAttribute();
		opActAttr->SetID(callback->GetKey());
		opActAttr->SetDBData(callback);
		m_OpActAttrVec.push_back(opActAttr);
	}
}

OpActivityAttribute* ActivityAttributeMgr::CreateAttribute(UInt32 opActId)
{
	auto opActAttr = new OpActivityAttribute();
	opActAttr->SetOpActId(opActId);
	opActAttr->SetOwner(GetOwner()->GetID());
	m_OpActAttrVec.push_back(opActAttr);
	return opActAttr;
}

OpActivityAttribute* ActivityAttributeMgr::FindOpActivityAttribute(UInt32 opActId)
{
	for (auto opActAttr : m_OpActAttrVec)
	{
		if (!opActAttr) continue;

		if (opActAttr->GetOpActId() == opActId)
		{
			return opActAttr;
		}
	}
	return NULL;
}

void ActivityAttributeMgr::SyncActivityAttributes(const OpActivityRegInfo* opAct)
{
	if (!opAct) return;

	switch ((OpActivityType)opAct->tmpType)
	{
	case OAT_ARTIFACT_JAR:
		if (opAct->state == AS_IN)
		{
			SyncArtifactJarDiscountInfo(opAct->dataId);
		}
		break;

	default:
		break;
	}
}

void ActivityAttributeMgr::OnOpenJar(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney)
{
	if (!opAct) return;
	
	DiscountJarCostMoneyByOpActivity(opAct, comboBuyNum, costMoney);
}

void ActivityAttributeMgr::DiscountJarCostMoneyByOpActivity(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney)
{
	if (!opAct) return;

	if (costMoney == 0)
	{
		return;
	}

	switch ((OpActivityType)opAct->tmpType)
	{
	case OAT_ARTIFACT_JAR:
		_DiscountArtifactJarCostMoney(opAct, comboBuyNum, costMoney);
		break;

	default:
		ErrorLogStream << PLAYERINFO(GetOwner()) << opAct->LogStr() << " is not open jar discount." << LogStream::eos;
		break;
	}
}

void ActivityAttributeMgr::SyncArtifactJarDiscountInfo(UInt32 opActId)
{
	auto opActAttr = FindOpActivityAttribute(opActId);
	if (!opActAttr)
	{
		InfoLogStream << PLAYERINFO(GetOwner()) << " can not find opActivity(" << opActId << ")'s attribute!" << LogStream::eos;
		GetOwner()->SyncArtifactJarDiscountInfo(opActId, AJDES_IN);
		return;
	}

	// 刷新
	if (opActAttr->TryRefresh(TimeUtil::REFRESH_TYPE_PER_DAY, TimeUtil::RefreshDate()))
	{
		opActAttr->SetVarNoSync(ACT_ATTR_ARTIFACT_JAR_EXTRACT_TIMES, 0);
		opActAttr->SetVarNoSync(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_RATE, 0);
		opActAttr->SetVarNoSync(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES, 0);

		InfoLogStream << PLAYERINFO(GetOwner()) << " opActivity(" << opActId << ") refresh attribute!" << LogStream::eos;
		GetOwner()->SyncArtifactJarDiscountInfo(opActId, AJDES_IN);
		return;
	}

	// 抽取次数
	UInt32 extractTimes = (UInt32)opActAttr->GetVar(ACT_ATTR_ARTIFACT_JAR_EXTRACT_TIMES);

	// 没抽过
	if (extractTimes == 0)
	{
		GetOwner()->SyncArtifactJarDiscountInfo(opActId, AJDES_IN);
		return;
	}

	// 折扣率
	UInt32 discountRate = (UInt32)opActAttr->GetVar(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_RATE);
	// 折扣可用次数
	UInt32 discountEffectTimes = (UInt32)opActAttr->GetVar(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES);

	GetOwner()->SyncArtifactJarDiscountInfo(opActId, AJDES_OVER, discountRate, discountEffectTimes);
}

bool ActivityAttributeMgr::CanExtractArtifactJarDiscount(UInt32 opActId)
{
	auto opActAttr = FindOpActivityAttribute(opActId);
	if (opActAttr != NULL)
	{
		// 抽取次数
		UInt32 extractTimes = (UInt32)opActAttr->GetVar(ACT_ATTR_ARTIFACT_JAR_EXTRACT_TIMES);

		// 抽过了
		if (extractTimes > 0)
		{
			return false;
		}
	}

	return true;
}

void ActivityAttributeMgr::OnExtractArtifactJarDiscount(UInt32 opActId, UInt32 discountRate, UInt32 effectTimes)
{
	auto opActAttr = FindOpActivityAttribute(opActId);
	if (!opActAttr)
	{
		opActAttr = CreateAttribute(opActId);
	}

	opActAttr->IncVarNoSync(ACT_ATTR_ARTIFACT_JAR_EXTRACT_TIMES, 1);
	opActAttr->SetVarNoSync(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_RATE, (Int64)discountRate);
	opActAttr->SetVarNoSync(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES, (Int64)effectTimes);
	opActAttr->TryRefresh(TimeUtil::REFRESH_TYPE_PER_DAY, TimeUtil::RefreshDate());

	if (discountRate >= 1 && discountRate <= 5)
	{
		// 走马灯

		std::string playerLinkStr;
		if (!MsgPackage::GetPlayerMsgTag(playerLinkStr, GetOwner()->GetID(), GetOwner()->GetName(), GetOwner()->GetOccu(), GetOwner()->GetLevel()))
		{
			playerLinkStr = GetOwner()->GetName();
		}
		Sys::SendAnnouncement(91, playerLinkStr.c_str(), effectTimes, discountRate);
	}

	InfoLogStream << PLAYERINFO(GetOwner()) << " on extract discount(rate:" << discountRate << ",effectTimes:" << effectTimes << "), now extract times(" << opActAttr->GetVar(ACT_ATTR_ARTIFACT_JAR_EXTRACT_TIMES) << ")." << LogStream::eos;

	GetOwner()->SendExtractArtifactJarDiscountUdpLog(opActId, discountRate, effectTimes);
}

void ActivityAttributeMgr::_DiscountArtifactJarCostMoney(const OpActivityRegInfo* opAct, UInt32 comboBuyNum, UInt32& costMoney)
{
	if (!opAct) return;

	// 没有抽过
	auto opActAttr = FindOpActivityAttribute(opAct->dataId);
	if (!opActAttr)
	{
		return;
	}

	// 有效次数为0
	UInt32 effecTimes = (UInt32)opActAttr->GetVar(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES);
	if (effecTimes == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " artifact jar discount effect times is zero!" << LogStream::eos;
		return;
	}

	UInt32 discountRate = (UInt32)opActAttr->GetVar(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_RATE);
	if (discountRate == 0)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " artifact jar discount rate is zero!" << LogStream::eos;
		return;
	}

	// 折扣错误
	if (discountRate > ACT_ATTR_ARTIFACT_JAR_DISCOUNT_CALC_BASE_VALUE)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " artifact jar discount rate(" << discountRate << ") is error!" << LogStream::eos;
		return;
	}

	// 打折

	UInt32 originalCost = costMoney;
	
	if (effecTimes >= comboBuyNum)
	{
		opActAttr->SetVarNoSync(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES, effecTimes - comboBuyNum);
		costMoney = originalCost * discountRate / ACT_ATTR_ARTIFACT_JAR_DISCOUNT_CALC_BASE_VALUE;
	}
	else
	{
		opActAttr->SetVarNoSync(ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES, 0);
		UInt32 uninCost = originalCost / comboBuyNum;
		costMoney = uninCost * effecTimes * discountRate / ACT_ATTR_ARTIFACT_JAR_DISCOUNT_CALC_BASE_VALUE + uninCost * (comboBuyNum - effecTimes);
	}

	SyncArtifactJarDiscountInfo(opAct->dataId);

	InfoLogStream << PLAYERINFO(GetOwner()) << " after artifact jar discount rate(" << discountRate << "), cost money(old:" << originalCost << ",new:" << costMoney << ")." << LogStream::eos;
}