require "Debug"
require "Task_Define"

local var_condition = 50
--7日活动 Day5
--随从累计洗炼
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindDayChange();
    task:BindRetinueChangeSkill();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    task:SetStatusNoSync(TASK_UNFINISH); 

    local num = player:GetChangeSkillTotal();
    if num >= var_condition then
    	task:SetVarNoSync("k", var_condition);
    	task:SetStatusNoSync(TASK_FINISHED);
    else
    	task:SetVarNoSync("k", num);
    end	
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnRetinueChangeSkill(player, task, skillId, skillLv)
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local num = player:GetChangeSkillTotal();
    if num >= var_condition then
    	task:SetVar("k", var_condition);
    	task:SetStatus(TASK_FINISHED);
    else
    	task:SetVar("k", num);
    end
end

function OnDayChange(player, task, preTask)		---	跨天
	local createFromNowOn = player:GetCreateFromNowOn()
	local day = createFromNowOn / (24 * 3600)

	--真正接受任务
	if day >= 4 then
		OnRetinueChangeSkill(player, task, 0, 0)
	end

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
