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

function OnLostDungeonClearNum(player, task, num)
	task:SetVar("num", num);
	if num >= 2 then								---	�жϽ�������
		task:SetStatus(2);
	end
	task:SyncData();
end

function OnLostDungeonFixClear(player, task, dungeonId, num)
end

function CheckFinish(player, task)		---��������������
end
