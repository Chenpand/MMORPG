function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindClearActivityDungeonEvent(601000)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(601001)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(601002)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(601003)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(602000)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(602001)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(602002)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(602003)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(603000)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(603001)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(603002)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearActivityDungeonEvent(603003)  -- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    local num = task:GetVar("parameter");           --- 获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
    if num >= 1 then
        task:SetStatus(2)                           ---完成任务修改状态
    end	
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end


function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
    local num = task:GetVar("parameter");           --- 获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
    num = num + 1;
    task:SetVar("parameter",num);                   --- 设置变量新值
    if num >= 1 then
        task:SetStatus(2)                           ---完成任务修改状态
    end
end


function CheckFinish(player, task)		---检查任务完成条件
end
