function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEventByRevive(305000, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(305001, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(305002, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
	task:BindClearDungeonEventByRevive(305003, 1); --��עͨ��ĳ�����³�(���³�ID, �޶��������)(0��ʾ���޶�, N��ʾ���Ը���N-1��)
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


function CheckFinish(player, task)		---��������������
end
