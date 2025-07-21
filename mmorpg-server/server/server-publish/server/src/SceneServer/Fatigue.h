#ifndef _FATIGUE_H_
#define _FATIGUE_H_

#include <CLDefine.h>
#include <CLObjectProperty.h>
#include <CLOwnerObjectProperty.h>

class Player;

/**
*@brief ƣ��ֵ
*/
class Fatigue : public CLOwnerComplexProperty
{

public:
	Fatigue();
	~Fatigue() {};

public:

	void SetOwner(Player* owner);
	Player* GetOwner() const { return m_pOwner; }

	/**
	*@brief ��ȡƣ��ֵ����
	*/
	ObjUInt16& GetMaxFatigue() { return m_MaxFatigue; }

	/**
	*@brief ����ƣ��ֵ����
	*/
	void SetMaxFatigue(UInt16 num);

	/**
	*@brief ��ȡ�Ѿ�ʹ�õ�ƣ��ֵ
	*/
	UInt16 GetUseFatigue() { return m_UseFatigue; }

	/**
	*@brief ��ȡ���۵�ƣ��ֵ
	*/
	UInt32 GetLastFatigue();

	/**
	*@brief ��ȡ����ȱ�ٵ�ƣ��ֵ
	*/
	UInt16 GetLostFatigue() { return m_MaxFatigue - m_Fatigue; }

	/**
	*@brief ��ȡ��ǰƣ��ֵ
	*/
	ObjUInt16& GetFatigue() { return m_Fatigue; }

	/**
	*@brief ����ƣ��ֵ
	*/
	void AddFatigue(const char* reason, UInt16 fatigue, bool notify = false);

	/**
	*@brief ����ƣ��ֵ
	*/
	void RemoveFatigue(const char* reason, UInt16 fatigue);

	/**
	*@brief ˢ��ƣ��ֵ
	*/
	void RefreshFatigue(const Avalon::Time& now);

	/**
	*@brief ��ȡ���ˢ��ʱ��
	*/
	Avalon::Time GetLastRefreshTime() { return m_LastRefreshTime; }

public:
	void Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty);
	void Decode(Avalon::NetInputStream& stream);

	void DecodeString(std::istringstream& is);
	void EncodeString(std::ostringstream& os);

	void OnHeartbeat(const Avalon::Time& now, int tickType);

	void OnBirth();

	void OnOnline(bool bDayChanged);

private:

	//ӵ����
	Player* m_pOwner;

	//���ֵ
	ObjUInt16 m_MaxFatigue;

	//�Ѿ�ʹ�����ĵ�
	UInt16 m_UseFatigue;

	//����ӵ�е�
	ObjUInt16 m_Fatigue;

	//�ϴ�ˢ��ʱ��
	Avalon::Time m_LastRefreshTime;

};

#endif