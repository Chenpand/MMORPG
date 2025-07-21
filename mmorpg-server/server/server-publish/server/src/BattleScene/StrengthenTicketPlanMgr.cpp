#include "StrengthenTicketPlanMgr.h"

#include <CLErrorCode.h>
#include <DropItemMgr.h>
#include <CLSystemValueDataEntry.h>
#include <CLItemDataEntry.h>
#include <CLStrengthenTicketSynthesisDataEntry.h>
#include <CLStrengthenTicketFuseDataEntry.h>
#include <CLStrenTicketFusePramMappingDataEntry.h>
#include <CLStrenTicketFuseMaterialDataEntry.h>
#include <CLStandardNormalDistributionDataEntry.h>

#include "Player.h"
#include "SSApplication.h"


bool StrengthenTicketPlanMgr::Init()
{
	if (!_InitStrengthenTicketSynthesisPlan())
	{
		return false;
	}

	StrTicketDataMgr::Instance()->SortFuseStrengthenTicketsByFuseValue();

	return true;
}

UInt32 StrengthenTicketPlanMgr::SynthesisStrengthenTicket(Player* player, UInt32 planId)
{
	if (!player) return ErrorCode::ITEM_DATA_INVALID;

	// ��鱳������
	if (!player->GetItemMgr().CheckPackageCapacityEnough(PACK_MATERIAL, 1))
	{
		return ErrorCode::ITEM_PACKSIZE_MAX;
	}

	auto synthesisData = StrengthenTicketSynthesisDataEntryMgr::Instance()->FindEntry(planId);
	if (!synthesisData)
	{
		ErrorLogStream << "Can not find strengthen ticket synthesis data(" << planId << ")!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	InfoLogStream << PLAYERINFO(player) << " begin synthesis strengthen ticket by plan(" << planId << ")." << LogStream::eos;

	ItemRewardVec reduceCostMatRates;

	static UInt32 REUDECE_CALC_BASE_VALUE = 100;

	ItemRewardVec costMaterials = synthesisData->costMaterials;
	// �������Ƿ��㹻
	for (auto& costMaterial : costMaterials)
	{
		for (auto reduceCost : reduceCostMatRates)
		{
			if (reduceCost.id == costMaterial.id)
			{
				// �����Ǽ��ٰٷֱ���ֵ,����Ӧ����0-99
				if (REUDECE_CALC_BASE_VALUE < reduceCost.num)
				{
					ErrorLogStream << PLAYERINFO(player) << " reduce cost item(" << reduceCost.id << ")'s rate(" << reduceCost.num << ") is more than 100!" << LogStream::eos;
					return ErrorCode::ITEM_NO_REASON;
				}

				UInt32 reduceNum = costMaterial.num * reduceCost.num / REUDECE_CALC_BASE_VALUE;

				if (reduceNum > costMaterial.num)
				{
					reduceNum = costMaterial.num;
				}

				costMaterial.num -= reduceNum;

				InfoLogStream << PLAYERINFO(player) << " reduce cost material(" << costMaterial.id << "), now cost num=" << costMaterial.num << ", reduce num=" << reduceNum << LogStream::eos;
			}
		}

		if (player->GetItemNum(costMaterial.id) < costMaterial.num)
		{
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}
	}

	// ���ҷ���
	std::vector<SynthesisPlan*> synthesisPlans;
	if (!_FindSynthesisPlans(planId, synthesisPlans))
	{
		ErrorLogStream << "Can not find strengthen ticket synthesis plan(" << planId << ")!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// �������
	SynthesisPlan* synthesisPlan = _RandSynthesisPlan(synthesisPlans);
	if (!synthesisPlan)
	{
		ErrorLogStream << PLAYERINFO(player) << " rand synthesis plan failed!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// ���ݵȼ��͸����ҵ�ǿ��ȯdata����
	StrTicketData* strenTicketData = StrTicketDataMgr::Instance()->GetFuseStrengthenTicketData(synthesisData->strengthenLv, synthesisPlan->strengthenProbOfSuccess);
	if (!strenTicketData)
	{
		ErrorLogStream << "Can not find possible tickets by level(" << synthesisData->strengthenLv << ") and prob(" << synthesisPlan->strengthenProbOfSuccess << ")!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	// �����ǿ��ȯ����data
	ItemDataEntry* giveStrenTicketItemData = _GetItemDataByStrengthenTicketDataIndex(strenTicketData->id, synthesisData->binding);
	if (!giveStrenTicketItemData)
	{
		ErrorLogStream << PLAYERINFO(player) << " give strengthen ticket item data ptr is null!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_STRENGTHEN_TICKET_SYNTHESIS, planId);

	// �۲��Ͽ�Ǯ
	for (auto costMaterial : costMaterials)
	{
		if (!player->RemoveItem(reason.c_str(), costMaterial.id, costMaterial.num))
		{
			ErrorLogStream << PLAYERINFO(player) << " remove item failed, id=" << costMaterial.id << ", num=" << costMaterial.num << LogStream::eos;
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}
	}

	// ����ǿ��ȯ
	if (player->AddItem(reason.c_str(), giveStrenTicketItemData->id, 1) == 0)
	{
		ErrorLogStream << PLAYERINFO(player) << " add item(" << giveStrenTicketItemData->id << ") failed!" << LogStream::eos;
		return ErrorCode::ITEM_NO_REASON;
	}

	InfoLogStream << PLAYERINFO(player) << " synthesis strengthen ticket success, give ticket(" << giveStrenTicketItemData->id << "), propery(" << strenTicketData->id << ")." << LogStream::eos;

	return ErrorCode::SUCCESS;
}

UInt32 StrengthenTicketPlanMgr::FuseStrengthenTickets(Player* player, ObjID_t pickTicketA, ObjID_t pickTicketB)
{
	UInt64 beginTime = TimeUtil::GetMicroSecondNow();

	if (!player) return ErrorCode::ITEM_DATA_INVALID;

	// ��鱳������
	if (!player->GetItemMgr().CheckPackageCapacityEnough(PACK_MATERIAL, 1))
	{
		return ErrorCode::ITEM_PACKSIZE_MAX;
	}

	// ������ݺϷ�

	auto pickItemA = player->GetItemMgr().FindItem(pickTicketA);
	auto pickItemB = player->GetItemMgr().FindItem(pickTicketB);

	if (!pickItemA || !pickItemA->GetDataEntry() ||
		!pickItemB || !pickItemB->GetDataEntry())
	{
		return ErrorCode::ITEM_NO_REASON;
	}

	InfoLogStream << PLAYERINFO(player) << " begin fuse strengthen ticket with ticket(" << pickTicketA << "," << pickItemA->GetDataID() << ") and ticket(" << pickTicketB << "," << pickItemB->GetDataID() << ")." << LogStream::eos;

	if (pickTicketA == pickTicketB)
	{
		if (pickItemA->GetNum() < 2)
		{
			return ErrorCode::ITEM_NUM_INVALID;
		}
	}
	else
	{
		if (pickItemA->GetNum() < 1 || pickItemB->GetNum() < 1)
		{
			return ErrorCode::ITEM_NUM_INVALID;
		}
	}

	if (pickItemA->GetSubType() != ST_COUPON ||
		pickItemB->GetSubType() != ST_COUPON)
	{
		ErrorLogStream << PLAYERINFO(player) << " item subtype error!" << LogStream::eos;
		return ErrorCode::ITEM_TYPE_INVALID;
	}

	UInt32 dataIndexA = pickItemA->GetDataEntry()->strenTicketDataIndex;
	UInt32 dataIndexB = pickItemB->GetDataEntry()->strenTicketDataIndex;

	auto strenTicketDataA = StrTicketDataMgr::Instance()->FindEntry(dataIndexA);
	auto strenTicketDataB = StrTicketDataMgr::Instance()->FindEntry(dataIndexB);

	if (!strenTicketDataA || !strenTicketDataB)
	{
		ErrorLogStream << "Can not find strengthen ticket data by id(" << dataIndexA << " or " << dataIndexB << ")!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	if (!strenTicketDataA->canFuse || !strenTicketDataB->canFuse)
	{
		ErrorLogStream << PLAYERINFO(player) << " strengthen ticket can not been fuse!" << LogStream::eos;
		return ErrorCode::ITEM_NO_REASON;
	}

	// *���Ĳ��ܲ���
	UInt32 strenLevelLowerLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LOWER_LIMIT);
	UInt32 strenLevelUpperLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LIMIT);

	if (strenTicketDataA->effectLv <= strenLevelLowerLimit || strenTicketDataB->effectLv <= strenLevelLowerLimit)
	{
		ErrorLogStream << PLAYERINFO(player) << " strengthen ticket level is more than limit(" << strenLevelLowerLimit << ")!" << LogStream::eos;
		return ErrorCode::ITEM_NO_REASON;
	}

	if (strenTicketDataA->effectLv >= strenLevelUpperLimit || strenTicketDataB->effectLv >= strenLevelUpperLimit)
	{
		ErrorLogStream << PLAYERINFO(player) << " strengthen ticket level is more than limit(" << strenLevelUpperLimit << ")!" << LogStream::eos;
		return ErrorCode::ITEM_NO_REASON;
	}

	// ���ݵȼ���ȡ�����õ��Ĳ���
	UInt32 higherLevel = strenTicketDataA->effectLv > strenTicketDataB->effectLv ? strenTicketDataA->effectLv : strenTicketDataB->effectLv;
	ItemRewardVec costMaterials = StrenTicketFuseMaterialDataEntryMgr::Instance()->GetFuseMaterialsByStrenLevel(higherLevel);

	// �������Ƿ��㹻
	for (auto costMaterial : costMaterials)
	{
		if (player->GetItemNum(costMaterial.id) < costMaterial.num)
		{
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}
	}

	// ������ںϷ���
	FusePlan fusePlan;
	UInt64 beginTimeOfRand = TimeUtil::GetMicroSecondNow();
	if (!_RandFusePlan(strenTicketDataA, strenTicketDataB, fusePlan))
	{
		ErrorLogStream << PLAYERINFO(player) << " rand failed by strengthen ticket data index(" << dataIndexA << " and " << dataIndexB << ")!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}
	UInt64 endTimeOfRand = TimeUtil::GetMicroSecondNow();
	UInt64 useTimeOfRand = endTimeOfRand - beginTimeOfRand;

	if (useTimeOfRand >= 80000)
	{
		ErrorLogStream << "Rand fuse plan use too long time(" << useTimeOfRand << ") by dataA(" << strenTicketDataA->id << ") and dataB(" << strenTicketDataB->id << ")." << LogStream::eos;
	}

	// Ҫ�ϳɵ�ǿ��ȯ����
	UInt8 fuseTicketOwener = pickItemA->GetBind() > pickItemB->GetBind() ? pickItemA->GetBind() : pickItemB->GetBind();

	// �����ǿ��ȯ����data
	ItemDataEntry* giveStrenTicketItemData = _GetItemDataByStrengthenTicketDataIndex(fusePlan.giveTicketDataId, fuseTicketOwener);
	if (!giveStrenTicketItemData)
	{
		ErrorLogStream << PLAYERINFO(player) << " give strengthen ticket item data ptr is null!" << LogStream::eos;
		return ErrorCode::ITEM_DATA_INVALID;
	}

	std::string reason = GetReason(LogSourceType::SOURCE_TYPE_STRENGTHEN_TICKET_FUSE);

	// �۲���
	for (auto costMaterial : costMaterials)
	{
		if (!player->RemoveItem(reason.c_str(), costMaterial.id, costMaterial.num))
		{
			ErrorLogStream << PLAYERINFO(player) << " remove item failed, id=" << costMaterial.id << ", num=" << costMaterial.num << LogStream::eos;
			return ErrorCode::ITEM_NOT_ENOUGH_MAT;
		}
	}

	// ��ǿ��ȯ
	if (pickTicketA == pickTicketB)
	{
		if (ErrorCode::SUCCESS != player->ReduceItemNum(reason.c_str(), pickItemA, 2))
		{
			ErrorLogStream << PLAYERINFO(player) << " remove item failed!" << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
	}
	else
	{
		if (ErrorCode::SUCCESS != player->ReduceItemNum(reason.c_str(), pickItemA) ||
			ErrorCode::SUCCESS != player->ReduceItemNum(reason.c_str(), pickItemB))
		{
			ErrorLogStream << PLAYERINFO(player) << " remove item failed!" << LogStream::eos;
			return ErrorCode::ITEM_DATA_INVALID;
		}
	}

	// ����ǿ��ȯ
	if (player->AddItem(reason.c_str(), giveStrenTicketItemData->id, 1) == 0)
	{
		ErrorLogStream << PLAYERINFO(player) << " add item(" << giveStrenTicketItemData->id << ") failed!" << LogStream::eos;
		return ErrorCode::ITEM_NO_REASON;
	}

	InfoLogStream << PLAYERINFO(player) << " fuse strengthen ticket success, give ticket(" << giveStrenTicketItemData->id << "), propery(" << fusePlan.giveTicketDataId << ")." << LogStream::eos;

	UInt64 endTime = TimeUtil::GetMicroSecondNow();
	UInt64 useTime = endTime - beginTime;

	if (useTime >= 100000)
	{
		ErrorLogStream << "Fuse plan use too long time(" << useTime << ") by dataA(" << strenTicketDataA->id << ") and dataB(" << strenTicketDataB->id << ")." << LogStream::eos;
	}

	return ErrorCode::SUCCESS;
}

void StrengthenTicketPlanMgr::RecordSynthesisPlanToFile()
{
	std::string rootpath = SSApplication::Instance()->GetStrengthenTicketPlanRecordPath();

	std::string filename = rootpath + "synthesis.csv";

	std::ofstream ofs;
	ofs.open(filename.c_str());
	if (!ofs.good())
	{
		ErrorLogStream << "open file(" << filename << ") failed!" << LogStream::eos;
		return;
	}

	std::string headStr = "dataId,strengthenLevel,lowerRange,upperRange,strengthenProbability,synthesisProbability\n";
	ofs.write(headStr.c_str(), headStr.size());

	for (auto elem : m_StrengthenTicketSynthesisPlan)
	{
		UInt32 synthId = elem.first;
		auto synthData = StrengthenTicketSynthesisDataEntryMgr::Instance()->FindEntry(synthId);
		if (!synthData) continue;

		std::string linePreStr = Avalon::StringUtil::ConvertToString<UInt32>(synthId);
		linePreStr.append(",").append(Avalon::StringUtil::ConvertToString<UInt32>(synthData->strengthenLv))
			.append(",").append(Avalon::StringUtil::ConvertToString<UInt32>(synthData->lower))
			.append(",").append(Avalon::StringUtil::ConvertToString<UInt32>(synthData->upper));

		for (auto plan : elem.second)
		{
			if (!plan) continue;

			std::ostringstream oss;
			oss << "," << plan->strengthenProbOfSuccess << "," << plan->synthesisProbOfSuccess << "\n";

			std::string lineStr(linePreStr);
			lineStr.append(oss.str());

			ofs.write(lineStr.c_str(), lineStr.size());
		}
	}

	ofs.close();
}

bool StrengthenTicketPlanMgr::CalcAssignFusePlanAndRecordFile(UInt32 beginQueryIndex, UInt32 endQueryIndex, bool needTimeOutSavePlan, UInt64 timeMax)
{
	std::vector<StrTicketData*> fuseTicketDatas = StrTicketDataMgr::Instance()->GetFuseStrTicketDatas();
	if (fuseTicketDatas.empty())
	{
		ErrorLogStream << "Fuse strengthen ticket datas are empty!" << LogStream::eos;
		return false;
	}

	UInt32 strenLevelLowerLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LOWER_LIMIT);
	UInt32 strenLevelUpperLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LIMIT);

	std::string rootpath = SSApplication::Instance()->GetStrengthenTicketPlanRecordPath();

	for (UInt32 idx = beginQueryIndex; idx < endQueryIndex; ++idx)
	{
		auto data = fuseTicketDatas[idx];
		if (!data) continue;

		// *����ǿ��ȯ�������ں�
		if (data->effectLv <= strenLevelLowerLimit || data->effectLv >= strenLevelUpperLimit)
		{
			continue;
		}

		std::string fileNameAppend = Avalon::StringUtil::ConvertToString<UInt32>(data->effectLv);
		std::string fileNameAppend2 = Avalon::StringUtil::ConvertToString<UInt32>(data->rate);

		std::string filename = rootpath + "fuse_" + fileNameAppend + "_" + fileNameAppend2 + ".csv";

		std::ofstream ofs;
		ofs.open(filename.c_str());
		if (!ofs.good())
		{
			ErrorLogStream << "open file(" << filename << ") failed!" << LogStream::eos;
			return false;
		}

		std::string headStr = "tableIdA,levelA,rateA,valueA,tableIdB,levelB,rateB,valueB,fuseValue,fuseProb,giveDataId,giveLevel,giveRate,giveFuseValue\n";
		ofs.write(headStr.c_str(), headStr.size());

		std::string lineAppend = ",";
		std::string lineAppend2 = Avalon::StringUtil::ConvertToString<UInt32>(data->id);
		std::string lineAppend3 = Avalon::StringUtil::ConvertToString<UInt32>(data->effectLv);
		std::string lineAppend4 = Avalon::StringUtil::ConvertToString<UInt32>(data->rate);
		std::string lineAppend5 = Avalon::StringUtil::ConvertToString<UInt32>(data->fuseValue);

		std::string abnorFilename = rootpath + "fuse_" + fileNameAppend + "_" + fileNameAppend2 + "_abnor.csv";
		std::ofstream abnorOfs;
		std::string abnorHeadStr = "tableIdA,levelA,rateA,valueA,tableIdB,levelB,rateB,valueB\n";
		bool hasWriteHead = false;

		for (auto data2 : fuseTicketDatas)
		{
			if (!data2) continue;

			// *����ǿ��ȯ�������ں�
			if (data2->effectLv <= strenLevelLowerLimit || data2->effectLv >= strenLevelUpperLimit)
			{
				continue;
			}

			// Ĭ��A��B�ĵȼ�С
			if (data->effectLv > data2->effectLv)
			{
				continue;
			}

			std::ostringstream oss;
			std::string lineAppend6 = Avalon::StringUtil::ConvertToString<UInt32>(data2->id);
			std::string lineAppend7 = Avalon::StringUtil::ConvertToString<UInt32>(data2->effectLv);
			std::string lineAppend8 = Avalon::StringUtil::ConvertToString<UInt32>(data2->rate);
			std::string lineAppend9 = Avalon::StringUtil::ConvertToString<UInt32>(data2->fuseValue);

			oss << lineAppend2 << lineAppend << lineAppend3 << lineAppend << lineAppend4 << lineAppend << lineAppend5 << lineAppend 
				<< lineAppend6 << lineAppend << lineAppend7 << lineAppend << lineAppend8 << lineAppend << lineAppend9;

			std::string linePreStr = oss.str();

			std::vector<FusePlan> fusePlans;

			UInt64 beginTime = TimeUtil::GetMicroSecondNow();
			if (!_CalcFusePlan(data, data2, fusePlans))
			{
				ErrorLogStream << "Can not find fuse plans by dataA(" << data->id << ") and dataB(" << data2->id << ")!" << LogStream::eos;
				if (!hasWriteHead)
				{
					abnorOfs.open(abnorFilename.c_str());
					if (!abnorOfs.good())
					{
						ErrorLogStream << "open file(" << abnorFilename << ") failed!" << LogStream::eos;
						continue;
					}

					hasWriteHead = true;
					abnorOfs.write(abnorHeadStr.c_str(), abnorHeadStr.size());
				}
				std::string lineStr(linePreStr);
				lineStr.append("\n");
				abnorOfs.write(lineStr.c_str(), lineStr.size());
				continue;
			}
			UInt64 endTime = TimeUtil::GetMicroSecondNow();
			UInt64 useTime = endTime - beginTime;
			InfoLogStream << "Fuse strengthen ticket use time(" << useTime << ") with by dataA(" << data->id << ") and dataB(" << data2->id << ")." << LogStream::eos;

			{
				for (auto plan : fusePlans)
				{
					if (needTimeOutSavePlan && useTime >= timeMax)
					{
						_AddFusePlan(data, data2, plan);
					}

					std::ostringstream oss;
					oss << "," << plan.fuseValue << "," << plan.fuseProb << "," << plan.giveTicketDataId;

					auto strData = StrTicketDataMgr::Instance()->FindEntry(plan.giveTicketDataId);
					if (strData != NULL)
					{
						oss << "," << strData->effectLv << "," << strData->rate << "," << strData->fuseValue;
					}

					oss << "\n";

					std::string lineStr(linePreStr);
					lineStr.append(oss.str());

					ofs.write(lineStr.c_str(), lineStr.size());
				}
			}
		}

		ofs.close();
		abnorOfs.close();
	}

	if (needTimeOutSavePlan)
	{
		UInt64 fusePlanMem = 0;
		for (auto elem : m_StrengthenTicketFusePlan)
		{
			UInt64 elemMem = (UInt64)sizeof(elem.first) + (UInt64)sizeof(elem.second) * (UInt64)elem.second.size();
			fusePlanMem += elemMem;
		}

		InfoLogStream << "Fuse plans(size:" << (UInt32)m_StrengthenTicketFusePlan.size() << ") need memory(" << fusePlanMem << ")" << LogStream::eos;
	}

	return true;
}

void StrengthenTicketPlanMgr::CalculateFusePlans(UInt32 dataIdA, UInt32 dataIdB)
{
	auto strenTicketDataA = StrTicketDataMgr::Instance()->FindEntry(dataIdA);
	if (!strenTicketDataA)
	{
		ErrorLogStream << "Can not find strengthen ticket data by id(" << dataIdA << ")!" << LogStream::eos;
		return;
	}

	// *����ǿ��ȯ�������ں�
	UInt32 strenLevelLowerLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LOWER_LIMIT);
	UInt32 strenLevelUpperLimit = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTHEN_TICKET_FUSE_STREN_LEVEL_LIMIT);

	if (strenTicketDataA->effectLv <= strenLevelLowerLimit || strenTicketDataA->effectLv >= strenLevelUpperLimit)
	{
		return;
	}

	StrTicketData* strenTicketDataB = NULL;

	if (dataIdB > 0)
	{
		strenTicketDataB = StrTicketDataMgr::Instance()->FindEntry(dataIdB);
	}

	std::string rootpath = SSApplication::Instance()->GetStrengthenTicketPlanRecordPath();

	if (!strenTicketDataB)
	{
		std::vector<StrTicketData*> fuseTicketDatas = StrTicketDataMgr::Instance()->GetFuseStrTicketDatas();
		if (fuseTicketDatas.empty())
		{
			return;
		}

		std::string fileAppend = Avalon::StringUtil::ConvertToString<UInt32>(strenTicketDataA->effectLv);
		std::string filename = rootpath + "fuse_" + fileAppend + ".csv";

		std::ofstream ofs;
		ofs.open(filename.c_str());
		if (!ofs.good())
		{
			ErrorLogStream << "open file(" << filename << ") failed!" << LogStream::eos;
			return;
		}

		std::string headStr = "valueA,valueB,fuseValue,fuseProbability,giveSTrengthenTicketDataId,level,rate,fuseValue\n";
		ofs.write(headStr.c_str(), headStr.size());

		for (auto strenTicketDataB : fuseTicketDatas)
		{
			if (!strenTicketDataB) continue;

			// *����ǿ��ȯ�������ں�
			if (strenTicketDataB->effectLv <= strenLevelLowerLimit || strenTicketDataB->effectLv >= strenLevelUpperLimit)
			{
				continue;
			}

			// Ĭ��A��B�ĵȼ�С
			if (strenTicketDataA->effectLv > strenTicketDataB->effectLv)
			{
				continue;
			}

			std::vector<FusePlan> fusePlans;

			if (!_CalcFusePlan(strenTicketDataA, strenTicketDataB, fusePlans))
			{
				ErrorLogStream << "Calculate fuse plans failed by ticketA(" << strenTicketDataA->id << ") and ticketB(" << strenTicketDataB->id << ")!" << LogStream::eos;
				return;
			}

			{
				std::string linePreStr = Avalon::StringUtil::ConvertToString<UInt32>(strenTicketDataA->fuseValue);
				linePreStr.append(",").append(Avalon::StringUtil::ConvertToString<UInt32>(strenTicketDataB->fuseValue));

				for (auto plan : fusePlans)
				{
					std::ostringstream oss;
					oss << "," << plan.fuseValue << "," << plan.fuseProb << "," << plan.giveTicketDataId;

					auto strData = StrTicketDataMgr::Instance()->FindEntry(plan.giveTicketDataId);
					if (strData != NULL)
					{
						oss << "," << strData->effectLv << "," << strData->rate << "," << strData->fuseValue;
					}

					oss << "\n";

					std::string lineStr(linePreStr);
					lineStr.append(oss.str());

					ofs.write(lineStr.c_str(), lineStr.size());
				}

				ofs.close();
			}
		}
	}
	else
	{
		// *����ǿ��ȯ�������ں�
		if (strenTicketDataB->effectLv <= strenLevelLowerLimit || strenTicketDataB->effectLv >= strenLevelUpperLimit)
		{
			return;
		}

		std::vector<FusePlan> fusePlans;

		if (!_CalcFusePlan(strenTicketDataA, strenTicketDataB, fusePlans))
		{
			ErrorLogStream << "Calculate fuse plans failed by ticketA(" << strenTicketDataA->id << ") and ticketB(" << strenTicketDataB->id << ")!" << LogStream::eos;
			return;
		}

		std::string fileAppend = Avalon::StringUtil::ConvertToString<UInt32>(strenTicketDataA->effectLv);
		fileAppend.append("_").append(Avalon::StringUtil::ConvertToString<UInt32>(strenTicketDataB->effectLv));
		std::string filename = rootpath + "fuse_" + fileAppend + ".csv";

		std::ofstream ofs;
		ofs.open(filename.c_str());
		if (!ofs.good())
		{
			ErrorLogStream << "open file(" << filename << ") failed!" << LogStream::eos;
			return;
		}

		std::string headStr = "valueA,valueB,fuseValue,fuseProbability,giveSTrengthenTicketDataId,level,rate,fuseValue\n";
		ofs.write(headStr.c_str(), headStr.size());

		std::string linePreStr = Avalon::StringUtil::ConvertToString<UInt32>(strenTicketDataA->fuseValue);
		linePreStr.append(",").append(Avalon::StringUtil::ConvertToString<UInt32>(strenTicketDataB->fuseValue));

		for (auto plan : fusePlans)
		{
			std::ostringstream oss;
			oss << "," << plan.fuseValue << "," << plan.fuseProb << "," << plan.giveTicketDataId;

			auto strData = StrTicketDataMgr::Instance()->FindEntry(plan.giveTicketDataId);
			if (strData != NULL)
			{
				oss << "," << strData->effectLv << "," << strData->rate << "," << strData->fuseValue;
			}

			oss << "\n";

			std::string lineStr(linePreStr);
			lineStr.append(oss.str());

			ofs.write(lineStr.c_str(), lineStr.size());
		}

		ofs.close();
	}
}

bool StrengthenTicketPlanMgr::_InitStrengthenTicketSynthesisPlan()
{
	class DatasVisit : public Avalon::DataEntryVisitor<StrengthenTicketSynthesisDataEntry>
	{
	public:
		bool operator()(StrengthenTicketSynthesisDataEntry* data)
		{
			datas.push_back(data);
			return true;
		}

		std::vector<StrengthenTicketSynthesisDataEntry*> datas;
	};

	// ��ȡ����->ȯ�ϳɷ�ʽ������
	DatasVisit visitor;
	StrengthenTicketSynthesisDataEntryMgr::Instance()->Visit(visitor);

	// ���㲻ͬǿ��ȯ�ȼ��¸�ǿ���ɹ�������Ӧ�ĺϳɳɹ�����
	for (auto data : visitor.datas)
	{
		if (!data) continue;

		// ǿ����������
		UInt32 lowerStrengthenProb = data->lower / 10;
		// ǿ����������
		UInt32 upperStrengthenProb = data->upper / 10;
		// mu
		UInt32 mu = data->mu;
		// sigmla
		UInt32 sigmal = data->sigmal;

		for (UInt32 value = lowerStrengthenProb; value <= upperStrengthenProb; ++value)
		{
			UInt32 prob = 0;
			if (!_CalcNormalDistribution(mu, sigmal, value, prob, lowerStrengthenProb, upperStrengthenProb))
			{
				ErrorLogStream << "Plan(" << data->id << ") calc mormal distribution2 failed!" << LogStream::eos;
				continue;
			}

			if (prob == 0)
			{
				continue;
			}

			SynthesisPlan* plan = new SynthesisPlan();
			plan->strengthenProbOfSuccess = value * 10;
			plan->synthesisProbOfSuccess = prob;
			m_StrengthenTicketSynthesisPlan[data->id].push_back(plan);
		}

		SynthesisPlan* plan = new SynthesisPlan();
		plan->strengthenProbOfSuccess = STRENGTHEN_TICKET_RATE_BASE_VALUE;
		plan->synthesisProbOfSuccess = data->synthProbOfPerfect;
		m_StrengthenTicketSynthesisPlan[data->id].push_back(plan);
	}

	return true;
}

bool StrengthenTicketPlanMgr::_FindSynthesisPlans(UInt32 planId, std::vector<SynthesisPlan*>& synthesisPlans)
{
	auto itr = m_StrengthenTicketSynthesisPlan.find(planId);
	if (itr != m_StrengthenTicketSynthesisPlan.end())
	{
		synthesisPlans = itr->second;
		return true;
	}

	return false;
}

StrengthenTicketPlanMgr::SynthesisPlan* StrengthenTicketPlanMgr::_RandSynthesisPlan(const std::vector<SynthesisPlan*>& synthesisPlans)
{
	if (synthesisPlans.empty())
	{
		ErrorLogStream << "Synthesis plans are empty!" << LogStream::eos;
		return NULL;
	}

	std::vector<SynthesisPlan*> outOfOrderPlans = WeightRander::DisorganizeData<SynthesisPlan*>(synthesisPlans);
	if (outOfOrderPlans.empty())
	{
		ErrorLogStream << "Synthesis plans are empty after diorganize!" << LogStream::eos;
		return NULL;
	}

	std::vector<UInt32> wts;

	for (auto data : outOfOrderPlans)
	{
		if (!data) continue;
		wts.push_back(data->synthesisProbOfSuccess);
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= (Int32)outOfOrderPlans.size() || hitIdx < 0)
	{
		ErrorLogStream << "Rand failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return NULL;
	}

	auto data = outOfOrderPlans[hitIdx];
	if (!data)
	{
		ErrorLogStream << "Rand failed, data entry ptr is null!" << LogStream::eos;
		return NULL;
	}

	return data;
}

bool StrengthenTicketPlanMgr::_CalcFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, std::vector<FusePlan>& fusePlans)
{
	if (!strenTicketDataA || !strenTicketDataB) return false;

	if (strenTicketDataA->effectLv > strenTicketDataB->effectLv)
	{
		const StrTicketData* tmpData = strenTicketDataA;
		strenTicketDataA = strenTicketDataB;
		strenTicketDataB = tmpData;
	}

	// ȯA����
	UInt32 strenTicketProbA = strenTicketDataA->rate;
	// ȯB����
	UInt32 strenTicketProbB = strenTicketDataB->rate;

	// ��������ǿ��ȯ���ܼ�ֵ
	UInt32 totalValue = strenTicketDataA->fuseValue + strenTicketDataB->fuseValue;

	// �����ںϽ��������

	// �ȼ�����
	UInt32 fuseTicketLowerLevel = strenTicketDataA->effectLv;
	// ��������
	UInt32 fuseTicketLowerProb = strenTicketProbA;
	// ��ֵ����
	UInt32 fuseTicketLowerValue = strenTicketDataA->fuseValue;

	// �ȼ�����
	UInt32 fuseTicketUpperLevel = strenTicketDataB->effectLv + 1;
	// ����ֵM
	UInt32 fixM = StrengthenTicketFuseDataEntryMgr::Instance()->GetFixM(fuseTicketUpperLevel);
	if (fixM == 0)
	{
		ErrorLogStream << "Strengthen level(" << fuseTicketUpperLevel << ") mapping fix M is zero!" << LogStream::eos;
		return false;
	}
	// ����
	UInt32 baseNum = 4;
	// ��������
	UInt32 fuseTicketUpperProb = 0;
	{
		UInt32 calcTmpValueA = strenTicketDataB->effectLv - strenTicketDataA->effectLv;
		UInt32 calcTmpValueB = calcTmpValueA > 0 ? baseNum : 1;
		for (UInt32 idx = 1; idx < calcTmpValueA; ++idx)
		{
			calcTmpValueB *= baseNum;
		}
		UInt32 tmpVarC = strenTicketProbA / calcTmpValueB + strenTicketProbB;
		UInt32 tmpVarD = tmpVarC < STRENGTHEN_TICKET_RATE_BASE_VALUE ? tmpVarC : STRENGTHEN_TICKET_RATE_BASE_VALUE;
		float tmpVarE = (float)tmpVarD / (float)fixM;
		UInt32 tmpVarF = (UInt32)(ceil(tmpVarE));
		fuseTicketUpperProb = tmpVarF * 10;
	}

	// ���ݵȼ��͸����ҵ�ǿ��ȯdata����
	StrTicketData* strenTicketUpperData = StrTicketDataMgr::Instance()->GetFuseStrengthenTicketData(fuseTicketUpperLevel, fuseTicketUpperProb);
	if (!strenTicketUpperData)
	{
		ErrorLogStream << "Can not find strengthen ticket data by level(" << fuseTicketUpperLevel << ") and prob(" << fuseTicketUpperProb << ")!" << LogStream::eos;
		return false;
	}

	// ��ֵ����
	UInt32 fuseTicketUpperValue = strenTicketUpperData->fuseValue;

	// ��־ƴ��
	std::ostringstream ticketFuseLogOss;
	ticketFuseLogOss << "ticketA(" << fuseTicketLowerLevel << "," << fuseTicketLowerProb << "," << fuseTicketLowerValue << ") and ticketB(" << strenTicketDataB->effectLv << "," << strenTicketDataB->rate 
		<< "," << strenTicketDataB->fuseValue << ")" << " tickeC(" << fuseTicketUpperLevel << "," << fuseTicketUpperProb << "," << fuseTicketUpperValue;
	std::string ticketFuseLogStr = ticketFuseLogOss.str();

	if (fuseTicketLowerValue > fuseTicketUpperValue)
	{
		ErrorLogStream << "Lower value is more than upper value while calculate " << ticketFuseLogStr << LogStream::eos;
		return false;
	}

	{
		// ������̬�ֲ�

		// �ܼ�ֵ����
		UInt32 totalValueFix = SystemValueDataEntryMgr::Instance()->GetSystemValue(SVT_STRENGTHEN_TICKET_FUSE_TOTAL_VALUE_FIX);
		if (totalValueFix > 0)
		{
			totalValue = totalValue * totalValueFix / NORMAL_DISTRIBUTION_CALC_BASE_VALUE;
		}
		// ϵ����
		UInt32 mu = totalValue;
		// ϵ����
		UInt32 sigmal = StrenTicketFusePramMappingDataEntryMgr::Instance()->GetSigmal(mu);
		if (sigmal == 0)
		{
			ErrorLogStream << "Mu(" << mu << ") mapping sigmal is zero!" << LogStream::eos;
			return false;
		}

		for (UInt32 value = fuseTicketLowerValue; value <= fuseTicketUpperValue; ++value)
		{
			UInt32 prob = 0;
			if (!_CalcNormalDistribution(mu, sigmal, value, prob, fuseTicketLowerValue, fuseTicketUpperValue))
			{
				ErrorLogStream << ticketFuseLogStr << " calc mormal distribution2 failed!" << LogStream::eos;
				continue;
			}

			if (prob == 0)
			{
				continue;
			}

			FusePlan fusePlan;
			fusePlan.fuseValue = value;
			fusePlan.fuseProb = prob;

			fusePlans.push_back(fusePlan);
		}
	}

	for (auto& fusePlan : fusePlans)
	{
		if (!_RandStrengthenTicketDataByFusePlan(&fusePlan, fuseTicketLowerLevel, fuseTicketUpperLevel))
		{
			ErrorLogStream << "Rand strengthen ticket data failed by " << ticketFuseLogStr << "!" << LogStream::eos;
		}
	}

	return true;
}

bool StrengthenTicketPlanMgr::_RandStrengthenTicketDataByFusePlan(FusePlan* fusePlan, UInt32 lowerLevel, UInt32 upperLevel)
{
	if (!fusePlan) return false;

	// ��ֵ��ͬ��ǿ��ȯdata
	std::vector<const StrTicketData*> sameFuseValueTicketDatas;
	// �ȼ�������ļ�ֵ�ٽ���data
	const StrTicketData* nearValueOverRangeTicketData = NULL;

	StrTicketDataMgr::Instance()->VisitFuseStrengthenTickets([&](const StrTicketData* data){
		if (!data) return true;

		if (lowerLevel <= data->effectLv && data->effectLv <= upperLevel)
		{
			if (fusePlan->fuseValue == data->fuseValue)
			{
				sameFuseValueTicketDatas.push_back(data);
			}
			else if (fusePlan->fuseValue > data->fuseValue)
			{
				nearValueOverRangeTicketData = data;
				return false;
			}
		}
		else
		{
			if (!sameFuseValueTicketDatas.empty())
			{
				return false;
			}
		}

		return true;
	});

	// �����ǿ��ȯdata
	const StrTicketData* giveTicketData = NULL;

	// ������������Ӧ�ļ�ֵ�������¼����ٽ��ļ�ֵ
	if (sameFuseValueTicketDatas.empty())
	{
		giveTicketData = nearValueOverRangeTicketData;
		fusePlan->needRandTicketAgain = false;
	}
	else if (sameFuseValueTicketDatas.size() == 1)
	{
		giveTicketData = sameFuseValueTicketDatas.front();
		fusePlan->needRandTicketAgain = false;
	}
	else
	{
		// ���������Ķ�Ӧ��ֵ�а�������ǿ��ȯ���ԣ����ѡ��һ��
		Int32 randNum = Avalon::Random::RandBetween(0, sameFuseValueTicketDatas.size() - 1);
		if (randNum < 0 || randNum >= (Int32)sameFuseValueTicketDatas.size())
		{
			ErrorLogStream << "Rand failed!" << LogStream::eos;
			return false;
		}

		giveTicketData = sameFuseValueTicketDatas[randNum];
	}

	if (!giveTicketData)
	{
		ErrorLogStream << "Give ticket data is ptr!" << LogStream::eos;
		return false;
	}

	fusePlan->giveTicketDataId = giveTicketData->id;
	return true;
}

bool StrengthenTicketPlanMgr::_RandFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, FusePlan& fusePlan)
{
	if (!strenTicketDataA || !strenTicketDataB)
	{
		return false;
	}

	if (strenTicketDataA->effectLv > strenTicketDataB->effectLv)
	{
		const StrTicketData* tmpData = strenTicketDataA;
		strenTicketDataA = strenTicketDataB;
		strenTicketDataB = tmpData;
	}

	// �ȴ��ڴ�����
	std::vector<FusePlan*> pFusePlans;
	if (_FindFusePlan(strenTicketDataA, strenTicketDataB, pFusePlans))
	{	
		for (auto fusePlan : pFusePlans)
		{
			if (!fusePlan) continue;

			if (!fusePlan->needRandTicketAgain)
			{
				continue;
			}

			if (!_RandStrengthenTicketDataByFusePlan(fusePlan, strenTicketDataA->effectLv, strenTicketDataB->effectLv + 1))
			{
				ErrorLogStream << "Rand strengthen ticket data failed by dataA(" << strenTicketDataA->id << ") and dataB(" << strenTicketDataB->id << ")!" << LogStream::eos;
			}
		}

		std::vector<FusePlan*> outOfOrderPlans = WeightRander::DisorganizeData<FusePlan*>(pFusePlans);
		if (!outOfOrderPlans.empty())
		{
			std::vector<UInt32> wts;

			for (auto data : outOfOrderPlans)
			{
				wts.push_back(data->fuseProb);
			}

			Int32 hitIdx = WeightRander::RandWeight(wts);
			if (hitIdx >= (Int32)outOfOrderPlans.size() || hitIdx < 0)
			{
				ErrorLogStream << "Rand failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
				return false;
			}

			fusePlan = *outOfOrderPlans[hitIdx];

			return true;
		}
	}

	// �Ҳ����ټ���

	UInt64 beginTime = TimeUtil::GetMicroSecondNow();
	std::vector<FusePlan> fusePlans;
	if (!_CalcFusePlan(strenTicketDataA, strenTicketDataB, fusePlans))
	{
		ErrorLogStream << "Can not find fuse plans by dataA(" << strenTicketDataA->id << ") and dataB(" << strenTicketDataB->id << ")!" << LogStream::eos;
		return false;
	}
	UInt64 endTime = TimeUtil::GetMicroSecondNow();
	UInt64 useTime = endTime - beginTime;
	
	UInt64 USE_TIME_MAX = SSApplication::Instance()->GetStrengthenTicketFuseUseMaxTime();

	if (useTime >= USE_TIME_MAX)
	{
		ErrorLogStream << "Calculate fuse plan use too long time(" << useTime << ") by dataA(" << strenTicketDataA->id << ") and dataB(" << strenTicketDataB->id << ")." << LogStream::eos;
	}

	if (fusePlans.empty())
	{
		ErrorLogStream << "Fuse plans are empty, by dataA(" << strenTicketDataA->id << ") and dataB(" << strenTicketDataB->id << ")!" << LogStream::eos;
		return false;
	}

	std::vector<FusePlan> outOfOrderPlans = WeightRander::DisorganizeData<FusePlan>(fusePlans);
	if (outOfOrderPlans.empty())
	{
		ErrorLogStream << "Fuse plans are empty after diorganize!" << LogStream::eos;
		return false;
	}

	std::vector<UInt32> wts;

	for (auto data : outOfOrderPlans)
	{
		wts.push_back(data.fuseProb);

		// ��ʱ̫���Ĵ�����
		if (useTime >= USE_TIME_MAX)
		{
			_AddFusePlan(strenTicketDataA, strenTicketDataB, data);
		}
	}

	Int32 hitIdx = WeightRander::RandWeight(wts);
	if (hitIdx >= (Int32)outOfOrderPlans.size() || hitIdx < 0)
	{
		ErrorLogStream << "Rand failed, hitIdx(" << hitIdx << ") is invalid!" << LogStream::eos;
		return false;
	}

	fusePlan = outOfOrderPlans[hitIdx];

	return true;
}

