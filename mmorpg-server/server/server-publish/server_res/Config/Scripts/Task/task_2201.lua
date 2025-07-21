function Init(task)	
	task:BindKillNpcEvent(101, 101000)
    task:BindClearDungeonEvent(101000)	

	
end

function CheckAccept(player)
	return true;
end

function OnAccepted(player, task)
end

function OnSubmitted(player, task)
end

function OnAbandomed(player, task)
end

function OnKillNpc(player, task, npcid, npctype)
    local npcnum = task:GetVar("parameter");            --- 获取变量
    if npcnum < 5 then                             --- 判断结束条件
        npcnum = npcnum + 1;                                --- 设定函数公式
        task:SetVar("parameter", npcnum);                   --- 设置变量新值
    end
end

function OnClearDungeon(player, task, dungeonId, beated, areas)
    local npcnum = task:GetVar("parameter");            --- 获取变量
    if npcnum >= 5 then                             --- 判断结束条件
        task:SetStatus(2);
    end
end

function CheckFinish(player, task)		---��������������
end
