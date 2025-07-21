function Init(task)					---绑定(注册)该任务需要关注的事件
    task:BindAddRetinueEvent()		-- 关注添加随从事件
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作	
	--获取指定品阶以上的数量(0:侍从, 1:骑士, 2:贤者, 3:传说)
	local num = task:GetVar("parameter");	---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	local qrSize = player:GetRetinueSizeByQuality(2);
	
	if(qrSize >= 1) then
		num = num + 1
		task:SetVar("parameter",num)
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end


function OnAddRetinue(player, task, quality)
	--获取指定品阶以上的数量(0:侍从, 1:骑士, 2:贤者, 3:传说)
	local num = task:GetVar("parameter");	---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	local qrSize = player:GetRetinueSizeByQuality(2);
	
	if(qrSize >= 1) then
		num = num + 1
		task:SetVar("parameter",num)
		task:SetStatus(2);
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
