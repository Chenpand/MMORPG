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

function OnLostDungeonAliveTime(player, task, time)	---	�������������ʱ��ı�ʱ��Ҫִ�еĲ���
	task:SetVar("num",time);							---	���ñ�����ֵ
	if time >= 5 then								---	�жϽ�������
		task:SetStatus(2);
	end
	task:SyncData();
end

function CheckFinish(player, task)		---��������������
end
