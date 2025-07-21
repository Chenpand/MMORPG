function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindGetItemEvent(330000108)		---关注获得道具事件。 光骑士之魂
    task:BindGetItemEvent(360000001)		---关注获得道具事件。 帝王印章
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local item1 = player:GetItemNum(330000108);
    task:SetVar("p1", item1);
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);
	if item1 >= 60 and item2 >= 15 then
		task:SetStatus(2);
	end	
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
	local item1 = task:GetVar("p1");	---	得到变量值
    local item2 = task:GetVar("p2");
	if item1 >= 60 and item2 >= 15 then
		task:SetStatus(4);
        player:RemoveItem(task:GetRewardsReason(), 330000108, 60);        ---	删除变量值的道具
		player:RemoveItem(task:GetRewardsReason(), 360000001, 15);        ---	删除变量值的道具
		player:AcceptLegendTask(1008);									--接取后续任务
	end
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnGetItem(player, task, itemid, num)		---	任务过程中获得道具时需要执行的操作
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    if itemid == 330000108 then
        item1 = item1 + num;
        task:SetVar("p1",item1);				---	设置变量新值
    end
    if itemid == 360000001 then
        item2 = item2 + num;
        task:SetVar("p2",item2);				---	设置变量新值
    end

	if item1 >= 60 and item2 >= 15 then						---	判断结束条件
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    if itemid == 330000108 then
        item1 = item1 - num;
        if item1 < 0 then
            item1 = 0;
        end
        task:SetVar("p1",item1);				---	设置变量新值
    end
    if itemid == 360000001 then
        item2 = item2 - num;
        if item2 < 0 then
            item2 = 0;
        end
        task:SetVar("p2",item2);				---	设置变量新值
    end

	if(task:GetStatus() == 2 and (item1 < 60 or item2 < 15)) then		-- 如果删除一定数量的道具后不满足完成任务的条件，则设置该任务为未完成状态
			task:SetStatus(1);
	end
end	

function CheckFinish(player, task)		---检查任务完成条件
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
	if item1 >= 60 and item2 >= 15 then
		task:SetStatus(2);
	end	
end
