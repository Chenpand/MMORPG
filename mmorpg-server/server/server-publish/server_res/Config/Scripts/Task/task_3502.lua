function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindSubmitTaskSucceedEvent(3500)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3501)   -- ��עɱ��ָ�������¼�
	task:BindSubmitTaskSucceedEvent(3503)   -- ��עɱ��ָ�������¼�
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

function OnSubmitTaskSucceed(player, task, taskid)		---	�����������ʱ��Ҫִ�еĲ���
    local num = task:GetVar("parameter");           --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
    num = num + 1;
	task:SetVar("parameter", num);
    if num >= 3 then
        task:SetStatus(2)                           ---��������޸�״̬
    end 

end

function CheckFinish(player, task)		---��������������
end
