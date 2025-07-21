#ifndef _CL_SKILL_PROTOCOL_H_
#define _CL_SKILL_PROTOCOL_H_

#include "CLProtocolDefine.h"
#include "CLSkillDefine.h"

namespace CLProtocol
{

	class SceneChangeSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_SKILLS_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & changeSkills;
		}

	public:

		// 技能配置类型
		UInt8 configType;

		//变化的技能列表
		std::vector<ChangeSkill> changeSkills;
	};

	class SceneChangeSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_CHANGE_SKILLS_RES)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		UInt32	result;
	};

	/**
	 *@brief 施放一个技能
	 */
	class SceneUseSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_SKILL_REQ)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillid & pos & target & timestamp;
		}
		
	public:
		//技能id
		UInt16		skillid;
		//位置
		CLPosition	pos;
		//objid
		ObjID_t		target;
		//时间戳
		UInt32		timestamp;
	};

	/**
	 *@brief 释放技能返回                                                                     
	 */
	class SceneUseSkillRet : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_USE_SKILL_RET)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & skillId & timestamp & result;
		}
		//技能id
		UInt32	skillId;
		//时间戳
		UInt32	timestamp;
		//结果  1成功 0失败
		UInt8	result;
	};

	//添加buff
	class SceneAddBuff : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_ADD_BUFF)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buffId;
		}
		//buffid
		UInt32	buffId;
	};

	//通知删除buff
	class SceneNotifyRemoveBuff : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REMOVE_BUFF_NOTIFY)
	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & buffId;
		}
		//buffid
		UInt32	buffId;
	};

	/**
	 *@brief client->scene 复活
	 */
	class SceneReviveReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_REVIVE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & type & index;
		}

	public:
		//复活类型 0为原地复活  ...
		UInt32	type;
		//复活点索引，目前部落战才有用，0 阵营复活点，1 的据点
		UInt8	index;
	};

	/**
	 *@brief server->client 通知死亡
	 */
	class SceneNotifyDeath : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_DEATH)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & reviveCost;
		}

	public:
		//原地复活替身娃娃消耗数量
		UInt32	reviveCost;
	};

	/**
	 *@brief client->server 切换pk模式
	 */
	class SceneSwitchPkMode : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SWITCH_PKMODE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & pkmode;
		}

	public:
		//pk模式
		UInt8	pkmode;
	};

	/** 
	 *@brief server->client 通知血量和法力变化
	 */
	class SceneNotifyHpMpChange : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_NOTIFY_HPMP_CHANGE)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & id & type & value;
		}

	public:
		//obj id
		ObjID_t	id;
		//类型 0为血量 1为法力
		UInt8	type;
		//值变化 正为加 1为减
		Int32	value;
	};

	/**
	*@brief client->server 请求初始化技能
	*/
	class SceneInitSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INIT_SKILLS_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType;
		}
		// 技能配置方案
		UInt32 configType;
	};

	/**
	 *@brief server->client 请求初始化技能返回
	 */
	class SceneInitSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_INIT_SKILLS_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		// 返回值
		UInt32 result;
	};

	/**
	*@brief client->server 请求推荐技能配置
	*/
	class SceneRecommendSkillsReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RECOMMEND_SKILLS_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType;
		}

	public:
		// 技能配置方案
		UInt32 configType;
	};

	/**
	*@brief server->client 请求推荐技能配置返回
	*/
	class SceneRecommendSkillsRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_RECOMMEND_SKILLS_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		// 返回值
		UInt32 result;
	};

	/**
	*@brief server->client 技能槽解锁
	*/
	class SceneSkillSlotUnlockNotify : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SKILL_SLOT_UNLOCK_NOTIFY)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & slot;
		}

	public:
		// 解锁槽位
		UInt32 slot;
	};

	/**
	*@brief client->server 设置技能页
	*/
	class SceneSetSkillPageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SKILL_PAGE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & page;
		}

	public:
		// 技能配置方案
		UInt32 configType;
		//设置的技能页
		UInt8 page;
	};

	/**
	*@brief server->client 设置技能页返回
	*/
	class SceneSetSkillPageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_SET_SKILL_PAGE_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & page & result;
		}

	public:
		// 技能配置方案
		UInt32 configType;
		//设置的技能页
		UInt8 page;
		// 返回值
		UInt32 result;
	};

	/**
	*@brief client->server 购买技能页
	*/
	class SceneBuySkillPageReq : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BUY_SKILL_PAGE_REQ)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & configType & page;
		}

	public:
		UInt32 configType;
		//购买的技能页
		UInt8 page;
	};

	/**
	*@brief server->client 购买技能页返回
	*/
	class SceneBuySkillPageRes : public Avalon::Protocol
	{
		AVALON_DEFINE_PROTOCOLID(SCENE_BUY_SKILL_PAGE_RES)

	public:
		AVALON_DEFINE_NETSERIALIZATION()
		{
			stream & result;
		}

	public:
		// 返回值
		UInt32 result;
	};

	
}

#endif
