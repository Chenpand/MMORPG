function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindClearDungeonEvent(208000, 0, 180000, 0)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(208001, 0, 180000, 0)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(208002, 0, 180000, 0)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(208003, 0, 180000, 0)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
	
	task:BindClearDungeonEventByRevive(208003, 10); --关注通关某个地下城(地下城ID, 限定复活次数)(0表示不限定, N表示可以复活N-1次)
	task:BindClearDungeonEvent(208003, 0, 0 10); --关注通关某个地下城,限定复活次数
	
	task:BindClearDungeonEvent(0, 0, 0)				-- 关注通关任何地下城
	task:BindClearDungeonEvent(0)					-- 关注通关任何地下城
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
	local dungeonType = dungeonId % 10;		---获取地下城难度(0:普通,1:冒险,2:勇士,3:王者,4:地狱)
	--if dungeonType >= 3 then
		--task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	--end
	
	local teamsize = player:GetTeamSize();	--获取组队人数
	--if teamsize >= 2 then
		--task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	--end
	
	if(beated < 10 and areas >= 2) then   -- 检查通关时被击次数以及通过房间数是否满足要求
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
