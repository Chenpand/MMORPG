function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEvent(405000, 0, 0, 1);
	task:BindClearDungeonEvent(405001, 0, 0, 1);
	task:BindClearDungeonEvent(405002, 0, 0, 1);
	task:BindClearDungeonEvent(405003, 0, 0, 1);
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
