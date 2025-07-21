function Init(task)				---绑定(注册)该任务需要关注的事件
	-- 关注杀死指定类型的怪物事件 (类型:0-全部,1-怪物,2-精英,3-boss,4-npc) (地下城ID 0表示任何地下城)
	task:BindKillNpcTypeEvent(0, 0)
	
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
	local npcnum = task:GetVar("parameter");			---	获取变量
		npcnum = npcnum + 1;								---	设定函数公式
		task:SetVar("parameter", npcnum);					---	设置变量新值
	if npcnum >= 50 then								---	判断结束条件
		task:SetStatus(2);
	end
end