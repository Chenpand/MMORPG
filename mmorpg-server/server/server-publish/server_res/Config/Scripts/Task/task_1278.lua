function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindKillNpcEvent(103, 201000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 201001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 201002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 201003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 202000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 202001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 202002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 202003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 203000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 203001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 203002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 203003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 204000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 204001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 204002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 204003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 205000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 205001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 205002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 205003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 206000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 206001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 206002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(103, 206003)	 -- ��עɱ��ָ�������¼�
	
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
		if npcnum >= 20 then							---	�жϽ�������
			task:SetStatus(2);
		end
end