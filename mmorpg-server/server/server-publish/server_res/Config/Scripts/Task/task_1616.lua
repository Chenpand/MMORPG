function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindClearDungeonEvent(0)					-- 关注通关任何地下城
	task:BindClearActivityDungeonEvent(0)			-- 关注通关任何活动地下城
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
	local teamsize = player:GetTeamSize();	--获取组队人数
	if teamsize >= 2 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end


function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
	local teamsize = player:GetTeamSize();	--获取组队人数
	if teamsize >= 2 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end



function CheckFinish(player, task)		---检查任务完成条件
end
