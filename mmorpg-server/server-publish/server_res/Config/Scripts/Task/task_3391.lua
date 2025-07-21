function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindStrengthenEquipEvent(false, 10)		--强化(是否只关注成功, 强化等级)
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

function OnStrengthenEquip(player, task, errorCode, oldStrengthen, newStrengthen)	--强化
	local num = task:GetVar("parameter");
--errorCode(1000019:装备强化失败破碎, 0:成功)
	if errorCode == 1000019 and oldStrengthen>=10 then			---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
		num = num + 1;
		task:SetVar("parameter",num);
		if num >= 10 then
			task:SetStatus(2);	      			-- 满足条件时设置任务为完成状态
		end
	end
end


function CheckFinish(player, task)		---检查任务完成条件
end
