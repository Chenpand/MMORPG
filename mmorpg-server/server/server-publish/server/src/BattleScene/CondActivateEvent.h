#ifndef _ACTIVATE_COND_CHECK_EVENT_H_
#define _ACTIVATE_COND_CHECK_EVENT_H_

#include <CLMallDefine.h>
#include <CLMallPersonalTailorDataEntry.h>


class Player;
class Item;

/**
*@brief �̳�˽�˶��ƴ�������
*/
enum MallPersonalTailorActiavateCond
{
	// �Ƿ�
	MPTAC_INVALID,
	// �ȼ��ﵽ50����Я����ɫ�������̩��������ķ�ɫ�������г�ֵ��û������ƿ�ʯ�������ƿ�ʯ����
	MPTAC_ONE,
	// �ȼ��ﵽ50����Я����ɫ��ָ���̩����ָ��ķ�ɫ��ָ���г�ֵ��û������ƿ�ʯ�������ƿ�ʯ����
	MPTAC_TWO,
	// �ȼ��ﵽ50����Я����ɫ�������̩��������ķ�ɫ�������г�ֵ��û������ƿ�ʯ�������ƿ�ʯ����
	MPTAC_THREE,
	// �ȼ��ﵽ50����Я����ɫ������̩��������ķ�ɫ�����г�ֵ��û������ƿ�ʯ�������ƿ�ʯ����
	MPTAC_FOUR,
	// �ۼƳ�ֵ�ﵽ30Ԫ~200Ԫ��δӵ��ħ�޳ƺ��Լ��̳��ƺţ���������ʲ�����־�ƺŹ���
	MPTAC_FIVE,
	// ȫ��ǿ����8��������ɫ���岻��100����������ɫ���幺��
	MPTAC_SIX,
	// ȫ��ǿ����10��������ɫ���岻��50����������ɫ���幺��
	MPTAC_SEVEN,
	// ȫ��ǿ����8�������󶨽�Ҳ���1�򣬵��������
	MPTAC_EIGHT,
	// �ȼ��ﵽ30����û�г�ֵ��¼��������ҹ���
	MPTAC_NINE,
	// �ȼ��ﵽ35����һ��ʱװ��û�У�����ʱװ����
	MPTAC_TEN,
};

/**
*@brief ��������
*/
enum NumLimit
{
	// ����������
	NUM_LIMIT_NONE = 0,
	// ��������50��
	NUM_LIMIT_FIFTY = 50,
	// ��������100��
	NUM_LIMIT_ONE_HUNDRED = 100,
	// ��������1���
	NUM_LIMIT_TEN_THOUNSAND = 10000,
};

/**
*@brief ��ҵȼ�����
*/
enum PlayerLevelLimiy
{
	// �����Ƶȼ�
	PLL_NONE = 0,
	// ����50��
	PLL_FIFTY = 50,
};

/**
*@brief ǿ���ȼ�����
*/
enum StrengthLevelLimit
{
	// ������ǿ���ȼ�
	SLL_NONE = 0,
	// ǿ���ȼ���������8��
	SLL_EIGHT = 8,
	// ǿ���ȼ���������10��
	SLL_TEN = 10,
};

/**
*@brief ��������������
*/
enum CondActivateSubType
{
	// ȫ�������ж�һ��
	CAST_ALL = 0,
	// ������ҵȼ��ж�
	CAST_PLAYER_LEVEL,
	// ���ݵ���ǿ���ȼ��ж�
	CAST_ITEM_STRENGEN_LEVEL,
	// ���ݳ�ֵ����ж�
	CAST_CHARGE_NUM,
};

/**
*@brief ��ֵ��������
*/
#define CHARGE_LEFT_BOUNDARY_VALUE 30
#define CHARGE_RIGHT_BOUNDARY_VALUE 200


/**
*@brief �����������
*/
class ActivateCondCheckEvent : public Avalon::Singleton<ActivateCondCheckEvent>
{
public:
	ActivateCondCheckEvent();
	~ActivateCondCheckEvent();

public:
	/**
	*@brief ��ʼ��
	*/
	void Init();

	/**
	*@brief ����һ������
	*/
	void OnActivate(Player* player, ConditionActivateType condType, CondActivateSubType subType);

	/**
	*@brief ���󴥷�
	*/
	void RequestActivate(UInt64 roleId, UInt8 cond, UInt8 type);

	/**
	*@brief ��ȡ����������װ������
	*/
	UInt16 GetEquipNumQualified(Player* player, ItemSubType itemSubType);

private:
	/**
	*@brief ����˽�˶���
	*/
	void _OnActivatePersonalTailor(Player* player, CondActivateSubType subType);

	/**
	*@brief ��÷��������ĳƺ�����
	*/
	UInt16 _GetTitleNumQualified(Player* player);

	/**
	*@brief ��÷���������ʱװ����
	*/
	UInt16 _GetFashonNumQualified(Player* player);

	/**
	*@brief ��÷�����������ǿ���ĵ�������
	*/
	UInt16 _GetStrengenEquipNumQualified(Player* player, UInt8 strengthLevel);

	/**
	*@brief ��ð����еĵ���
	*/
	void _GetItemFormPackage(Player* player, PackType packType, std::vector<Item*>& items);


private:
	// ����װ��
	std::vector<UInt32>		m_LimitEquips;

	// ���Ƴƺ�
	std::vector<UInt32>		m_LimitTitles;

	// ˽�˶��Ʊ�����
	std::vector<MallPersonalTailorDataEntry*> m_PersonalTailorTableDatas;
};

#endif