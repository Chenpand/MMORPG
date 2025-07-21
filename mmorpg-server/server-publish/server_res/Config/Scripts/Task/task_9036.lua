function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindClearDungeonEvent(0, 0, 180000)				-- 关注通关任何地下城
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


function OnClearDungeon(player, task, dungeonId, beated, areas)
	local num = task:GetVar("parameter");
		num = num + 1
		task:SetVar("parameter", num);					---	设置变量新值
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
end
