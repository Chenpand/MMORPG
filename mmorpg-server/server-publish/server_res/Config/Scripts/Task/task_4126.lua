function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindClearDungeonEvent(403000, 0, 0, 1);
	task:BindClearDungeonEvent(403001, 0, 0, 1);
	task:BindClearDungeonEvent(403002, 0, 0, 1);
	task:BindClearDungeonEvent(403003, 0, 0, 1);
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
