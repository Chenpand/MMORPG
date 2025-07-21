function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindPVPEvent(1, 1)
	--task:BindPVPEvent(0, 0)	--(type, status) p1:类型(0.传统决斗,1.赛季) p2:PK结果(0.无效,1.胜利,2.失败,3.平局)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作	
	local winNum = task:GetVar("parameter");
	if winNum >= 50 then
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	else
		task:SetVar("parameter", winNum);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnPVP(player, task, pkType, status)
	local winNum = task:GetVar("parameter");	--获取最大连胜
	winNum = winNum + 1;	
	task:SetVar("parameter", winNum);

	if winNum >= 50 then
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
