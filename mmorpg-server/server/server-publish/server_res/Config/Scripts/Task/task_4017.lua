function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(207000, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(207001, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(207002, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(207003, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
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
	num = num + 1;								---	�趨������ʽ
	task:SetVar("parameter", num);					---	���ñ�����ֵ
	task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
end
