function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindSubmitTaskSucceedEvent(1300) 	-- ��עɱ��ָ�������¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	if player:CheckTaskFinished(1300) then
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnSubmitTaskSucceed(player, task, taskid)		---	�����������ʱ��Ҫִ�еĲ���
	if player:CheckTaskFinished(1300) then
		task:SetStatus(2);
	end
end

function CheckFinish(player, task)		---��������������
end
