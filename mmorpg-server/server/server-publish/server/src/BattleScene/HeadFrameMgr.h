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

	// 登录
	void Online();
	// 补发
	void Compensate();
	// 心跳
	void OnHeartbeat(const Avalon::Time& now);
	// 加载数据
	void LoadHeadFrame(CLRecordCallback* callback);
	// 获得头像框
	void AddHeadFrame(UInt32 id, UInt32 expireTime);

	// 请求头像框列表
	void HandleHeadFrameReq();
	// 穿戴头像框
	void HandleUseHeadFrameReq(UInt32 id);
	// 新头像框通知
	void NotifyHeadFrame(UInt32 id, UInt32 expireTime, bool isGet);

private:

	// udp
	void _UdpHeadFrame(UInt32 opType, UInt32 id);
	// 存库
	void _InsertHeadFrame(UInt32 id, UInt32 expireTime);
	// 删除
	void _DelHeadFrame(UInt32 id);

private:

	// 拥有者
	Player* m_Owner;
	// 拥有的头像框
	std::map<UInt32, UInt32> m_HeadFrameMap;
};

#endif
