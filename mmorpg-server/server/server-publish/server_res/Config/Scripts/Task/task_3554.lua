function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindSubmitTaskSucceedEvent(3512)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3513)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3514)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3515)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3516)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3517)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3518)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3519)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3520)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3521)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3522)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3523)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3524)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3525)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3526)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3527)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3528)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3529)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3530)   -- ��עɱ��ָ�������¼�
    task:BindSubmitTaskSucceedEvent(3531)   -- ��עɱ��ָ�������¼�
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
