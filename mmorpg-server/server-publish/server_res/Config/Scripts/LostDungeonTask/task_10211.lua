function Init(task)				---绑定(注册)该任务需要关注的事件
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

function OnLostDungeonClearNum(player, task, num)
	task:SetVar("num", num);
	if num >= 2 then								---	判断结束条件
		task:SetStatus(2);
	end
	task:SyncData();
end

function OnLostDungeonFixClear(player, task, dungeonId, num)
end

function CheckFinish(player, task)		---检查任务完成条件
end
