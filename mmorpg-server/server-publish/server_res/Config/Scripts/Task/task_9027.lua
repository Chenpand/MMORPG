function Init(task)				---绑定(注册)该任务需要关注的事件
	--绑定(注册)死亡之塔事件 (p1:塔ID, p2:层数(最大15) ,p3:限定时间 0:不限定)
	task:BindDeathTowerBeginEvent(0)
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

function OnDeathTowerBegin(player, task, towerId)
    local num = task:GetVar("parameter");   --- 获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
    num = num + 1;
    task:SetVar("parameter",num);               --- 设置变量新值
	task:SetStatus(2)			---完成任务修改状态
end
