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

function OnLostDungeonKillRoleNum(player, task, num)
	task:SetVar("num",num);							---	���ñ�����ֵ
	if num >= 6 then								---	�жϽ�������
		task:SetStatus(2);
	end
	task:SyncData();
end

function OnLostDungeonKillFixRole(player, task, dieid)
end

function CheckFinish(player, task)		---��������������
end
