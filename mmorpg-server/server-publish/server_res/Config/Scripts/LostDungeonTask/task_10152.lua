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
end

function OnLostDungeonKillFixRole(player, task, dieid)
	local id=task:GetVar("id");
	if dieid == id then								---	�жϽ�������
		task:SetStatus(2);
		task:SyncData();
	end
end

function CheckFinish(player, task)		---��������������
end
