#ifndef _ACTIVITY_MGR_H_
#define _ACTIVITY_MGR_H_

#include <AvalonSingleton.h>
#include <CLGameDefine.h>
#include <map>
#include <vector>
#include <string>
#include <CLActivityDefine.h>
#include <AvalonSimpleTimer.h>

/**
*@brief �
*/
class GSActivityMgr : public Avalon::Singleton<GSActivityMgr>
{
public:
	GSActivityMgr();
	~GSActivityMgr();

	void OnTick(const Avalon::Time& now);

	/**
	*@brief ������Ӫ�����
	*/
	void OnLoadOpActData(OpActRegMap& activeData);

	/**
	*@brief ͬ����ǰ��Ӫ�״̬
	*/
	void OnOpActStateChange(OpActivityRegInfo& info);

	/**
	*@brief ͨ��ģ�����ͻ�ȡ��Ӧ��������Ӫ�
	*/
	std::vector<OpActivityRegInfo*> GetOpActDataByTmpType(OpActivityType type, bool inActivity = true) const;

public:
	/**
	*@brief ��ȡ���е�ԤԼְҵ
	*/
	void CountAppointmentOccus();

	/**
	*@brief �жϸ�ʱ�䴴����ְҵ�Ƿ��Ǵ���ԤԼ���
	*/
	bool IsOccuInAppointmentOccuActivity(UInt8 occu, UInt32 createTime) const;

	/**
	*@brief ��ȡ���е�ԤԼְҵ
	*/
	const std::vector<UInt8>& GetAppointmentOccus() const { return m_AppointmentOccus; }

	/**
	*@brief ����ԤԼְҵ
	*/
	void SetAppointmentOccus(const std::vector<UInt8>& occus) { m_AppointmentOccus = occus; }

	/**
	*@brief �Ƿ���ԤԼְҵ
	*/
	bool IsAppointmentOccu(UInt8 occu) const { return std::find(m_AppointmentOccus.begin(), m_AppointmentOccus.end(), occu) != m_AppointmentOccus.end(); }

	/**
	*@brief ��ȡ���ԤԼ��ɫ����
	*/
	UInt32 GetMaxAppointmentRoleNum();

private:
	// ��ʱ��
	Avalon::SimpleTimer						m_Timer;
	// ��Ӫ��б�
	std::map<UInt32, OpActivityRegInfo*>	m_opRegInfos;
	// ��ԤԼ��ְҵ
	std::vector<UInt8>						m_AppointmentOccus;
};

#endif