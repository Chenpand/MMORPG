function Init(task)				---��(ע��)��������Ҫ��ע���¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	task:SetStatus(2);				---	�������״̬������ ---0����ʼ��1��δ��ɡ�2�����
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	�������ʱ��Ҫִ�еĲ���
end

function CheckFinish(player, task)		---��������������
end
