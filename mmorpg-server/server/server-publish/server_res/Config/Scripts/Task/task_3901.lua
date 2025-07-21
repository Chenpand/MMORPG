function Init(task)				---绑定(注册)该任务需要关注的事件
	---关注获得死亡之塔事件。
	task:BindDeathTowerEvent(2003000, 20, 0)
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

function OnDeathTower(player, task, towerId, layer, usedTime)	--死亡之塔
	if layer < 20 then
		return;
	end
	
	task:SetStatus(2);	      			-- 满足条件时设置任务为完成状态
end


function CheckFinish(player, task)		---检查任务完成条件
end
