function Init(task)				---绑定(注册)该任务需要关注的事件
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	task:SetStatus(2);				---	把任务的状态设成完成 ---0：初始、1：未完成、2：完成
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	完成任务时需要执行的操作
end

function CheckFinish(player, task)		---检查任务完成条件
end
