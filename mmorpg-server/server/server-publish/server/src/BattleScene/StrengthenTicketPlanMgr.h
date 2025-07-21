#ifndef _SYNTHESIS_PROBABILITY_MGR_H_
#define _SYNTHESIS_PROBABILITY_MGR_H_


#include <CLDefine.h>


// 合成计算基值
#define STREN_TICKET_SYNTHESIS_CALC_BASE_VALUE 10000

// 正态分布计算基值
#define NORMAL_DISTRIBUTION_CALC_BASE_VALUE 1000

// 强化券概率基值
#define STRENGTHEN_TICKET_RATE_BASE_VALUE 1000

class Player;
class ItemDataEntry;
class StrTicketData;

// 强化券方案管理
class StrengthenTicketPlanMgr : public Avalon::Singleton<StrengthenTicketPlanMgr>
{
	// 强化券合成方案
	struct SynthesisPlan
	{
		SynthesisPlan()
		:strengthenProbOfSuccess(0), synthesisProbOfSuccess(0){}

		// 强化成功概率
		UInt32 strengthenProbOfSuccess;
		// 合成成功概率
		UInt32 synthesisProbOfSuccess;
	};

	struct FusePlanKey
	{
		FusePlanKey() :strenTicketALevel(0), strenTicketARate(0), strenTicketAfuseValue(0)
					,strenTicketBLevel(0), strenTicketBRate(0), strenTicketBfuseValue(0)
					{}

		bool operator<(const FusePlanKey& rhs) const
		{
			if (strenTicketALevel != rhs.strenTicketALevel)
				return strenTicketALevel < rhs.strenTicketALevel;

			if (strenTicketARate != rhs.strenTicketARate)
				return strenTicketARate < rhs.strenTicketARate;

			if (strenTicketAfuseValue != rhs.strenTicketAfuseValue)
				return strenTicketAfuseValue < rhs.strenTicketAfuseValue;

			if (strenTicketBLevel != rhs.strenTicketBLevel)
				return strenTicketBLevel < rhs.strenTicketBLevel;

			if (strenTicketBRate != rhs.strenTicketBRate)
				return strenTicketBRate < rhs.strenTicketBRate;

			if (strenTicketBfuseValue != rhs.strenTicketBfuseValue)
				return strenTicketBfuseValue < rhs.strenTicketBfuseValue;

			return false;
		}

		// 强化券A等级
		UInt8 strenTicketALevel;
		// 强化券A概率
		UInt32 strenTicketARate;
		// 强化券A融合价值
		UInt32 strenTicketAfuseValue;
		// 强化券B等级
		UInt8 strenTicketBLevel;
		// 强化券B概率
		UInt32 strenTicketBRate;
		// 强化券B融合价值
		UInt32 strenTicketBfuseValue;
	};

	// 强化券融合方案
	struct FusePlan
	{
		FusePlan() :fuseValue(0), fuseProb(0), giveTicketDataId(0), needRandTicketAgain(true){}

		bool operator==(const FusePlan* rhs) const
		{
			return fuseValue == rhs->fuseValue;
		}

		// 融合价值
		UInt32 fuseValue;
		// 融合概率
		UInt32 fuseProb;
		// 给予的强化券dataId
		UInt32 giveTicketDataId;
		// 是否需要再次随机强化券
		bool needRandTicketAgain;
	};

public:
	bool Init();

public:
	// 合成强化券
	UInt32 SynthesisStrengthenTicket(Player* player, UInt32 planId);

	// 融合强化券
	UInt32 FuseStrengthenTickets(Player* player, ObjID_t pickTicketA, ObjID_t pickTicketB);

public:
	// 记录合成方案到文件
	void RecordSynthesisPlanToFile();

	// 计算指定融合方案并记录
	bool CalcAssignFusePlanAndRecordFile(UInt32 beginQueryIndex, UInt32 endQueryIndex, bool needTimeOutSavePlan = false, UInt64 timeMax = 50000);

	// 计算融合方案
	void CalculateFusePlans(UInt32 dataIdA, UInt32 dataIdB);

private:
	// 初始化合成方案
	bool _InitStrengthenTicketSynthesisPlan();

	// 查找合成方案
	bool _FindSynthesisPlans(UInt32 planId, std::vector<SynthesisPlan*>& synthesisPlans);

	// 随机合成方案
	SynthesisPlan* _RandSynthesisPlan(const std::vector<SynthesisPlan*>& synthesisPlans);

private:
	// 计算融合方案
	bool _CalcFusePlan(const StrTicketData* dataA, const StrTicketData* dataB, std::vector<FusePlan>& fusePlans);

	// 随机强化券data
	bool _RandStrengthenTicketDataByFusePlan(FusePlan* fusePlan, UInt32 lowerLevel, UInt32 upperLevel);

	// 随机融合方案
	bool _RandFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, FusePlan& fusePlan);

	// 添加融合方案
	void _AddFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, const FusePlan& fusePlan);

	// 查找融合方案
	bool _FindFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, std::vector<FusePlan*>& fusePlans);

private:
	// 计算正态分布
	bool _CalcNormalDistribution(UInt32 mu, UInt32 sigmal, UInt32 input, UInt32& output, UInt32 leftBoundary = 0, UInt32 rightBoundary = 0);
	bool _CalcNormalDistribution2(UInt32 mu, UInt32 sigmal, UInt32 input, UInt32& output);

	// 根据强化券data索引和绑定获取道具data
	ItemDataEntry* _GetItemDataByStrengthenTicketDataIndex(UInt32 index, UInt8 binding);

private:
	// 强化券合成方案
	std::map<UInt32, std::vector<SynthesisPlan*> > m_StrengthenTicketSynthesisPlan;

	// 强化券融合方案
	std::map <FusePlanKey, std::vector<FusePlan*> > m_StrengthenTicketFusePlan;
};


#endif