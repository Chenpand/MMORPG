function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(205000, 0, 180000)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(205001, 0, 180000)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(205002, 0, 180000)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(205003, 0, 180000)	-- ��עͨ��ĳ�����³��¼�
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
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
end
