function Init(task)				---��(ע��)��������Ҫ��ע���¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
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

function CheckFinish(player, task)		---��������������
end
