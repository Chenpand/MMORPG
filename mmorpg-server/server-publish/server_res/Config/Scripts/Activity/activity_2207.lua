require "Debug"
require "Task_Define"


--累计在线活动任务
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindTickEvent();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    task:SetStatusNoSync(TASK_UNFINISH);	
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnMinTick(player, task)		---	5秒tick
	if task:GetStatus() ~= TASK_UNFINISH then return end 
	
	if player:GetDayOnlineTime() >= 1 * 60 * 60 + 1800 then
		task:SetStatus(TASK_FINISHED);
	end 
end

function OnDayChange(player, task, preTask, online)		---	跨天
	task:SetStatus(TASK_UNFINISH);
end