function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindGetItemEvent(330000110)		---关注获得道具事件。 死神气息
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local item1 = player:GetItemNum(330000110);
    task:SetVar("p1", item1);
	if item1 >= 100 then
		task:SetStatus(2);
	end	
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
	local item1 = task:GetVar("p1");	---	得到变量值
	if item1 >= 100 then
		task:SetStatus(4);
        player:RemoveItem(task:GetRewardsReason(), 330000110, 100);        ---	删除变量值的道具
	end
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnGetItem(player, task, itemid, num)		---	任务过程中获得道具时需要执行的操作
    local item1 = task:GetVar("p1");
    item1 = item1 + num;
    task:SetVar("p1",item1);				---	设置变量新值

	if item1 >= 100 then						---	判断结束条件
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
    local item1 = task:GetVar("p1");
    item1 = item1 - num;
    if item1 < 0 then
        item1 = 0;
    end
    task:SetVar("p1",item1);				---	设置变量新值

	if(task:GetStatus() == 2 and item1 < 100) then		-- 如果删除一定数量的道具后不满足完成任务的条件，则设置该任务为未完成状态
			task:SetStatus(1);
	end
end	

function CheckFinish(player, task)		---检查任务完成条件
    local item1 = task:GetVar("p1");
    task:SetVar("p1", item1);
	if item1 >= 100 then
		task:SetStatus(2);
	end	
end
