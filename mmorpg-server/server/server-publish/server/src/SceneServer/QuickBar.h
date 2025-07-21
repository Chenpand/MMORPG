#ifndef _QUICKBAR_H_
#define _QUICKBAR_H_

#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>
#include <CLSettingDefine.h>
#include <CLErrorCode.h>
#include <CLRecordClient.h>
#include <CLSkillDefine.h>
#include <map>

class Player;
class SkillMgr;
class PlayerSkillMgr;

/**
 *@brief �����  by huchenhui
 */
class QuickBar : public CLOwnerComplexProperty
{
public:
	QuickBar() {};
	virtual ~QuickBar() {};

	void SetOwner(Player* player) { m_Player = player; }
	Player* GetOwner() { return m_Player; }

	virtual void Init() = 0;
	virtual void OnLevelup() = 0;


public: //�����

	virtual void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty) = 0;

	virtual void Decode(Avalon::NetInputStream& stream) = 0;

	virtual void DecodeString(std::istringstream& is) = 0;

	virtual void EncodeString(std::ostringstream& os) = 0;

protected:
	//��Ҷ���
	Player* m_Player;
};


/**
*@brief ������
*/
class SkillBar : public QuickBar
{
public:
	typedef std::vector<SkillBarInfo> SkillBarVec;
	typedef std::vector<UInt16> SkillIdsVec;

public:
	SkillBar();
	virtual ~SkillBar();

protected:
	/**
	*@brief ��鼼�ܷ���
	*/
	UInt32 CheckBars(SkillBarVec skillBars);

	/**
	*@brief ���ĳҳ���ܷ���
	*/
	UInt32 CheckBars(SkillBarVec skillBars, UInt32 page);

	/**
	*@brief ��鼼����
	*/
	UInt32 CheckBar(SkillBarInfo skillBar);

	/**
	*@brief ��ȡʵ�ʴ��ڵ�SkillBars
	*/
	SkillBarVec GetExistSkillBar();

	/**
	*@brief ����ʵ�ʴ��ڵ�SkillBars
	*/
	void SetExistSkillBar(SkillBarVec skillBars);

	/**
	*@brief ��ȡ���еĸ���
	*/
	UInt32 GetFreeGridSize();



public:
	/**
	*@brief �Ƿ���ڼ���
	*/
	bool IsExist(UInt16 id);

	/**
	*@brief ��ȡ���ܵĲ�λ
	*/
	UInt8 GetSlot(UInt16 id);

	/**
	*@brief ���õ�ǰ�����Ĳ�λ
	*/
	void SetGrid(UInt8 slot, UInt16 id);

	/**
	*@brief ���÷���
	*/
	void SetBarIndex(UInt32 barIndex);

	/**
	*@brief ��÷�����ǰҳ
	*/
	inline UInt32 GetBarIndex() { return m_BarIndex; }


	/**
	*@brief ���ü�����
	*/
	UInt32 PushGrid(UInt8 barIndex, SkillIdsVec ids);

	/**
	*@brief ���ü�����
	*/
	//UInt32 PushGrid(SkillIdsVec ids);

	/**
	*@brief ���ü��ܵ��̶��ĸ���
	*/
	UInt32 PushFixGrid(UInt8 barIndex, SkillGridVec skillGirdVec);

	/**
	*@brief ����Ƽ����ú����м��ܸ���
	*/
	bool CheckRecommendGrid(UInt8 barIndex, SkillGridVec skillGirdVec);

	/**
	*@brief �����
	*/
	void ClearEntry(UInt16 id);

	/**
	*@brief ���������
	*/
	void ClearAllEntry(UInt32 page);

	/**
	*@brief ��ȡ���ܹ���
	*/
	PlayerSkillMgr& GetSkillMgr();

	/**
	*@brief ���bar����
	*/
	UInt32 GetMaxBarSize() { return m_MaxBarSize; }

public:
	/**
	*@brief ����
	*/
	UInt32 SetBars(UInt8 index, std::vector<SkillBarInfo> tbars, bool isContinue = false);


	/**
	*@brief �������͵Ŀ���
	*/
	void CopyWithOutConfig(const SkillBar & rsh);


	/**
	*@brief ����Ƿ�����
	*/
	void ClearInvaildPvpBar();
public:

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);

	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);

	void EncodeString(std::ostringstream& os);

public:
	using CLOwnerComplexProperty::Init;
	virtual void Init();
	virtual void OnLevelup();
	virtual void ClearNotValidEntry(UInt32 page);

protected:

	//һ��bar��������
	UInt32 m_MaxGridSize;

	//���bar����
	UInt32 m_MaxBarSize;

	//��ǰ����
	UInt32	m_BarIndex;

	//���ܷ���
	SkillBarVec m_SkillBars;

	//��������
	SkillConfigType m_SkillConfigType;

};

class PvpSkillBar : public SkillBar
{
public:
	PvpSkillBar():SkillBar() { m_SkillConfigType = SkillConfigType::SKILL_CONFIG_PVP; }
	~PvpSkillBar() { }
};


class EqualPvpSkillBar : public SkillBar
{
public:
	EqualPvpSkillBar() :SkillBar() { m_SkillConfigType = SkillConfigType::SKILL_CONFIG_EQUAL_PVP; }
	~EqualPvpSkillBar() { }
};



class ItemBar
{
public:
	ItemBar()
	{
	};
	virtual ~ItemBar() {};

private:


public:


private:


};

class WeaponBar : public QuickBar
{
public:
	WeaponBar();

	virtual ~WeaponBar();

public:
	using CLOwnerComplexProperty::Init;
	virtual void Init() {}
	virtual void OnLevelup() {}

public:

	bool IsInvalid();

	bool IsExist(ObjID_t id);

	UInt32 SetBarById(ObjID_t oldId, ObjID_t newId);

	UInt32 SetBarByIndex(UInt32 index, ObjID_t newId);

	UInt64 GetBarByIndex(UInt32 index);

public:

	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);

	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);

	void EncodeString(std::ostringstream& os);

private:

	//���bar����
	UInt32 m_MaxGridSize;

	std::vector<ObjID_t> m_Weapons;
};

#endif
