function Init(task)				---绑定(注册)该任务需要关注的事件
	-- 关注杀死指定类型的怪物事件 (类型:0-全部,1-怪物,2-精英,3-boss,4-npc) (地下城ID 0表示任何地下城)
	task:BindKillNpcTypeEvent(3, 0)
	
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

function OnKillNpc(player, task, npcid, npctype, dungeonId)		---	任务过程中杀死NPC时需要执行的操作
	local dungeonType = dungeonId % 10;		---获取地下城难度(0:普通,1:冒险,2:勇士,3:王者,4:地狱)
	local num = task:GetVar("parameter");
	if dungeonType >= 1 then
		num = num + 1
		task:SetVar("parameter", num);					---	设置变量新值
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end			
end