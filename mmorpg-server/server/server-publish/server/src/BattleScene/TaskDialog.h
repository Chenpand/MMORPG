#ifndef _TASK_DIALOG_H_
#define _TASK_DIALOG_H_

#include <CLTaskDefine.h>

class Player;

/**
 *@brief 任务对话框
 */
class TaskDialog
{
public:
	TaskDialog(Player* visitor, UInt8 type, ObjID_t id, UInt32 npcId, UInt32 taskId, UInt8 quality, UInt8 status = TASK_INIT);
	~TaskDialog();

public:
	/**
	 *@brief 添加谈话内容
	 */
	void AddWord(const char* word);

	/**
	 *@brief 增加选项
	 */
	void AddReply(const char* word);

	/**
	 *@brief 添加任务奖励
	 */
	void AddReward(){ m_bShowReward = true; }

	/**
	 *@brief 显示
	 */
	void Show();

	/**
	 *@brief 清除
	 */
	void Clear();

private:
	//当前访问者
	Player* m_pVisitor;
	//发布类型
	UInt8	m_PublishType;
	//发布者id
	ObjID_t	m_PublishId;
	//npc
	UInt32	m_NpcId;
	//相关任务
	UInt32	m_Task;
	//任务品质
	UInt8	m_Quality;
	//任务状态
	UInt8	m_Status;
	//内容
	std::vector<std::string> m_Words;
	//回复
	std::vector<std::string> m_Replies;
	//是否显示奖励
	bool	m_bShowReward;
};

#endif
