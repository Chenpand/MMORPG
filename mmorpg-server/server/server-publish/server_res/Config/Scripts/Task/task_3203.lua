function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindSubmitTaskSucceedEvent(2246) 	-- ��עɱ��ָ�������¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	if player:CheckTaskFinished(2246) then
        task:SetVar("parameter",1);        
		task:SetStatus(2);
	else
		task:SetVar("parameter",0);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnSubmitTaskSucceed(player, task, taskid)		---	�����������ʱ��Ҫִ�еĲ���
	if player:CheckTaskFinished(2246) then
        task:SetVar("parameter",1);        
		task:SetStatus(2);
	end
end

function CheckFinish(player, task)		---��������������
end
