function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindSetTaskItemEvent(2, 20)	-- 关注提交物品事件（0无效,1白色,2蓝色,3紫色,4绿色,5粉色,6黄色） (最小等级)
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


function OnSetTaskItem(player, task, item, itemId, itemNum)
	player:RemoveItemDirect(task:GetRewardsReason(), item);
	player:RemoveItem(task:GetRewardsReason(), itemId, itemNum);

	local num = task:GetVar("parameter");			---	获取变量
	num = num + 1;									---	设定函数公式
	task:SetVar("parameter", num);				---	设置变量新值
	task:SetStatus(2);
end


function CheckFinish(player, task)		---检查任务完成条件
end
