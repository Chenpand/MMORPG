function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindPVPEvent(1)		---��עPVP�¼���   1.��ͳ���� 2.����
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
	local num = task:GetVar("parameter");	---	�õ�����ֵ
	if(num >= 1) then          --- �����������Ƿ��㹻
		task:SetStatus(4);          ---��ɲ����ύ��ȡ����
	end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnPVP(player, task, pkType, status)		---	��������л�õ���ʱ��Ҫִ�еĲ���
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + 1;
	task:SetVar("parameter",num);					---	���ñ�����ֵ
	if num >= 1 then
		task:SetStatus(2)							---��������޸�״̬
	end
end


function CheckFinish(player, task)		---��������������
end
