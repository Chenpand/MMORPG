function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindSubmitTaskSucceedEvent(3532)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3533)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3534)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3535)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3536)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3537)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3538)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3539)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3540)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3541)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3542)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3543)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3544)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3545)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3546)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3547)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3548)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3549)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3550)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3551)   -- ��עɱ��ָ�������¼�
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
    if num >= 20 then
        task:SetStatus(2)                           ---��������޸�״̬
    end 

end

function CheckFinish(player, task)		---��������������
end
