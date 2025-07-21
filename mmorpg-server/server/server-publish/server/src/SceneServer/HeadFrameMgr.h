#ifndef _HEAD_FRAME_MGR_H_
#define _HEAD_FRAME_MGR_H_

#include <CLDefine.h>

class Player;
class CLRecordCallback;

class HeadFrameMgr
{
public:
	HeadFrameMgr();
	~HeadFrameMgr();

	void SetOwner(Player* player) { m_Owner = player; }
	Player* GetOwner() { return m_Owner; }

	// ��¼
	void Online();
	// ����
	void Compensate();
	// ����
	void OnHeartbeat(const Avalon::Time& now);
	// ��������
	void LoadHeadFrame(CLRecordCallback* callback);
	// ���ͷ���
	void AddHeadFrame(UInt32 id, UInt32 expireTime);

	// ����ͷ����б�
	void HandleHeadFrameReq();
	// ����ͷ���
	void HandleUseHeadFrameReq(UInt32 id);
	// ��ͷ���֪ͨ
	void NotifyHeadFrame(UInt32 id, UInt32 expireTime, bool isGet);

private:

	// udp
	void _UdpHeadFrame(UInt32 opType, UInt32 id);
	// ���
	void _InsertHeadFrame(UInt32 id, UInt32 expireTime);
	// ɾ��
	void _DelHeadFrame(UInt32 id);

private:

	// ӵ����
	Player* m_Owner;
	// ӵ�е�ͷ���
	std::map<UInt32, UInt32> m_HeadFrameMap;
};

#endif
