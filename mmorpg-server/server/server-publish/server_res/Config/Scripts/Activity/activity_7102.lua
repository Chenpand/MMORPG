require "Debug"
require "Task_Define"

 local condition = 30
--7日活动 Day1
--充值奖励
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindDayChange();
    task:BindCharge();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	task:SetStatusNoSync(TASK_UNFINISH);
	RefreshCharge(player, task);
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnCharge(player, task, value)		---	首充
	if task:GetStatus() ~= TASK_UNFINISH then return false end 
	
	RefreshCharge(player, task);
end

function RefreshCharge(player, task)
	local chargeTotal = player:GetPlayerChargeTotal();
	if chargeTotal >= condition then
		task:SetStatus(TASK_FINISHED);
		task:SetVar("k",condition);
	else
		task:SetVar("k",chargeTotal);
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