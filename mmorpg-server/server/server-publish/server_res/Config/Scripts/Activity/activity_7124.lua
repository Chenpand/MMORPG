require "Debug"
require "Task_Define"

local condition = 25;
--7日活动 Day1
--升级奖励
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindLevelUpEvent();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    task:SetStatusNoSync(TASK_UNFINISH);	

    local lv = player:GetLevel();
	if condition <= lv then
		task:SetVarNoSync("k",condition);
		task:SetStatus(TASK_FINISHED);
	else
		task:SetVarNoSync("k",lv);
	end
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnLvUpChange(player, task)		---	升级
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local lv = player:GetLevel();
	if condition <= lv then
		task:SetVar("k",condition);
		task:SetStatus(TASK_FINISHED);
	else
		task:SetVar("k",lv);
	end 
end

function OnDayChange(player, task, preTask)		---	跨天
	local createFromNowOn = player:GetCreateFromNowOn()
	local day = createFromNowOn / (24 * 3600)

	--创角超过10天结束任务
	if day > 10 then
		task:SetStatus(TASK_FAILED);
		return
	end

	--创角超过7天只能领取不能再做
	if day > 7 then
		if task:GetStatus() ~= TASK_FINISHED and
			task:GetStatus() ~= TASK_OVER then
			task:SetStatus(TASK_FAILED);
		end
	end
end