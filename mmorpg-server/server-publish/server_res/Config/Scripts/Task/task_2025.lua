function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindKillNpcEvent(814, 205000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(814, 205001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(814, 205002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(814, 205003)	 -- ��עɱ��ָ�������¼�
	
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

function OnKillNpc(player, task, npcid, type)		---	���������ɱ��NPCʱ��Ҫִ�еĲ���
	local npcnum = task:GetVar("parameter");		---	��ȡ����
	npcnum = npcnum + 1;						---	�趨������ʽ
	task:SetVar("parameter", npcnum);				---	���ñ�����ֵ
		if npcnum >= 1 then							---	�жϽ�������
			task:SetStatus(2);
		end
end