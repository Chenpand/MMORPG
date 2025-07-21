function Init(task)					---绑定(注册)该任务需要关注的事件
    task:BindPutEquipEvent(0,5)		-- 关注穿戴装备事件
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作	
	local typeMax = 9;
	for i = 1, typeMax, 1
	do
		local keyStr = "type_10_"..tostring(i);
		local tmpType = task:GetVar(keyStr);
		if(tmpType == 1) then
			task:SetStatus(2);
			task:SetVar("parameter", 1);
			return;
		end
	end
	task:SetVar("parameter", 0);			--设置初始值
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end


function OnPutEquip(player, task, subtype, strength)
	local typeMax = 9;
	if(subtype > typeMax or subtype < 1) then
		return;
	end
	
	local keyStr = "type_5_"..tostring(subtype);
	local tmpType = task:GetVar(keyStr);
	if(tmpType == 0) then
		task:SetVar(keyStr, 1);
	end

	if(strength >= 10) then
		local keyStr = "type_10_"..tostring(subtype);
		local tmpType = task:GetVar(keyStr);
		if(tmpType == 0) then
			task:SetVar(keyStr, 1);
		end
		local oldnum = task:GetVar("parameter");
		if(oldnum == 0) then
			task:SetVar("parameter", 1);
			task:SetStatus(2);
		end
	end
end
