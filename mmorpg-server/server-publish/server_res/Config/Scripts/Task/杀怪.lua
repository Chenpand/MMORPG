function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindKillNpcEvent(144, 206020)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(144, 206021)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(144, 206022)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(144, 206023)	 -- ��עɱ��ָ�������¼�
	
	
	-- ��עɱ��ָ�����͵Ĺ����¼� (����:1-����,2-��Ӣ,3-boss,4-npc) (���³�ID 0��ʾ�κε��³�)
	task:BindKillNpcTypeEvent(3, 0)
	
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

	task:AddTaskItem(400000001, 100);					-- ����������

	local npcnum = task:GetVar("parameter");			---	��ȡ����
	npcnum = npcnum + 1;								---	�趨������ʽ
	task:SetVar("parameter", npcnum);					---	���ñ�����ֵ
	if npcnum >= 5 then								---	�жϽ�������
		task:SetStatus(2);
	end
end

function CheckFinish(player, task)		---��������������
end
