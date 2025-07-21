require "Debug"
require "Task_Define"


--升级活动任务
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindLevelUpEvent();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    task:SetStatusNoSync(TASK_UNFINISH);
    task:SetVarNoSync("lv", player:GetLevel());	
    OnLvUpChange(player, task);
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnLvUpChange(player, task)		---	升级
	if task:GetStatus() ~= TASK_UNFINISH then return false end 
	
	local lv = player:GetLevel();
	if lv >= 10 then
		task:SetStatus(TASK_FINISHED);
	end 
end
