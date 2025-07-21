function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)开罐子事件  (p1: 0-全部 1-武道会; 8-魔罐; 9-金罐 ,  p2:combo)
	task:BindOpenMagicJarEvent(0,1)
	--绑定(注册)开罐子10连抽事件
	task:BindOpenMagicJarEvent(0,10)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local num = task:GetVar("parameter");		---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	if num >= 5 then
		task:SetStatus(2);	      -- 满足条件时设置任务为完成状态
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnOpenMagicJar(player, task, type, combo)
	local num = task:GetVar("parameter");			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	num = num + 1;
	task:SetVar("parameter",num);					---	设置变量新值
	if num >= 5 then
		task:SetStatus(2);	      					-- 满足条件时设置任务为完成状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
