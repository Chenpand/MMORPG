#ifndef _CL_NEW_TITLE_H_
#define _CL_NEW_TITLE_H_

#include "CLNewTitleDefine.h"
#include "AvalonSimpleTimer.h"
#include <AvalonObjectPool.h>
#include <functional>

class NewTitleItemDataEntry;
class WSPlayer;
struct AccountInfo;

//玩家头衔类
class PlayerTitleRole : public PlayerTitle
{
	AVALON_DEFINE_OBJECTPOOL(PlayerTitle, Avalon::NullMutex)
public:
	PlayerTitleRole() {};
	~PlayerTitleRole() {};

public:
	virtual std::string GetTableName() { return "t_new_title"; }
	std::string JoinTitleName(NewTitleItemDataEntry& dataEntry);
private:
};

//玩家头衔类,账号通用
class PlayerTitleAccout : public PlayerTitle
{
public:
	PlayerTitleAccout() {};
	~PlayerTitleAccout() {};
public:
	std::string GetTableName() { return "t_new_title_accout"; }
	std::string JoinTitleName(NewTitleItemDataEntry& dataEntry);
private:
};

//玩家头衔管理类
class PlayerTitleMgr
{
	typedef HashMap<ObjID_t, PlayerTitle*> MapTitle;
public:
	PlayerTitleMgr();
	~PlayerTitleMgr();

public:
	//创建头衔对象
	static PlayerTitle* CreateTitle(UInt64 owenerId, NewTitleItemDataEntry* entry, UInt32 dueTime = 0);

public:
	//定时器
	void OnTick(const Avalon::Time &now);

	//加载玩家角色头衔数据
	virtual void LoadTitles() = 0;

	//加载数据返回
	void OnLoadDataRet(CLRecordCallback* callback);

	//添加一个头衔
	bool AddOneTitle(PlayerTitle* pTitle);

	//添加一个头衔
	virtual void AddOneTitle(NewTitleItemDataEntry& dataEntry);

	//获取一个头衔
	PlayerTitle* GetOneTitleByGuid(UInt64 titleGuid);

	//获取一个头衔
	PlayerTitle* GetOneTitleByDataId(UInt32 titleId);

	//移除一个头衔
	void RemoveOneTitleByDataId(UInt32 titleId);

	//移除一个头衔
	void RemoveOneTitleByGuid(UInt64 titleGuid);

	//遍历头衔
	void TraversalTitles(const std::function<bool(PlayerTitle&)>& ff);

	//头衔生成名称
	void GenerateAllTitlesName();

	//获取ownerId
	virtual UInt64 GetOwnerId() { return 0; };

	//获取拥有角色
	virtual WSPlayer* GetOwnerPlayer() { return NULL; };

	virtual UInt64 GetOwnerPlayerId() { return 0; };

	virtual void CheckNewTitlesOnLoaded() {};

	//日志前缀
	virtual std::string GetOwnerLogStr() { return "";}

	//获取指定头衔
	static PlayerTitle* GetOneTitle(WSPlayer* player, UInt64 titleGuid);

	//获取指定类型头衔
	static PlayerTitle* GetOneTitle(WSPlayer* player, UInt64 titleGuid, UInt8 titleOwnerType);

	//处理玩家穿戴头衔
	static void OnPlayerNewTitleTakeUpReq(WSPlayer* player, UInt64 titleGuid, UInt32 titleId);
	
	//处理玩家脱掉头衔
	static void OnPlayerNewTitleTakeOffReq(WSPlayer* player, UInt64 titleGuid, UInt32 titleId);

	//处理添加头衔(使用道具)
	static void OnPlayerAddNewTitle(WSPlayer* player, UInt32 titleId, UInt32 dueTm);

	//检查头衔更换
	static UInt32 CheckTitleTake(WSPlayer* player);

protected:
	//头衔对象
	MapTitle			m_titles;
	//定时器检查过期头衔
	Avalon::SimpleTimer			m_checkDueTitle;
};

//玩家角色头衔管理类
class PlayerTitleRoleMgr : public PlayerTitleMgr
{
public:
	PlayerTitleRoleMgr();
	~PlayerTitleRoleMgr();

public:
	//设置获取拥有者
	void SetOwner(WSPlayer* owner){ m_pOwner = owner; }
	WSPlayer* GetOwner() const{ return m_pOwner; }
	UInt64 GetOwnerId();
	std::string GetOwnerLogStr()
	{
		std::stringstream ss;
		ss << "role id : " << GetOwnerId();
		return ss.str();
	}

	//加载头衔数据
	void LoadTitles();

	//添加一个头衔
	void AddOneTitleByTitleId(UInt32 titleId, UInt32 dueTm = 0);

	WSPlayer* GetOwnerPlayer() { return m_pOwner; }

	UInt64 GetOwnerPlayerId();

	void CheckNewTitlesOnLoaded();

	void OnPlayerGuildPostChange(UInt8 oldPost, UInt8 newPost);

	//处理拜师成功
	void OnMakeMaster();

	//处理解除师傅
	void OnRemoveMaster();

	//处理师傅名字改变
	void OnMasterNameChange();

	//处理公会名字改变
	void OnGuidNameChange();

	//处理离开公会
	void OnGuidLeave();

	//处理玩家穿上公会头衔
	void OnPlayerNewTitleTakeUpGuildPostReq(WSPlayer* player);

public:
	//玩家上线
	void OnOnline();

protected:
	//拥有者
	WSPlayer*	m_pOwner;
};

//玩家账号头衔管理类
class PlayerTitleAccountMgr : public PlayerTitleMgr
{
public:
	PlayerTitleAccountMgr();
	~PlayerTitleAccountMgr();

public:
	//设置获取拥有者
	AccountInfo* GetOwner() const { return m_pOwner; }
	void SetOwner(AccountInfo* owner) { m_pOwner = owner; }
	UInt64 GetOwnerId();
	std::string GetOwnerLogStr()
	{
		std::stringstream ss;
		ss << "accid id : " << GetOwnerId();
		return ss.str();
	}

	//加载头衔数据
	void LoadTitles();

	//添加一个头衔
	void AddOneTitleByTitleId(UInt32 titleId, UInt32 dueTm = 0);

	WSPlayer* GetOwnerPlayer();

	UInt64 GetOwnerPlayerId();

	void CheckNewTitlesOnLoaded();

	//处理创建佣兵团
	void OnCreateYbt();

	//处理佣兵团名字改变
	void OnYbtNameChange();
protected:
	//拥有者
	AccountInfo* m_pOwner;
};
#endif