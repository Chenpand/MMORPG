function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindPVPEvent(1)
	--task:BindPVPEvent(0, 0)	--(type, status) p1:类型(0.传统决斗,1.赛季) p2:PK结果(0.无效,1.胜利,2.失败,3.平局)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local pkLevelType = player:GetPkLevelType();	--获取类型(0.青铜,1.黄金,2.至尊)
	local pkLevel = player:GetPkLevel();			--获取段位

	if pkLevelType >= 0 and pkLevel >= 3 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnPVP(player, task, pkType, status)
	local pkLevelType = player:GetPkLevelType();	--获取类型(0.青铜,1.黄金,2.至尊)
	local pkLevel = player:GetPkLevel();			--获取段位

	if pkLevelType >= 0 and pkLevel >= 3 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end

function CheckFinish(player, task)		---检查任务完成条件
end
