function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)死亡之塔开始事件 (p1:塔ID 0表示所有塔)
	task:BindDeathTowerBeginEvent(0)
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

function OnDeathTowerBegin(player, task, towerId)
	local num = task::GetVar("parameter");
	num = num + 1;
	task:SetVar("parameter", num);
	if num >= 1 then
		task:SetStatus(2);						---完成任务修改状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
