function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindGetItemEvent(350000001)		---��ע��õ����¼���
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local itemnum = player:GetItemNum(350000001);
			task:SetVar("parameter", itemnum);	
		if itemnum >= 50 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
		local itemnum = task:GetVar("parameter");	---	�õ�����ֵ
		if(itemnum >= 50) then           -- �����������Ƿ��㹻
			task:SetStatus(4);
			player:RemoveItem(task:GetRewardsReason(), 350000001, 50);        ---	ɾ������ֵ�ĵ���
		end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
	local itemnum = task:GetVar("parameter");		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	itemnum = itemnum + num;					---	�趨������ʽ
	task:SetVar("parameter",itemnum);				---	���ñ�����ֵ
	if itemnum >= 50 then						---	�жϽ�������
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

	if(task:GetStatus() == 2 and itemnum < 50) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
			task:SetStatus(1);
	end
end	