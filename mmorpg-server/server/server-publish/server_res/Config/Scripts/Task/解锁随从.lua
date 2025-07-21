function Init(task)					---绑定(注册)该任务需要关注的事件
    task:BindAddRetinueEvent(0)		-- 关注添加随从事件(p1:随从品质, 0,侍从 1,骑士 2,贤者 3,传说)
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作	
	local rsize = player:GetRetinueSize();				--获取随从个数
	
	--获取指定品阶以上的数量(0:侍从, 1:骑士, 2:贤者, 3:传说)
	local qrSize = player:GetRetinueSizeByQuality();
	
	if(rsize >= 1) then
		task:SetVar("parameter", rsize);
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end


function OnAddRetinue(player, task, quality)
	local rsize = player:GetRetinueSize();				--获取随从个数
	
	--获取指定品阶以上的数量(0:侍从, 1:骑士, 2:贤者, 3:传说)
	local qrSize = player:GetRetinueSizeByQuality();
	
	if(rsize >= 1) then
		task:SetVar("parameter", rsize);
		task:SetStatus(2);
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
