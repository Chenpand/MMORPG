require "Debug"
require "Task_Define"


local var_condition = 1 	--多少件
local needStrLv = 12 		--强化等级

--7日活动 Day4
--装备强化等级
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindDayChange();
    task:BindEquipStrenth();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    task:SetStatusNoSync(TASK_UNFINISH);

    local str = player:GetStrMaxLv(var_condition, needStrLv);
    if str >= needStrLv then
    	task:SetVarNoSync("k", needStrLv);
    	task:SetStatusNoSync(TASK_FINISHED);
    else
    	task:SetVarNoSync("k", str);
    end
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnEquipStrenth(player, task)
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local str = player:GetStrMaxLv(var_condition, needStrLv);
    if str >= needStrLv then
    	task:SetVar("k", needStrLv);
    	task:SetStatus(TASK_FINISHED);
    else
    	task:SetVar("k", str);
    end
end

function OnDayChange(player, task, preTask)		---	跨天
	local createFromNowOn = player:GetCreateFromNowOn()
	local day = createFromNowOn / (24 * 3600)

	--完成条件
	if day >= 3 then
		OnEquipStrenth(player, task)
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
