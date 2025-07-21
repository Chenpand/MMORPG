#ifndef _OPEN_JAR_MGR_H_
#define _OPEN_JAR_MGR_H_
#include <vector>
#include "CLItemDataEntry.h"
#include "CLEventMgr.h"
#include "Item.h"

class Player;
class JarItem;
class FakeWeightBase;
struct OpActivityRegInfo;

#define MAGIC_BOX_GUILD_ID 51	//ħ������ID

//��������&Ʒ�� ���key
struct JarBonusColorKey
{
	JarBonusColorKey(UInt32 type, UInt8 color) : m_type(type), m_color(color){}

	UInt32 m_type;
	UInt8 m_color;

	bool operator < (const JarBonusColorKey& other) const
	{
		if (m_type < other.m_type)
			return true;
		if (other.m_type < m_type)
			return false;

		if (m_color < other.m_color)
			return true;
		if (other.m_color < m_color)
			return false;

		return false;
	}
};

class OpenJarMgr
{
public:
	OpenJarMgr();
	~OpenJarMgr();

	typedef std::vector<FakeWeightBase*>				FakeWeightVec;
	typedef std::vector<JarItem*>						BonusVec;
	typedef	std::map<JarBonusColorKey, BonusVec>		BonusPoolColorMap;				//�������&ְҵ����toƷ������
	typedef std::map<UInt32, BonusVec>				BonusPoolMap;					//�������&ְҵ����to����

	void SetOwner(Player* owner) { m_pOwner = owner; }
	Player* GetOwner() const { return m_pOwner; }

	bool	LoadJarItem(JarItem* jarItem);
	void	Init();

	//������
	UInt32	OpenJar(UInt32 id, UInt8 combo, UInt32& getPointId, UInt32& getPoint, UInt32& getPointCrit, UInt32& baseItemId, UInt32& baseItemNum, UInt32 opActId);

	//�����Ӳ��Խӿ�
	void	OpenJarSample(UInt32 type, UInt32 combo, UInt32 num);

	//������ӽ���
	UInt32	GiveJarRewards(const std::string& reason, UInt32 type);

	//������ӽ���
	void	GiveJarRewards(const std::string& reason, JarBonusDataEntry* data, UInt32 comboBuyNum, bool bNotify, UInt32& baseItemId, UInt32& baseItemNum);

	//������ӽ���
	void	GiveJarRewards(const std::string& reason, JarBonusDataEntry* data, UInt32 comboBuyNum, bool bNotify, UInt32& baseItemId, UInt32& baseItemNum, RewardGroup& rewardGroup);

	//ͨ�ó���ӽӿ�, type:��������
	void	ComRandJarItem(UInt32 type, std::string condCt);		

	//�Ƿ�����
	bool IsActivityJar(JarType type);

	//��ֵ����
	void	ResetPoint(bool online);

	//��ÿ����ӽ��
	void GetOpenResult(std::vector<Item*>& results, std::vector<OpenJarResult>& jarResult);

	//������ۿ��Ƿ�����
	bool IsActiveJarSaleReset(UInt32 jarId);

	//��ħ��
	UInt32 OpenMagicBox(UInt64 uid, UInt8 isBatch);

	//�����ӹ�������
	UInt32 OpenJarGift(UInt32 jarid);

	//�����ӹ�������
	UInt32 OpenJarGift(UInt32 jarid, RewardGroup& rewardGroup);

	//�Ƿ�װ�����ս�������
	bool IsEquipRewardJar(JarType type);

	// �Ƿ����ڿ���ħ��
	void StartOpenJar() { m_opening = true; }
	void FinishOpenJar() { m_opening = false; }
	bool IsOpenJar() { return m_opening; }

	// �������ĵ���
	void ClearHitItems();

	//��Global���ͻ����������
	void SendArtifactJarDataToGlobal();

	// ����������Ӵ���
	void OpenActifactJarData();

	// ���������ɽ���Ҫ����
	UInt32 GetActifactJarLotteryNeedCnt(UInt32 jarId);

	// �Ƿ��ǻ����������
	bool IsActivityArtifactJar(UInt32 jarId);

