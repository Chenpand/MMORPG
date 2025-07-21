function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindChangeOccuEvent()	--转职
end

function CheckTask(player, task)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
    local isInitOccu = player:IsInitOccu();     --获取角色是否还是初始职业
    local num = task:GetVar("parameter");   --- 获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
    if not isInitOccu then                      --角色升级到15级，并完成转职
        num = num + 1
        task:SetVar("parameter",num);
        task:SetStatus(2)                       ---完成任务修改状态
    end

end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnChangeOccu(player, task)		--转职
    local num = task:GetVar("parameter");   --- 获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
    num = num + 1
    task:SetVar("parameter",num)
	task:SetStatus(2)
end




function CheckFinish(player, task)		---检查任务完成条件
end
