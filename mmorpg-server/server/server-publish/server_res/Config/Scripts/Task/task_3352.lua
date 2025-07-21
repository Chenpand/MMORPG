function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindChangeMoneyEvent(1)		---关注获得道具事件。
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local moneynum = task:GetVar("parameter");
	if moneynum >= 100000 then							---	判断结束条件
		task:SetStatus(2)
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnChangeMoney(player, task, moneyType, num)		---	任务过程中获得道具时需要执行的操作
	if num < 0 then
		num = num * -1;
	end
	local moneynum = task:GetVar("parameter");			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	moneynum = moneynum + num;							---	设定函数公式
	task:SetVar("parameter",moneynum);					---	设置变量新值
	if moneynum >= 100000 then							---	判断结束条件
		task:SetStatus(2)
	end
end
