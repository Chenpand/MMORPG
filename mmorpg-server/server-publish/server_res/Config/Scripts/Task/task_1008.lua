function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindGetItemEvent(360000001)		---关注获得道具事件。 帝王印章
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	if player:CheckLegendTaskFinished(1003) and
	player:CheckLegendTaskFinished(1004) and
	player:CheckLegendTaskFinished(1005) and
	player:CheckLegendTaskFinished(1006) and
	player:CheckLegendTaskFinished(1007) then
		return true;
	end
	return false;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);
	if item2 >= 80 then
		task:SetStatus(2);
	end	
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
    local item2 = task:GetVar("p2");
	if item2 >= 80 then
		task:SetStatus(4);
		player:RemoveItem(task:GetRewardsReason(), 360000001, 80);        ---	删除变量值的道具
	end
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnGetItem(player, task, itemid, num)		---	任务过程中获得道具时需要执行的操作
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);

	if item2 >= 80 then						---	判断结束条件
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);

	if(task:GetStatus() == 2 and item2 < 80) then		-- 如果删除一定数量的道具后不满足完成任务的条件，则设置该任务为未完成状态
		task:SetStatus(1);
	end
end	

function CheckFinish(player, task)		---检查任务完成条件
    local item2 = task:GetVar("p2");
	if item2 >= 80 then
		task:SetStatus(2);
	else
		task:SetStatus(1);
	end	
end
