function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	-- ��עɱ��ָ�����͵Ĺ����¼� (����:0-ȫ��,1-����,2-��Ӣ,3-boss,4-npc) (���³�ID 0��ʾ�κε��³�)
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

function OnKillNpc(player, task, npcid, npctype, dungeonId)		---	���������ɱ��NPCʱ��Ҫִ�еĲ���
	local dungeonType = dungeonId % 10;		---��ȡ���³��Ѷ�(0:��ͨ,1:ð��,2:��ʿ,3:����,4:����)
	local num = task:GetVar("parameter");
	if dungeonType >= 1 then
		num = num + 1
		task:SetVar("parameter", num);					---	���ñ�����ֵ
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end			
end