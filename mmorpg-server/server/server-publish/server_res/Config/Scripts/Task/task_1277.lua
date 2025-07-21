function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindKillNpcEvent(102, 201000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 201001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 201002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 201003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 202000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 202001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 202002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 202003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 203000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 203001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 203002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 203003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 204000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 204001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 204002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 204003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 205000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 205001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 205002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 205003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 206000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 206001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 206002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(102, 206003)	 -- 关注杀死指定怪物事件
	
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

function OnKillNpc(player, task, npcid, type)		---	任务过程中杀死NPC时需要执行的操作
	local npcnum = task:GetVar("parameter");		---	获取变量
	npcnum = npcnum + 1;						---	设定函数公式
	task:SetVar("parameter", npcnum);				---	设置变量新值
		if npcnum >= 20 then							---	判断结束条件
			task:SetStatus(2);
		end
end