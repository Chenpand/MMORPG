function Init(task)
	task:BindGetItemEvent(400000001);
    task:BindKillNpcEvent(803, 203011)   -- 关注杀死指定�?物事�?
    task:BindKillNpcEvent(803, 203012)   -- 关注杀死指定�?物事�?
end

function CheckAccept(player)
	return true;
end

function OnAccepted(player, task)
end

function OnSubmitted(player, task)
	local itemnum = task:GetVar("parameter");
		if(itemnum >= 1 ) then
			task:SetStatus(4);
			player:RemoveItem(task:GetRewardsReason(), 400000001, 1);
		end
end

function OnGetItem(player, task, itemid, num)	
	local itemnum = task:GetVar("parameter");
	if itemid == 400000001 then 
		itemnum = itemnum + num;
		task:SetVar("parameter",itemnum);
	end
	
	if(itemnum >= 1) then
		task:SetStatus(2);
	end

end

function OnAbandomed(player, task)
	local itemnum = task:GetVar("parameter");
		if(itemnum >= 1 ) then
			player:RemoveItem(task:GetRewardsReason(), 400000001, 1);
		end
end

function OnKillNpc(player, task, npcid, npctype)        --- 任务过程�?杀死NPC时需要执行的操作
    local itemnum = task:GetVar("parameter");
    if itemnum < 1 then
        task:AddTaskItem(400000001, 100);                   -- 添加任务道具
    end
end