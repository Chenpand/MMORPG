function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindKillNpcEvent(146, 205000)   -- ��עɱ��ָ�������¼�
    task:BindKillNpcEvent(146, 205001)   -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(146, 205002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(146, 206003)	 -- ��עɱ��ָ�������¼�
	
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

function OnKillNpc(player, task, npcid, npctype)		---	���������ɱ��NPCʱ��Ҫִ�еĲ���
	local npcnum = task:GetVar("parameter");			---	��ȡ����
	npcnum = npcnum + 1;								---	�趨������ʽ
	task:SetVar("parameter", npcnum);					---	���ñ�����ֵ
	if npcnum >= 15 then								---	�жϽ�������
		task:SetStatus(2);
	end
end

function CheckFinish(player, task)		---��������������
end
