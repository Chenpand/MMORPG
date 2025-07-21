function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindChangeOccuEvent();
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作	
	if player:GetOccu()%10 ~= 0 then
		task:SetVar("parameter", 1);                   --- 设置变量新值
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnChangeOccu(player, task)		--转职
	task:SetVar("parameter", 1);                   --- 设置变量新值
	task:SetStatus(2);
end

function CheckFinish(player, task)		---检查任务完成条件
end
