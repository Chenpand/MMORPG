function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)����֮���¼� (p1:��ID, p2:����(���15) ,p3:�޶�ʱ�� 0:���޶�)
	task:BindDeathTowerBeginEvent(0)
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

function OnDeathTowerBegin(player, task, towerId)
    local num = task:GetVar("parameter");   --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
    num = num + 1;
    task:SetVar("parameter",num);               --- ���ñ�����ֵ
	task:SetStatus(2)			---��������޸�״̬
end
