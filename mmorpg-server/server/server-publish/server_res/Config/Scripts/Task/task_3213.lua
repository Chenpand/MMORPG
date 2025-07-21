function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindPutEquipEvent(0, 5, 0)		--穿戴装备(p1:等级, p2:品质, p3:强化等级)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local num = player:GetEquipSize(0, 5);
	if num >= 1 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      			-- 满足条件时设置任务为完成状态
	else
		task:SetVar("parameter", 0);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnPutEquip(player, task, level, subType, strengthen)	--强化
	--(0:所有 1:白,2:蓝,3:紫, 4:绿, 5:粉, 6:黄)
	local num = player:GetEquipSize(0, 5);
	if num >= 1 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      			-- 满足条件时设置任务为完成状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
