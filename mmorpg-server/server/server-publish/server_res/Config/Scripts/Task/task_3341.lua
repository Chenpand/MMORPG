require "Debug"

function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindClearDungeonEvent(301003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(302003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(303003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(304003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(305003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(306003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(307003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(308003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(309003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(310003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
    task:BindClearDungeonEvent(311003)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒, 复活次数）
	
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
	print("OnClearDungeon");
	local prefixKey="index";
	local sum = 0;
	local x = dungeonId / 1000 - 300;
	local key = tostring(prefixKey)..tostring(x);
	local num = task:GetVar(key);
	if num == 0 then
		task:SetVar(key, 1);
	end

	for i=1, 11 do
		key = tostring(prefixKey)..tostring(i);
		sum = sum + task:GetVar(key);
	end
	
	task:SetVar("parameter", sum);

	if sum >= 11 then
		task:SetStatus(2);
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
