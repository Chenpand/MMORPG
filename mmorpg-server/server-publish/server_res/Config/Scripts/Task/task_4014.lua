function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEventByRevive(206000, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(206001, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(206002, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(206003, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
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
