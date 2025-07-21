function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindKillNpcEvent(101, 101000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(101, 101001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(101, 101002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(101, 101003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(101, 102000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(101, 102001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(101, 102002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(101, 102003)	 -- 关注杀死指定怪物事件
	
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