function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearDungeonEvent(205000)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearDungeonEvent(205001)  -- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearDungeonEvent(205002)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
	task:BindClearDungeonEvent(205003)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
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


function CheckFinish(player, task)		---��������������
end
