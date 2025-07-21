function Init(task)				---绑定(注册)该任务需要关注的事件
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

function OnLostDungeonCollectItem(player, task, id, num)

	local idTable = {[810001301]=50}
	
	local isFind = false;
	for k, v in pairs(idTable) do
		if k == id then
			isFind = true;
			break;
		end
	end

	if isFind then
		task:SetVar(tostring(id), num);
	else
		return;
	end
	
	local isFinish = true;
	for k, v in pairs(idTable) do
		local num = task:GetVar(tostring(k));
		if num < v then
			isFinish = false;
		end
	end
	
	if isFinish then
		task:SetStatus(2);
	end
	
	task:SyncData();
end

function CheckFinish(player, task)		---检查任务完成条件
end
