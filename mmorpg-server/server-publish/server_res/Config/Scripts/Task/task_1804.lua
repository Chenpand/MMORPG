function Init(task)				---绑定(注册)该任务需要关注的事件
	---关注获得道具事件。 (1.金币,2.点券,3.经验,11.死亡币, 12.决斗币)
	task:BindChangeMoneyEvent(1)            ---金币
    task:BindGetItemEvent(360000002)		---关注获得道具事件。  破空石
    task:BindGetItemEvent(300000105)		---关注获得道具事件。  有色晶体
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function CheckTask(item1, item2, item3)
    if item1 >= 150 and item2 >= 1000 and item3 >= 150000 then
        return true;
    end
    return false;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    local item1 = player:GetItemNum(360000002);
    task:SetVar("p1", item1);
    local item2 = player:GetItemNum(300000105);
    task:SetVar("p2", item2);
    local item3 = player:GetGold();
    task:SetVar("p3", item3);
    if CheckTask(item1, item2, item3) then
        task:SetStatus(2);
    end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
    local item1 = task:GetVar("p1");	---	得到变量值
    local item2 = task:GetVar("p2");	---	得到变量值
    local item3 = task:GetVar("p3");	---	得到变量值

    if CheckTask(item1, item2, item3) then
        task:SetStatus(4);
        player:RemoveItem(task:GetRewardsReason(), 360000002, 150);        ---	删除变量值的道具
        player:RemoveItem(task:GetRewardsReason(), 300000105, 1000);        ---	删除变量值的道具
        player:RemoveGold(task:GetRewardsReason(), 150000)
    end
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnChangeMoney(player, task, moneyType, num)	--货币变化
    
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = player:GetGold();
    task:SetVar("p3", item3);

	if CheckTask(item1, item2, item3) then
		task:SetStatus(2);
    elseif task:GetStatus() == 2  then
        task:SetStatus(1);
	end
end

function OnGetItem(player, task, itemid, num)
	local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = task:GetVar("p3");
    if itemid == 360000002 then
        item1 = item1 + num;
        task:SetVar("p1", item1);
    end
    if itemid == 300000105 then
        item2 = item2 + num;
        task:SetVar("p2", item2);
    end

	if CheckTask(item1, item2, item3) then
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
	local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = task:GetVar("p3");
    if itemid == 360000002 then
        item1 = item1 - num;
        if item1 < 0 then
            item1 = 0;
        end
        task:SetVar("p1", item1);
    end
    if itemid == 300000105 then
        item2 = item2 - num;
        if item2 < 0 then
            item2 = 0;
        end
        task:SetVar("p2", item2);
    end

	if task:GetStatus() == 2 and (not CheckTask(item1, item2, item3)) then
			task:SetStatus(1);
	end
end

function CheckFinish(player, task)		---检查任务完成条件
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = task:GetVar("p3");
    if CheckTask(item1, item2, item3) then
		task:SetStatus(2);
	else
		task:SetStatus(1);
	end
end
