function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindGetItemEvent(400000031)		---��ע��õ����¼���
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local itemnum = player:GetItemNum(400000031);
			task:SetVar("parameter", itemnum);	
		if itemnum >= 20 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
		local itemnum = task:GetVar("parameter");	---	�õ�����ֵ
		if(itemnum >= 20) then           -- �����������Ƿ��㹻
			task:SetStatus(4);
			player:RemoveItem(task:GetRewardsReason(), 400000031, 20);
		end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
	local itemnum = task:GetVar("parameter");		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	itemnum = itemnum + num;					---	�趨������ʽ
	task:SetVar("parameter",itemnum);				---	���ñ�����ֵ
	if itemnum >= 20 then						---	�жϽ�������
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

	if(task:GetStatus() == 2 and itemnum < 20) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
			task:SetStatus(1);
	end
end	