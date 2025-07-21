function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindSubmitTaskSucceedEvent(3532)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3533)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3534)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3535)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3536)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3537)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3538)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3539)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3540)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3541)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3542)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3543)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3544)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3545)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3546)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3547)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3548)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3549)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3550)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3551)   -- 关注杀死指定怪物事件
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

function OnSubmitTaskSucceed(player, task, taskid)		---	完成其他任务时需要执行的操作
    local num = task:GetVar("parameter");           --- 获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
    num = num + 1;
	task:SetVar("parameter", num);
    if num >= 20 then
        task:SetStatus(2)                           ---完成任务修改状态
    end 

end

function CheckFinish(player, task)		---检查任务完成条件
end
