function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindClearDungeonEvent(502000)	-- 关注通关某个地下城事件
    task:BindClearDungeonEvent(502001)	-- 关注通关某个地下城事件
    task:BindClearDungeonEvent(502002)	-- 关注通关某个地下城事件
    task:BindClearDungeonEvent(502003)	-- 关注通关某个地下城事件
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
	if(beated < 20) then   -- 检查通关时被击次数以及通过房间数是否满足要求
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end
