function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindGetItemEvent(400000006)		---��ע��õ����¼���
	task:BindKillNpcEvent(146, 205010)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(146, 205011)	 -- ��עɱ��ָ�������¼�
	task:BindKillNpcEvent(146, 205012)	 -- ��עɱ��ָ�������¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local itemnum = player:GetItemNum(40000006);
			task:SetVar("parameter", itemnum);	
		if itemnum >= 12 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
		local itemnum = task:GetVar("parameter");	---	�õ�����ֵ
		if(itemnum >= 3) then           -- �����������Ƿ��㹻
			task:SetStatus(4);
			player:RemoveItem(task:GetRewardsReason(), 400000006, 12);        ---	ɾ������ֵ�ĵ���
		end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
	local itemnum = task:GetVar("parameter");		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	itemnum = itemnum + num;					---	�趨������ʽ
	task:SetVar("parameter",itemnum);				---	���ñ�����ֵ
	if itemnum >= 12 then						---	�жϽ�������
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

	if(task:GetStatus() == 2 and itemnum < 12) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
			task:SetStatus(1);
	end
end

function OnKillNpc(player, task, npcid, npctype)		---	���������ɱ��NPCʱ��Ҫִ�еĲ���
	local itemnum = task:GetVar("parameter");
    if itemnum < 12 then
    	task:AddTaskItem(400000006, 100);					-- ����������
    end
end
