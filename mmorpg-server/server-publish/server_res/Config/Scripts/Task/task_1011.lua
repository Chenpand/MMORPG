function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindGetItemEvent(330000110)		---��ע��õ����¼��� ������Ϣ
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local item1 = player:GetItemNum(330000110);
    task:SetVar("p1", item1);
	if item1 >= 100 then
		task:SetStatus(2);
	end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
	local item1 = task:GetVar("p1");	---	�õ�����ֵ
	if item1 >= 100 then
		task:SetStatus(4);
        player:RemoveItem(task:GetRewardsReason(), 330000110, 100);        ---	ɾ������ֵ�ĵ���
	end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
    local item1 = task:GetVar("p1");
    item1 = item1 + num;
    task:SetVar("p1",item1);				---	���ñ�����ֵ

	if item1 >= 100 then						---	�жϽ�������
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
    local item1 = task:GetVar("p1");
    item1 = item1 - num;
    if item1 < 0 then
        item1 = 0;
    end
    task:SetVar("p1",item1);				---	���ñ�����ֵ

	if(task:GetStatus() == 2 and item1 < 100) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
			task:SetStatus(1);
	end
end	

function CheckFinish(player, task)		---��������������
    local item1 = task:GetVar("p1");
    task:SetVar("p1", item1);
	if item1 >= 100 then
		task:SetStatus(2);
	end	
end
