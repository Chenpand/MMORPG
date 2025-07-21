function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)活动任务提交事件(p1:活动大类ID, ID)
	task:BindSubmitActiveTaskSucceedEvent(3000, 0)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local num = player:GetSignInCount();
	task:SetVar("parameter", num);					---	设置变量新值
	if num >= 10 then
		task:SetStatus(2);	      					-- 满足条件时设置任务为完成状态
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnSubmitActiveTaskSucceed(player, task, templateId, taskId)
	local num = player:GetSignInCount();			--- 获取总签到次数
	task:SetVar("parameter",num);					---	设置变量新值
	if num >= 10 then
		task:SetStatus(2);	      					-- 满足条件时设置任务为完成状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
