function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindStrengthenEquipEvent(true, 8)		--强化(是否关注成功, 强化等级)
	-- task:BindGetItemQualityEvent(0, 8)			--监听获取装备(装备id, 强化等级)
	task:BindGetItemStrengthenEvent(0, 8)			--监听获取装备(装备id, 强化等级)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local strengthen = player:GetStrMaxLvAll()
	if strengthen >= 8 then
		task:SetVar("parameter",1);			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
		task:SetStatus(2);	      			-- 满足条件时设置任务为完成状态
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnStrengthenEquip(player, task, errorCode, oldStrengthen, newStrengthen)	--强化
	if newStrengthen >= 8 then
		task:SetVar("parameter",1);			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
		task:SetStatus(2);	      			-- 满足条件时设置任务为完成状态
	end
end

function OnGetItem(player, task, itemId, itemNum)
	task:SetVar("parameter",1);			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	task:SetStatus(2);	      			-- 满足条件时设置任务为完成状态
end

function OnRmItem(player, task, itemId, itemNum)
end

function CheckFinish(player, task)		---检查任务完成条件
end
