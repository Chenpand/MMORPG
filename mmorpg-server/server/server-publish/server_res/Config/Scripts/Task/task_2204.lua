function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(201010)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearDungeonEvent(201011)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
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


function CheckFinish(player, task)		---��������������
end
