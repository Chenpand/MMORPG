function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindSubmitTaskSucceedEvent(1001) 	-- ��ע��������¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	local task = player:FindTask(1274);
	if player:CheckTaskFinished(1001) then
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnSubmitTaskSucceed(player, task, taskid)		---	�����������ʱ��Ҫִ�еĲ���
	if player:CheckTaskFinished(1001) then
		task:SetStatus(2);
	end
end