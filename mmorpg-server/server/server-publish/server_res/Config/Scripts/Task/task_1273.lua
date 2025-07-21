function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindSubmitTaskSucceedEvent(1203) 	-- 关注任务完成事件
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作	
	local task = player:FindTask(1273);
	if player:CheckTaskFinished(1203) then
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnSubmitTaskSucceed(player, task, taskid)		---	完成其他任务时需要执行的操作
	if player:CheckTaskFinished(1203) then
		task:SetStatus(2);
	end
end