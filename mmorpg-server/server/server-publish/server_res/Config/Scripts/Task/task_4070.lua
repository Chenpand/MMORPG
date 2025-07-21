function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindClearDungeonEventByRevive(305000, 1); --关注通关某个地下城(地下城ID, 限定复活次数)(0表示不限定, N表示可以复活N-1次)
	task:BindClearDungeonEventByRevive(305001, 1); --关注通关某个地下城(地下城ID, 限定复活次数)(0表示不限定, N表示可以复活N-1次)
	task:BindClearDungeonEventByRevive(305002, 1); --关注通关某个地下城(地下城ID, 限定复活次数)(0表示不限定, N表示可以复活N-1次)
	task:BindClearDungeonEventByRevive(305003, 1); --关注通关某个地下城(地下城ID, 限定复活次数)(0表示不限定, N表示可以复活N-1次)
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
	local num = task:GetVar("parameter");			---	获取变量
	num = num + 1;									---	设定函数公式
	task:SetVar("parameter", num);				---	设置变量新值
	task:SetStatus(2);							-- 满足条件时设置任务为完成状态
end


function CheckFinish(player, task)		---检查任务完成条件
end
