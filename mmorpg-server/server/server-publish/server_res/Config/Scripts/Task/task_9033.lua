function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	-- ��עɱ��ָ�����͵Ĺ����¼� (����:0-ȫ��,1-����,2-��Ӣ,3-boss,4-npc) (���³�ID 0��ʾ�κε��³�)
	task:BindKillNpcTypeEvent(2, 0)
	
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

function OnKillNpc(player, task, npcid, npctype, dungeonId)		---	���������ɱ��NPCʱ��Ҫִ�еĲ���
	local npcnum = task:GetVar("parameter");			---	��ȡ����
	local dungeonType = dungeonId % 10;		---��ȡ���³��Ѷ�(0:��ͨ,1:ð��,2:��ʿ,3:����,4:����)
	if dungeonType >= 1 then
		npcnum = npcnum + 1;								---	�趨������ʽ
		task:SetVar("parameter", npcnum);					---	���ñ�����ֵ
	end	
	if npcnum >= 3 then								---	�жϽ�������
		task:SetStatus(2);
	end
end