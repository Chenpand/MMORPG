function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindKillNpcEvent(102, 201000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 201001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 201002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 201003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 202000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 202001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 202002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 202003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 203000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 203001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 203002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 203003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 204000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 204001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 204002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 204003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 205000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 205001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 205002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 205003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 206000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 206001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 206002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(102, 206003)	 -- ��עɱ��ָ�������¼�
	
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