function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindKillNpcEvent(0, 0)	 -- 关注杀死指定怪物事件
end

function CheckAccept(accountInfo)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(accountInfo, task)			---	接受任务时需要执行的操作	
end

function OnSubmitted(accountInfo, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(accountInfo, task)		---	放弃任务时需要执行的操作
end

function OnKillNpc(accountInfo, task, npcid, npctype)		---	任务过程中杀死NPC时需要执行的操作
	local npcnum = task:GetVar("p1");			---	获取变量
	npcnum = npcnum + 1;								---	设定函数公式
	task:SetVar("p1", npcnum);					---	设置变量新值
	if npcnum >= 5 then								---	判断结束条件
		task:SetStatus(2);
	end
end

function CheckFinish(accountInfo, task)		---检查任务完成条件
end
