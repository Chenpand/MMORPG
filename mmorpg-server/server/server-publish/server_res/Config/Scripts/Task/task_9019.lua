function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)商城购买事件 (p1:物品ID)
	task:BindBuyMallItemEvent(0)
	
	--绑定(注册)商店购买事件 (p1:商店ID, P2:商品ID)
	task:BindShopBuyEvent(0, 0)
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

function OnBuyItem(player, task, itemNum)
	local num = task:GetVar("parameter");			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	num = num + itemNum;
	if num >= 1 then
		task:SetVar("parameter",1);
		task:SetStatus(2);	      					-- 满足条件时设置任务为完成状态
	else
		task:SetVar("parameter",num);
	end
end

function OnBuyMallItem(player, task, mallItemId, mallItemNum)
	OnBuyItem(player, task, mallItemNum);
end

function OnShopBuy(player, task, shopId, shopItemId, shopItemNum)
	OnBuyItem(player, task, shopItemNum);
end


function CheckFinish(player, task)		---检查任务完成条件
end
