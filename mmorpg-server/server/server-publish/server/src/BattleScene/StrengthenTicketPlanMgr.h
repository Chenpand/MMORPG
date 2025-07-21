#ifndef _SYNTHESIS_PROBABILITY_MGR_H_
#define _SYNTHESIS_PROBABILITY_MGR_H_


#include <CLDefine.h>


// �ϳɼ����ֵ
#define STREN_TICKET_SYNTHESIS_CALC_BASE_VALUE 10000

// ��̬�ֲ������ֵ
#define NORMAL_DISTRIBUTION_CALC_BASE_VALUE 1000

// ǿ��ȯ���ʻ�ֵ
#define STRENGTHEN_TICKET_RATE_BASE_VALUE 1000

class Player;
class ItemDataEntry;
class StrTicketData;

// ǿ��ȯ��������
class StrengthenTicketPlanMgr : public Avalon::Singleton<StrengthenTicketPlanMgr>
{
	// ǿ��ȯ�ϳɷ���
	struct SynthesisPlan
	{
		SynthesisPlan()
		:strengthenProbOfSuccess(0), synthesisProbOfSuccess(0){}

		// ǿ���ɹ�����
		UInt32 strengthenProbOfSuccess;
		// �ϳɳɹ�����
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

		// ǿ��ȯA�ȼ�
		UInt8 strenTicketALevel;
		// ǿ��ȯA����
		UInt32 strenTicketARate;
		// ǿ��ȯA�ںϼ�ֵ
		UInt32 strenTicketAfuseValue;
		// ǿ��ȯB�ȼ�
		UInt8 strenTicketBLevel;
		// ǿ��ȯB����
		UInt32 strenTicketBRate;
		// ǿ��ȯB�ںϼ�ֵ
		UInt32 strenTicketBfuseValue;
	};

	// ǿ��ȯ�ںϷ���
	struct FusePlan
	{
		FusePlan() :fuseValue(0), fuseProb(0), giveTicketDataId(0), needRandTicketAgain(true){}

		bool operator==(const FusePlan* rhs) const
		{
			return fuseValue == rhs->fuseValue;
		}

		// �ںϼ�ֵ
		UInt32 fuseValue;
		// �ںϸ���
		UInt32 fuseProb;
		// �����ǿ��ȯdataId
		UInt32 giveTicketDataId;
		// �Ƿ���Ҫ�ٴ����ǿ��ȯ
		bool needRandTicketAgain;
	};

public:
	bool Init();

public:
	// �ϳ�ǿ��ȯ
	UInt32 SynthesisStrengthenTicket(Player* player, UInt32 planId);

	// �ں�ǿ��ȯ
	UInt32 FuseStrengthenTickets(Player* player, ObjID_t pickTicketA, ObjID_t pickTicketB);

public:
	// ��¼�ϳɷ������ļ�
	void RecordSynthesisPlanToFile();

	// ����ָ���ںϷ�������¼
	bool CalcAssignFusePlanAndRecordFile(UInt32 beginQueryIndex, UInt32 endQueryIndex, bool needTimeOutSavePlan = false, UInt64 timeMax = 50000);

	// �����ںϷ���
	void CalculateFusePlans(UInt32 dataIdA, UInt32 dataIdB);

private:
	// ��ʼ���ϳɷ���
	bool _InitStrengthenTicketSynthesisPlan();

	// ���Һϳɷ���
	bool _FindSynthesisPlans(UInt32 planId, std::vector<SynthesisPlan*>& synthesisPlans);

	// ����ϳɷ���
	SynthesisPlan* _RandSynthesisPlan(const std::vector<SynthesisPlan*>& synthesisPlans);

private:
	// �����ںϷ���
	bool _CalcFusePlan(const StrTicketData* dataA, const StrTicketData* dataB, std::vector<FusePlan>& fusePlans);

	// ���ǿ��ȯdata
	bool _RandStrengthenTicketDataByFusePlan(FusePlan* fusePlan, UInt32 lowerLevel, UInt32 upperLevel);

	// ����ںϷ���
	bool _RandFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, FusePlan& fusePlan);

	// ����ںϷ���
	void _AddFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, const FusePlan& fusePlan);

	// �����ںϷ���
	bool _FindFusePlan(const StrTicketData* strenTicketDataA, const StrTicketData* strenTicketDataB, std::vector<FusePlan*>& fusePlans);

private:
	// ������̬�ֲ�
	bool _CalcNormalDistribution(UInt32 mu, UInt32 sigmal, UInt32 input, UInt32& output, UInt32 leftBoundary = 0, UInt32 rightBoundary = 0);
	bool _CalcNormalDistribution2(UInt32 mu, UInt32 sigmal, UInt32 input, UInt32& output);

	// ����ǿ��ȯdata�����Ͱ󶨻�ȡ����data
	ItemDataEntry* _GetItemDataByStrengthenTicketDataIndex(UInt32 index, UInt8 binding);

private:
	// ǿ��ȯ�ϳɷ���
	std::map<UInt32, std::vector<SynthesisPlan*> > m_StrengthenTicketSynthesisPlan;

	// ǿ��ȯ�ںϷ���
	std::map <FusePlanKey, std::vector<FusePlan*> > m_StrengthenTicketFusePlan;
};


#endif