function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)日常任务提交事件
	task:BindDailyTaskSubmittedEvent(0)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	task:SetVar("taskNum", 10);
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnDailyTaskSubmitted(player, task, taskType)
	local taskNum = task:GetVar("taskNum");			--- 日常任务个数
	local num = task:GetVar("parameter");			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	num = num + 1;
	task:SetVar("parameter",num);					---	设置变量新值
	if num >= taskNum then
		task:SetStatus(2);	      					-- 满足条件时设置任务为完成状态
	end
end
