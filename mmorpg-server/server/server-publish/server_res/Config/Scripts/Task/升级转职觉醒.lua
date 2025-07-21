function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindAwakenEvent()		--觉醒
	task:BindChangeOccuEvent()	--转职
	task:BindLevelupEvent(15)	--升级 (表示监听等级)
end

function CheckTask(player, task)
	local isInitOccu = player:IsInitOccu();		--获取角色是否还是初始职业
	if not isInitOccu then						--角色升级到15级，并完成转职
		task:SetVar("parameter",1);
		task:SetStatus(2)						---完成任务修改状态
	end
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local level = player:GetLevel();			--获取角色等级
	if level >= 15 then
		CheckTask(player, task)
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnPlayerLevelup(player, task)	--升级
	CheckTask(player, task)
end

function OnChangeOccu(player, task)		--转职
	CheckTask(player, task)
end

function OnAwaken(player, task)			--觉醒
	CheckTask(player, task)
end



function CheckFinish(player, task)		---检查任务完成条件
end
