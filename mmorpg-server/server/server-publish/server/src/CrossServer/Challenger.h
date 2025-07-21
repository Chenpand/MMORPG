#ifndef _CHALLENGER_H_
#define _CHALLENGER_H_

#include <CLObject.h>

class CLRecordCallback;

/**
 *@brief 挑战者                                                                     
 */
class Challenger : public CLObject
{
public:
	Challenger();
	~Challenger();

public:
	/**
	 *@brief 设置获取gateId                                                                     
	 */
	void SetGateId(UInt32 gateId){ m_GateId = gateId; }
	UInt32 GetGateId() const{ return m_GateId; }

	/**
	 *@brief 设置获取等级                                                                     
	 */
	void SetLevel(UInt16 level){ m_Level = level; }
	UInt16 GetLevel() const{ return m_Level; }

	/**
	 *@brief 设置获取职业                                                                     
	 */
	void SetOccu(UInt8 occu){ m_Occu = occu; }
	UInt8 GetOccu() const{ return m_Occu; }

	/**
	 *@brief 设置获取积分                                                                     
	 */
	void SetScore(UInt32 score){ m_Score = score; }
	UInt32 GetScore() const{ return m_Score; }

	/**
	 *@brief 区号                                                                     
	 */
	void SetZoneId(UInt32 zoneId){ m_ZoneId = zoneId; }
	UInt32 GetZoneId() const{ return m_ZoneId; }

	/**
	 *@brief 设置获取战力                                                                     
	 */
	void SetPower(UInt32 power){ m_Power = power; }
	UInt32 GetPower() const{ return m_Power; }

	/**
	 *@brief 设置获取分组                                                                     
	 */
	void SetGroup(UInt8 group){ m_Group = group; }
	UInt8 GetGroup() const{ return m_Group; }

	/**
	 *@brief 设置pk数据                                                                     
	 */
	void SetPkData(const UInt8* pkdata, size_t pkdatalen);
	UInt8* GetPkData() { return m_pPkData; }
	size_t GetPkDataLen() const{ return m_PkDataLen; }

	/**
	 *@brief 奖励标志位                                                                     
	 */
	void SetAwardFlag(UInt32 flag){ m_AwardFlag |= flag; }
	bool TestAwardFlag(UInt32 flag) const { return (m_AwardFlag & flag) != 0; }
	UInt32 GetAwardFlag() const{ return m_AwardFlag; }

public://数据库相关
	void InsertToDB();
	void SaveMe();

public:
	//gateid
	UInt32	m_GateId;

	//等级
	UInt16	m_Level;
	//职业
	UInt8	m_Occu;
	//积分
	UInt32	m_Score;
	//区号
	UInt32	m_ZoneId;
	//战力
	UInt32	m_Power;

	//分组
	UInt8	m_Group;

	//奖励领取标志
	UInt32	m_AwardFlag;

	//pk数据
	UInt8*	m_pPkData;
	//pk数据长度
	size_t	m_PkDataLen;
};

#endif
