function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindClearActivityDungeonEvent(2002000)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
	task:BindClearActivityDungeonEvent(2002001)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
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


function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
        local num = task:GetVar("parameter");   --- 获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
        num = num + 1;
        task:SetVar("parameter",num);               --- 设置变量新值
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
end


function CheckFinish(player, task)		---检查任务完成条件
end
