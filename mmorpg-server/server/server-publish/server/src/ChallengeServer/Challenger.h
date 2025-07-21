#ifndef _CHALLENGER_H_
#define _CHALLENGER_H_

#include <CLObject.h>

class CLRecordCallback;

/**
 *@brief ��ս��                                                                     
 */
class Challenger : public CLObject
{
public:
	Challenger();
	~Challenger();

public:
	/**
	 *@brief ���û�ȡgateId                                                                     
	 */
	void SetGateId(UInt32 gateId){ m_GateId = gateId; }
	UInt32 GetGateId() const{ return m_GateId; }

	/**
	 *@brief ���û�ȡ�ȼ�                                                                     
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief ���û�ȡְҵ                                                                     
	 */
	void SetOccu(UInt8 occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; }

	/**
	 *@brief ���û�ȡ����                                                                     
	 */
	void SetScore(UInt32 score){ m_Score = score; }
	UInt32 GetScore() const{ return m_Score; }

	/**
	 *@brief ����                                                                     
	 */
	void SetZoneId(UInt32 zoneId){ m_ZoneId = zoneId; }
	UInt32 GetZoneId() const{ return m_ZoneId; }

	/**
	 *@brief ���û�ȡս��                                                                     
	 */
	void SetPower(UInt32 power){ m_Power = power; }
	UInt32 GetPower() const{ return m_Power; }

	/**
	 *@brief ���û�ȡ����                                                                     
	 */
	void SetGroup(UInt8 group){ m_Group = group; }
	UInt8 GetGroup() const{ return m_Group; }

	/**
	 *@brief ����pk����                                                                     
	 */
	void SetPkData(const UInt8* pkdata, size_t pkdatalen);
	UInt8* GetPkData() { return m_pPkData; }
	size_t GetPkDataLen() const{ return m_PkDataLen; }

	/**
	 *@brief ������־λ                                                                     
	 */
	void SetAwardFlag(UInt32 flag){ m_AwardFlag |= flag; }
	bool TestAwardFlag(UInt32 flag) const { return (m_AwardFlag & flag) != 0; }
	UInt32 GetAwardFlag() const{ return m_AwardFlag; }

public://���ݿ����
	void InsertToDB();
	void SaveMe();

public:
	//gateid
	UInt32	m_GateId;

	//�ȼ�
	UInt16	m_Level;
	//ְҵ
	UInt8	m_Occu;
	//����
	UInt32	m_Score;
	//����
	UInt32	m_ZoneId;
	//ս��
	UInt32	m_Power;

	//����
	UInt8	m_Group;

	//������ȡ��־
	UInt32	m_AwardFlag;

	//pk����
	UInt8*	m_pPkData;
	//pk���ݳ���
	size_t	m_PkDataLen;
};

#endif
