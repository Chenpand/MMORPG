function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindAddRetinueEvent(0);
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnAddRetinue(player, task, quality)	--获得随从
	if quality < 2 then
		return;
	end
	
	task:SetStatus(2);
end


function CheckFinish(player, task)		---检查任务完成条件
end
