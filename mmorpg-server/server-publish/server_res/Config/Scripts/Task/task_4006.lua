function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEventByRevive(205000, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(205001, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(205002, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(205003, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
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


function OnClearDungeon(player, task, dungeonId, beated, areas)
	local num = task:GetVar("parameter");			---	��ȡ����
	num = num + 1;									---	�趨������ʽ
	task:SetVar("parameter", num);				---	���ñ�����ֵ
	task:SetStatus(2);							-- ��������ʱ��������Ϊ���״̬
end
