function Init(script)									---加载脚本时初始调用
end

function OnBirth(script, npc)							--出生的一些处理
end

function OnPublishTaskTalk(dialog, player, taskid)		---NPC发布任务时的对话
end

function OnUnfinishTaskTalk(dialog, player, taskid)		---未完成任务对话
end

function OnSubmitTaskTalk(dialog, player, taskid)		---提交任务对话
end

function OnFailedTaskTalk(dialog, player, taskid)		---失败任务对话
end

function OnNormalTalk(dialog, player)					---npc旁白对话
end

function OnClickOption(dialog,player,optionid)			---点击某个选项
end

function CheckPublishTask(player, taskid)				---检查是否发布除主支线外的其他任务
	return false;
end

function CheckSubmitTask(player, taskid)				---检查是否可提交除主支线外的其他任务
	return false;
end