void StrengthenTicketPlanMgr::_AddFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, const FusePlan& fusePlan)
{
	if (!strenTicketDataA || !strenTicketDataB)
	{
		return;
	}

	FusePlanKey fusePlanKey;
	fusePlanKey.strenTicketALevel = strenTicketDataA->effectLv;
	fusePlanKey.strenTicketARate = strenTicketDataA->rate;
	fusePlanKey.strenTicketAfuseValue = strenTicketDataA->fuseValue;
	fusePlanKey.strenTicketBLevel = strenTicketDataB->effectLv;
	fusePlanKey.strenTicketBRate = strenTicketDataB->rate;
	fusePlanKey.strenTicketBfuseValue = strenTicketDataB->fuseValue;

	m_StrengthenTicketFusePlan[fusePlanKey].push_back(new FusePlan(fusePlan));
}

bool StrengthenTicketPlanMgr::_FindFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, std::vector<FusePlan*>& fusePlans)
{
	if (!strenTicketDataA || !strenTicketDataB)
	{
		return false;
	}

	FusePlanKey fusePlanKey;
	fusePlanKey.strenTicketALevel = strenTicketDataA->effectLv;
	fusePlanKey.strenTicketARate = strenTicketDataA->rate;
	fusePlanKey.strenTicketAfuseValue = strenTicketDataA->fuseValue;
	fusePlanKey.strenTicketBLevel = strenTicketDataB->effectLv;
	fusePlanKey.strenTicketBRate = strenTicketDataB->rate;
	fusePlanKey.strenTicketBfuseValue = strenTicketDataB->fuseValue;

	auto itr = m_StrengthenTicketFusePlan.find(fusePlanKey);
	if (itr == m_StrengthenTicketFusePlan.end())
	{
		return false;
	}

	fusePlans = itr->second;
	return true;
}

