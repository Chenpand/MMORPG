require "Debug"
require "Task_Define"

local needLv = 35;
local condition = 9;

--7日活动 Day2
--全身装备等级奖励
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindWearEquip();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    task:SetStatusNoSync(TASK_UNFINISH);	

    local minLv = player:GetAllEquipLvMin();
	if minLv >= needLv then
		task:SetVarNoSync("k",needLv);
		task:SetStatusNoSync(TASK_FINISHED);
	else
		task:SetVarNoSync("k",minLv);
	end 
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnWearEquip(player, task)		---	穿装备
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local minLv = player:GetAllEquipLvMin();
	if minLv >= needLv then
		task:SetVar("k",needLv);
		task:SetStatus(TASK_FINISHED);
	else
		task:SetVar("k",minLv);
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
			return
		end
	end

	OnWearEquip(player, task);
end