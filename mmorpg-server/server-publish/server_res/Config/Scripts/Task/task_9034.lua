function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	-- ��עɱ��ָ�����͵Ĺ����¼� (����:0-ȫ��,1-����,2-��Ӣ,3-boss,4-npc) (���³�ID 0��ʾ�κε��³�)
	task:BindKillNpcTypeEvent(0, 0)
	
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
		npcnum = npcnum + 1;								---	�趨������ʽ
		task:SetVar("parameter", npcnum);					---	���ñ�����ֵ
	if npcnum >= 50 then								---	�жϽ�������
		task:SetStatus(2);
	end
end