bool StrengthenTicketPlanMgr::_CalcNormalDistribution(UInt32 mu, UInt32 sigmal, UInt32 input, UInt32& output, UInt32 leftBoundary, UInt32 rightBoundary)
{
	if (input == 0)
	{
		return false;
	}

	UInt32 outputA = 0;
	if (!_CalcNormalDistribution2(mu, sigmal, input, outputA))
	{
		return false;
	}

	UInt32 outputB = 0;
	if (!_CalcNormalDistribution2(mu, sigmal, input - 1, outputB))
	{
		return false;
	}

	if (outputA < outputB)
	{
		ErrorLogStream << "OutputB(" << outputB << ") is less than outputA(" << outputA << "), mu=" << mu << ", sigmal=" << sigmal << ", input=" << input << LogStream::eos;
		return false;
	}

	output = outputA - outputB;

	if (input == leftBoundary)
	{
		UInt32 tmpProb = 0;
		if (!_CalcNormalDistribution2(mu, sigmal, input, tmpProb))
		{
			return false;
		}
		output += tmpProb;
	}
	else if (input == rightBoundary)
	{
		UInt32 tmpProb = 0;
		if (!_CalcNormalDistribution2(mu, sigmal, input, tmpProb))
		{
			return false;
		}
		if (NORMAL_DISTRIBUTION_CALC_BASE_VALUE < tmpProb)
		{
			ErrorLogStream << "Calc mormal distribution3 failed, result(" << tmpProb << ") is more than " << NORMAL_DISTRIBUTION_CALC_BASE_VALUE << ", mu=" << mu << ", sigmal=" 
				<< sigmal << ", input=" << input << ", leftB=" << leftBoundary << ", rightB=" << rightBoundary << LogStream::eos;
			return false;
		}
		output += (NORMAL_DISTRIBUTION_CALC_BASE_VALUE - tmpProb);
	}

	return true;
}

