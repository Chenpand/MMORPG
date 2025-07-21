function Init(task)				---绑定(注册)该任务需要关注的事件
		task:BindKillNpcEvent(102, 204010)	 -- 关注杀死指定怪物事件
		task:BindKillNpcEvent(102, 204011)	 -- 关注杀死指定怪物事件

    task:BindClearDungeonEvent(204010)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
    task:BindClearDungeonEvent(204011)	-- 关注通关某个地下城事件（地下城，评价，时间/毫秒）
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

function OnKillNpc(player, task, npcid, npctype)        
    local npcnum = task:GetVar("parameter");            --- 获取变量
    if npcnum < 8 then                             --- 判断结束条件
        npcnum = npcnum + 1;                                --- 设定函数公式
        task:SetVar("parameter", npcnum);                   --- 设置变量新值
    end
end

function OnClearDungeon(player, task, dungeonId, beated, areas)
    local npcnum = task:GetVar("parameter");            --- 获取变量
    if npcnum >= 8 then                             --- 判断结束条件
        task:SetStatus(2);
    end
end


function CheckFinish(player, task)		---��������������
end
