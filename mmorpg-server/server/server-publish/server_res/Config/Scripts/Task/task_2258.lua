function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(404000)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
	task:BindClearDungeonEvent(404001)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����
	task:BindClearDungeonEvent(404002)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����
	task:BindClearDungeonEvent(404003)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����
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
