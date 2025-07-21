#ifndef _TCS_TEAM_CHECK_H_
#define _TCS_TEAM_CHECK_H_

#include <CLTeamCopyDefine.h>

/**
*@brief ���鿪ս���
*/
class TCSTeamCheck
{
public:
	TCSTeamCheck();
	~TCSTeamCheck();

	/**
	*@brief ����id
	*/
	inline void SetTeamId(UInt32 id) { m_TeamId = id; }
	inline UInt32 GetTeamId() { return m_TeamId; }

	/**
	*@brief �������
	*/
	const std::set<UInt64>& GetCheckPlayerSet() { return m_PlayerSet; }

	/**
	*@brief �Ƿ�ȫ�����ok
	*/
	bool IsAllCheck();

	/**
	*@brief �������б�
	*/
	void JoinCheck(UInt64 roleId);

	/**
	*@brief ɾ��������
	*/
	void RemoveCheck(UInt64 roleId);

private:

	// ����id
	UInt32 m_TeamId;
	// ��������б�
	std::set<UInt64> m_PlayerSet;
};

#endif
