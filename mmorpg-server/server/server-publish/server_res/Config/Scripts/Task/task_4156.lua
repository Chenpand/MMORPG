function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(407000, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(407001, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(407002, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
    task:BindClearDungeonEvent(407003, 0, 0, 0)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/����, ���������
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
	if num >= 1 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
