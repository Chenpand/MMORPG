function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(102000, 0, 120000)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(102001, 0, 120000)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(102002, 0, 120000)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(102003, 0, 120000)	-- ��עͨ��ĳ�����³��¼�
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