	// �������ӻ����
	void ArtifactJarActivity(UInt32 jarId, UInt32 cnt);

public:	//�¼�
	void OnOnline();
	void OnOffline();

private:
	//ע���¼�
	void RegistGameEvent();
	void UnRegistGameEvent();

	//��鿪������Ѵ���,���ؿ�����ѵĴ���
	UInt32 CheckFreeCD(JarBonusDataEntry* data);

	//��¼����ָ��Ʒ�ʵĵ���
	void OpenSpecifyItemRecord(UInt32 jarId, std::string jarName, JarType jarType);

	void RandJarItem(JarBonusDataEntry* data, bool isMagicBox = false);
	JarItem* HitProc(BonusVec& pool);

	void CreateMap(JarItem* jarItem);
	void DeleteMap(JarItem* jarItem);
	void DeletAllMap();

	void AddJarItem(JarItem* jarItem, UInt8 color);

	bool GetBonusPool(UInt32 key, std::string cond, BonusVec& datas);
	bool GetBonusPool(UInt32 key, JarBonusDataEntry* data, BonusVec& datas);
	bool GetBonusPool(JarBonusColorKey key, JarBonusDataEntry* data, BonusVec& datas);

	//���ӱ�ת�ɶ�Ӧ��jaritem�б�
	bool GetBonusPool(JarBonusDataEntry* data, BonusVec& datas);

	void RejectJarItem(JarBonusDataEntry* data, BonusVec& inDatas, BonusVec& outDatas);
	void RejectJarItem(std::string cond, BonusVec& inDatas, BonusVec& outDatas);

	void RareDropItemReq(BonusVec& inDatas);

	UInt32	CheckOpenJar(JarBonusDataEntry* data, UInt32 comboBuyNum, UInt32& cost, OpActivityRegInfo* opAct);
	void GetOccuDatas(BonusVec& datas);

	void SyncPoolToDB(BonusVec& pool);
	void GetFakeWeightBase(BonusVec& pool, FakeWeightVec& fwPool);

	//���޻�û���
	void OnGetPoint(std::string reason, JarBonusDataEntry* data, UInt32 comboNum, UInt32& getPointId, UInt32& getPoint, UInt32& getPointCrit);
	UInt32 GetPointId(JarBonusDataEntry* data);
	UInt32	GetPointNum(JarBonusDataEntry* data);
	UInt32	GetPointCrit(JarBonusDataEntry* data);
	bool	IsResetPoint(bool isGold);
	
	//�״ο�ħ�޴���
	void OnFirstOpenMagJar();

	//��ȡ����װ�����ǿ���ȼ�
	UInt8 GetRandStrength(JarItemPoolData* data);

	//��ʼ��ħ�޼���
	void InitMagJarCounter();

	UInt32 CheckOpenMagBox(Item* item, JarBonusDataEntry* data, UInt8 isBatch, UInt32& costBoxNum, UInt32& costKeyNum);

	//�Ƿ��״ο�ħ��
	bool IsFirstOpenMagBox();

private: // ϵͳ����
	bool IsGoldJarPointOpen();
	bool IsMagicJarPointOpen();

	//���ͻ�õ��߹���
	void SendAnnounce(Item* item, Int32 num, std::string jarName);
	void SendAnnounce(UInt32 itemId, Int32 num, std::string jarName);

	void SendUdpLog(JarBonusDataEntry* data, UInt32 combo, ItemRewardVec itemRewards);

private: // �¼�����
	void OnGetNewItemEvent(IEventParam* param);
	void OnUpdateItemEvent(IEventParam* param);
	void OnLevelUp(IEventParam* param);

	//�Ƿ��Ѿ���ʼ��
	bool					m_inited;

	//ӵ����
	Player*					m_pOwner;

	//���ӽ���
	BonusPoolMap		bonusPool;				//������� �����Ľ���
	BonusPoolColorMap	bonusPoolColor;			//�������&Ʒ�� ��������		
	BonusVec			occuToDatas;			//data����

	//һ�ο��޵ý��
	std::vector<Item*> m_hitItems;
	std::vector<UInt32> m_hitJarItems;

	// �Ƿ����ڿ���ħ��
	bool					m_opening;
};


#endif