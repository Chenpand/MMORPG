function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindKillNpcEvent(105, 206010)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(105, 206011)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(105, 206012)	 -- ��עɱ��ָ�������¼�

    task:BindClearDungeonEvent(206010)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearDungeonEvent(206011)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
    task:BindClearDungeonEvent(206012)	-- ��עͨ��ĳ�����³��¼������³ǣ����ۣ�ʱ��/���룩
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

function OnKillNpc(player, task, npcid, npctype)        
    local npcnum = task:GetVar("parameter");            --- ��ȡ����
    if npcnum < 5 then                             --- �жϽ�������
        npcnum = npcnum + 1;                                --- �趨������ʽ
        task:SetVar("parameter", npcnum);                   --- ���ñ�����ֵ
    end
end

function OnClearDungeon(player, task, dungeonId, beated, areas)
    local npcnum = task:GetVar("parameter");            --- ��ȡ����
    if npcnum >= 5 then                             --- �жϽ�������
        task:SetStatus(2);
    end
end


function CheckFinish(player, task)		---��������������
end
