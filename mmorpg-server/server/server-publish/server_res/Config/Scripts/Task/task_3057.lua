function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)次元石升级事件 p1:石头类型(0.无效,1.红,2.绿,3.黄,4.蓝,5.黑) p2:等级(0表示无效)
	task:BindWarpStoneUpLevelEvent(0, 20)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local num = player:GetWarpStoneSize(0, 20);		--(石头类型, 等级)
	task:SetVar("parameter",num);				---	设置变量新值
	if num >= 9 then
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnWarpStoneUpLevel(player, task, wsType, level)
	local num = player:GetWarpStoneSize(0, 20);		--(石头类型, 等级)
	task:SetVar("parameter",num);				---	设置变量新值
	if num >= 9 then
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
