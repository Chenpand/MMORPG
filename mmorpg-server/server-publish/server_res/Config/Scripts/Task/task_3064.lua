function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)死亡之塔事件 (p1:塔ID, p2:层数(最大15) ,p3:限定时间 0:不限定)
	task:BindDeathTowerEvent(2003003, 15, 0)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local oldTowerId = task:GetVar("towerId");			--获取保存的最大的塔ID
	local oldLayer = task:GetVar("layer");				--获取保存的最高层数
	if (oldTowerId == 2003003 and oldLayer >= 15) or (oldTowerId > 2003003) then	--层数到达或者更高级的塔
		task:SetVar("parameter", 1);
		task:SetStatus(2)							---完成任务修改状态
	else
		task:SetVar("parameter", 0);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnDeathTower(player, task, towerId, layer, usedTime)
	local oldTowerId = task:GetVar("towerId");			--获取保存的最大的塔ID
	local oldLayer = task:GetVar("layer");				--获取保存的最高层数
	if towerId > oldTowerId then						--如果这次比之前打的塔高
		task:SetVar("towerId", towerId);				--保存塔ID和层数
		task:SetVar("layer", layer);
		oldTowerId = towerId;
		oldLayer = layer;
	elseif towerId == oldTowerId and layer > oldLayer then	--如果比之前的层数高  保存层数
		task:SetVar("layer", layer);
		oldLayer = layer;
	end
	if (oldTowerId == 2003003 and oldLayer >= 15) or (oldTowerId > 2003003) then	--层数到达或者更高级的塔
		task:SetVar("parameter", 1);
		task:SetStatus(2);							---完成任务修改状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
