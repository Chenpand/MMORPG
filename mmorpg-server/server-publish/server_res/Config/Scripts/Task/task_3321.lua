function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindSubmitTaskSucceedEvent(1523) 	-- 关注杀死指定怪物事件
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作	
    local num = task:GetVar("parameter");
	if player:CheckTaskFinished(1523) then
	    num = num + 1;
        task:SetVar("parameter",num);
    	task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnSubmitTaskSucceed(player, task, taskid)		---	完成其他任务时需要执行的操作
    local num = task:GetVar("parameter");
	if player:CheckTaskFinished(1523) then
        num = num + 1;
        task:SetVar("parameter",num);
		task:SetStatus(2);
	end
end

function CheckFinish(player, task)		---检查任务完成条件
end
