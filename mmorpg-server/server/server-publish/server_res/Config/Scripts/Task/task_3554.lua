function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindSubmitTaskSucceedEvent(3512)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3513)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3514)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3515)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3516)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3517)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3518)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3519)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3520)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3521)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3522)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3523)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3524)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3525)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3526)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3527)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3528)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3529)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3530)   -- 关注杀死指定怪物事件
    task:BindSubmitTaskSucceedEvent(3531)   -- 关注杀死指定怪物事件
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
