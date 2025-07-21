#ifndef _TCS_GRID_CFG_MGR_H_
#define _TCS_GRID_CFG_MGR_H_

#include <CLTeamCopyDefine.h>

/**
 *@brief �ű���������
 */
struct TCSGridCfg
{
	TCSGridCfg() : gridId(0), x(0), y(0), fieldId(0), type(TC_GRID_OBJ_NULL), block(false) {}

	UInt32 gridId;
	Int32 x;
	Int32 y;
	UInt32 fieldId;
	// ��������
	TCGridObjType type;
	// �Ƿ��赲
	bool block;
	// ����
	std::vector<UInt32> paramVec;
	std::vector<UInt32> paramVec2;
};

typedef std::map<UInt32, TCSGridCfg*> TCSGridCfgMap;
typedef std::map<TCGridPos, TCSGridCfg*> TCSGridCfgPosMap;

/**
 *@brief �ű��������ù���
 */
class TCSGridCfgMgr : public Avalon::Singleton<TCSGridCfgMgr>
{
public:
	TCSGridCfgMgr();
	~TCSGridCfgMgr();

	/**
	 *@brief ��ʼ����ͼ
	 */
	void InitCfg();

	/**
	 *@brief ���Ҹ���
	 */
	TCSGridCfg* FindGrid(UInt32 grade, UInt32 gridId);
	TCSGridCfg* FindGrid(UInt32 grade, const TCGridPos& pos);

	/**
	 *@brief ��ȡ�оݵ���б�
	 */
	TCSGridCfgMap& GetFieldMap(UInt32 grade) { return m_FieldMap[grade]; }

	/**
	 *@brief Ѱ·
	 */
	std::list<UInt32> SearchWay(UInt32 grade, UInt32 srcId, UInt32 destId, std::set<UInt32>& blockSet);
	std::list<UInt32> SearchWay(UInt32 grade, UInt32 srcX, UInt32 srcY, UInt32 destX, UInt32 destY, std::set<UInt32>& blockSet);
	
	/**
	 *@brief ��ȡ������Χ�ĸ���
	 */
	std::vector<UInt32> GetRoundGrid(UInt32 grade, UInt32 gridId, UInt32 roundNum = 1);

	/**
	 *@brief ���������Ƿ�����
	 */
	bool GridIsNear(UInt32 grade, UInt32 gridId1, UInt32 gridId2);

	/**
	 *@brief ��ȡ��������ĸ���
	 */
	void GetMonsterBornGrid(UInt32 grade, std::vector<UInt32>& monsterIndexVec, std::vector<UInt32>& bornVec);

private:

	// �Ѷ�->(key->����id)
	std::map<UInt32, TCSGridCfgMap> m_GridMap;

	//�Ѷ�->(��������->����)
	std::map<UInt32, TCSGridCfgPosMap> m_PosMap;

	// �оݵ�ĸ��� �Ѷ�->(key->����id)
	std::map<UInt32, TCSGridCfgMap> m_FieldMap;
};

#endif // !_TCS_GRID_CFG_MGR_H_

