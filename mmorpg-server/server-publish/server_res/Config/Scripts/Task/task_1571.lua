function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindKillNpcEvent(509, 602000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(509, 602001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(509, 602002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(509, 602003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(510, 602000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(510, 602001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(510, 602002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(510, 602003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(511, 602000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(511, 602001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(511, 602002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(511, 602003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(512, 602000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(512, 602001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(512, 602002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(512, 602003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(513, 602000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(513, 602001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(513, 602002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(513, 602003)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(835, 602000)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(835, 602001)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(835, 602002)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(835, 602003)	 -- ��עɱ��ָ�������¼�

    task:BindGetItemEvent(400000001)		---��ע��õ����¼���
	
	
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local itemnum = player:GetItemNum(400000001);
			task:SetVar("parameter", itemnum);	
		if itemnum >= 1500 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
	local itemnum = task:GetVar("parameter");	---	�õ�����ֵ
		if(itemnum >= 1500) then           -- �����������Ƿ��㹻
			task:SetStatus(4);
			player:RemoveItem(task:GetRewardsReason(), 400000001, 1500);        ---	ɾ������ֵ�ĵ���
		end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnKillNpc(player, task, npcid, npctype, dungeonId)		---	���������ɱ��NPCʱ��Ҫִ�еĲ���
	local dungeonType = dungeonId % 10
	if dungeonType == 0 then
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������

			task:AddTaskItem(400000001, 60);					-- ����������
			task:AddTaskItem(400000001, 30);					-- ����������
			task:AddTaskItem(400000001, 10);					-- ����������
		else 
			task:AddTaskItem(400000001, 60);					-- ����������
			task:AddTaskItem(400000001, 30);					-- ����������
			task:AddTaskItem(400000001, 10);					-- ����������
		end
	elseif dungeonType == 1 then
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������

			task:AddTaskItem(400000001, 75);					-- ����������
			task:AddTaskItem(400000001, 55);					-- ����������
			task:AddTaskItem(400000001, 35);					-- ����������
			task:AddTaskItem(400000001, 20);					-- ����������
		else 
			task:AddTaskItem(400000001, 75);					-- ����������
			task:AddTaskItem(400000001, 55);					-- ����������
			task:AddTaskItem(400000001, 35);					-- ����������
			task:AddTaskItem(400000001, 20);					-- ����������
		end
	elseif dungeonType == 2 then
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������

			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 80);					-- ����������
			task:AddTaskItem(400000001, 50);					-- ����������
			task:AddTaskItem(400000001, 20);					-- ����������
		else 
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 80);					-- ����������
			task:AddTaskItem(400000001, 50);					-- ����������
			task:AddTaskItem(400000001, 20);					-- ����������
		end
	else
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������
			task:AddTaskItem(400000001, 100);					-- ����������

			task:AddTaskItem(400000001, 95);					-- ����������
			task:AddTaskItem(400000001, 85);					-- ����������
			task:AddTaskItem(400000001, 75);					-- ����������
			task:AddTaskItem(400000001, 50);					-- ����������
			task:AddTaskItem(400000001, 30);					-- ����������
		else 
			task:AddTaskItem(400000001, 95);					-- ����������
			task:AddTaskItem(400000001, 85);					-- ����������
			task:AddTaskItem(400000001, 75);					-- ����������
			task:AddTaskItem(400000001, 50);					-- ����������
			task:AddTaskItem(400000001, 30);					-- ����������
		end
	end



end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
	local itemnum = task:GetVar("parameter");		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	itemnum = itemnum + num;					---	�趨������ʽ
	task:SetVar("parameter",itemnum);				---	���ñ�����ֵ
	if itemnum >= 1500 then						---	�жϽ�������
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
	local itemnum = task:GetVar("parameter");
	itemnum = itemnum - num;
	if(itemnum < 0) then
		itemnum = 0;
	end
	task:SetVar("parameter",itemnum);

	if(task:GetStatus() == 2 and itemnum < 1500) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
			task:SetStatus(1);
	end
end	