function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindClearActivityDungeonEvent(601001)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(601002)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearActivityDungeonEvent(601003)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
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


function OnClearActivityDungeon(player, task, dungeonId, beated, areas)
	local num = task:GetVar("parameter");			---	��ȡ����
		num = num + 1;								---	�趨������ʽ
		task:SetVar("parameter", num);					---	���ñ�����ֵ
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
end


function CheckFinish(player, task)		---��������������
end
