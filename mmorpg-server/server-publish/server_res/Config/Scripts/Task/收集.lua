function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindGetItemEvent(400000008)		---关注获得道具事件。
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local itemnum = player:GetItemNum(400000008);
			task:SetVar("parameter", itemnum);	
		if itemnum >= 5 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
		local itemnum = task:GetVar("parameter");	---	得到变量值
		if(itemnum >= 5) then           -- 检查道具数量是否足够
			task:SetStatus(4);
			player:RemoveItem(task:GetRewardsReason(), 400000008, 5);        ---	删除变量值的道具
		end
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnGetItem(player, task, itemid, num)		---	任务过程中获得道具时需要执行的操作
	local itemnum = task:GetVar("parameter");		---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	itemnum = itemnum + num;					---	设定函数公式
	task:SetVar("parameter",itemnum);				---	设置变量新值
	if itemnum >= 5 then						---	判断结束条件
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
	local itemnum = task:GetVar("parameter");
	itemnum = itemnum - num;
	if(itemnum < 0) then
		itemnum = 0;
	end
	task:SetVar("parameter",itemnum);

	if(task:GetStatus() == 2 and itemnum < 5) then		-- 如果删除一定数量的道具后不满足完成任务的条件，则设置该任务为未完成状态
			task:SetStatus(1);
	end
end	

function CheckFinish(player, task)		---检查任务完成条件
end
