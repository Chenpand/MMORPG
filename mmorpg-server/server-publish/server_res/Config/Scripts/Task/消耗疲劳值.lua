function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)疲劳消耗事件 (参数表示绑定消耗疲劳值,0表示不限定疲劳值)
	task:BindRemoveFatigueEvent(0)
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

function OnRemoveFatigue(player, task, fatigue)
	local num = task:GetVar("parameter");
	num = num + fatigue;
	task:SetVar("parameter", num);
	if(num >= 150) then
		task:SetStatus(2);	      					-- 满足条件时设置任务为完成状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
