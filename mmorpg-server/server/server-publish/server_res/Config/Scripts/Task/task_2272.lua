function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindPVPEvent(1)		---关注PVP事件。   1.传统决斗 2.赛季
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
	local num = task:GetVar("parameter");	---	得到变量值
	if(num >= 1) then          --- 检查道具数量是否足够
		task:SetStatus(4);          ---完成并且提交领取奖励
	end
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnPVP(player, task, pkType, status)		---	任务过程中获得道具时需要执行的操作
	local num = task:GetVar("parameter");			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	num = num + 1;
	task:SetVar("parameter",num);					---	设置变量新值
	if num >= 1 then
		task:SetStatus(2)							---完成任务修改状态
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
