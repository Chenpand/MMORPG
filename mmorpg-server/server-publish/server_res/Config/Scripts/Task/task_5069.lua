function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(502000)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(502001)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(502002)	-- ��עͨ��ĳ�����³��¼�
    task:BindClearDungeonEvent(502003)	-- ��עͨ��ĳ�����³��¼�
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
	if(beated < 20) then   -- ���ͨ��ʱ���������Լ�ͨ���������Ƿ�����Ҫ��
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end
