function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindKillNpcEvent(834, 601000)   -- ��עɱ��ָ�������¼�
    task:BindKillNpcEvent(834, 601001)   -- ��עɱ��ָ�������¼�
    task:BindKillNpcEvent(834, 601002)   -- ��עɱ��ָ�������¼�
    task:BindKillNpcEvent(834, 601003)   -- ��עɱ��ָ�������¼�
		
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
		task:SetStatus(2);
end