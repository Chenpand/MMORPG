function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindLevelupEvent(20)	--升级 (表示监听等级)
end

function CheckTask(player, task)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local level = player:GetLevel();			--获取角色等级
	task:SetVar("parameter", level);
	if level >= 30 then
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnPlayerLevelup(player, task)	--升级
	local level = player:GetLevel();			--获取角色等级
	task:SetVar("parameter", level);
	if level >= 30 then
		task:SetStatus(2);
	end
end




function CheckFinish(player, task)		---检查任务完成条件
end
