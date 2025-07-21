function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindClearDungeonEvent(0, 0, 0)				-- 关注通关任何地下城
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


function OnClearDungeon(player, task, dungeonId, beated, areas)
	local dungeonType = dungeonId % 10;		---获取地下城难度(0:普通,1:冒险,2:勇士,3:王者,4:地狱)
	local num = task:GetVar("parameter");
	if dungeonType >= 1 then
		num = num + 1;
		task:SetVar("parameter",num);					---	设置变量新值
		if num >= 50 then
			task:SetStatus(2);	      					-- 满足条件时设置任务为完成状态
		end
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