bool StrengthenTicketPlanMgr::_CalcNormalDistribution2(UInt32 mu, UInt32 sigmal, UInt32 input, UInt32& output)
{
	if (input >= mu)
	{
		UInt32 tmpVarA = input - mu;
		UInt32 tmpVarC = tmpVarA * 1000 / sigmal;
		output = StandardNormalDistributionDataEntryMgr::Instance()->GetOutputValue(tmpVarC);
	}
	else
	{
		UInt32 tmpVarA = mu - (input - 1);
		UInt32 tmpVarC = tmpVarA * 1000 / sigmal;
		UInt32 tmpVarD = StandardNormalDistributionDataEntryMgr::Instance()->GetOutputValue(tmpVarC);
		if (NORMAL_DISTRIBUTION_CALC_BASE_VALUE < tmpVarD)
		{
			ErrorLogStream << "Normal distribution value(" << tmpVarD << ") is less than " << NORMAL_DISTRIBUTION_CALC_BASE_VALUE << ", mu = " << mu << ", sigmal = " << sigmal << ", input = " << input << LogStream::eos;
			return false;
		}
		output = NORMAL_DISTRIBUTION_CALC_BASE_VALUE - tmpVarD;
	}

	return true;
}

ItemDataEntry* StrengthenTicketPlanMgr::_GetItemDataByStrengthenTicketDataIndex(UInt32 index, UInt8 binding)
{
	ItemDataEntryVec strenTicketItemDatas;
	if (!ItemDataEntryMgr::Instance()->GetItemDataByStrenTicketIndex(index, strenTicketItemDatas))
	{
		ErrorLogStream << "Can not find strengthen ticket's item data by index(" << index << ")!" << LogStream::eos;
		return NULL;
	}

	if (strenTicketItemDatas.size() != STREN_TICKET_INDEX_ITEM_DATA_NUM_LIMIT)
	{
		ErrorLogStream << "Strengthen ticket index(" << index << ") not match item data size(" << (UInt32)strenTicketItemDatas.size() << ")!" << LogStream::eos;
		return NULL;
	}

	// �����ǿ��ȯ����data
	ItemDataEntry* giveStrenTicketItemData = NULL;

	for (auto itemData : strenTicketItemDatas)
	{
		if (!itemData) continue;

		if (itemData->owner == binding)
		{
			giveStrenTicketItemData = itemData;
			break;
		}
	}

	if (!giveStrenTicketItemData)
	{
		ErrorLogStream << "Give strengthen ticket item data ptr is null!" << LogStream::eos;
		return NULL;
	}

	return giveStrenTicketItemData;
}