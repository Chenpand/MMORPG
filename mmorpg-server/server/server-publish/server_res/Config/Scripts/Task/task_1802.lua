function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	---��ע��õ����¼��� (1.���,2.��ȯ,3.����,11.������, 12.������)
	task:BindChangeMoneyEvent(1)            ---���
    task:BindGetItemEvent(360000002)		---��ע��õ����¼���  �ƿ�ʯ
    task:BindGetItemEvent(300000105)		---��ע��õ����¼���  ��ɫ����
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function CheckTask(item1, item2, item3)
    if item1 >= 150 and item2 >= 1000 and item3 >= 150000 then
        return true;
    end
    return false;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    local item1 = player:GetItemNum(360000002);
    task:SetVar("p1", item1);
    local item2 = player:GetItemNum(300000105);
    task:SetVar("p2", item2);
    local item3 = player:GetGold();
    task:SetVar("p3", item3);
    if CheckTask(item1, item2, item3) then
        task:SetStatus(2);
    end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
    local item1 = task:GetVar("p1");	---	�õ�����ֵ
    local item2 = task:GetVar("p2");	---	�õ�����ֵ
    local item3 = task:GetVar("p3");	---	�õ�����ֵ

    if CheckTask(item1, item2, item3) then
        task:SetStatus(4);
        player:RemoveItem(task:GetRewardsReason(), 360000002, 150);        ---	ɾ������ֵ�ĵ���
        player:RemoveItem(task:GetRewardsReason(), 300000105, 1000);        ---	ɾ������ֵ�ĵ���
        player:RemoveGold(task:GetRewardsReason(), 150000)
    end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnChangeMoney(player, task, moneyType, num)	--���ұ仯
    
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = player:GetGold();
    task:SetVar("p3", item3);

	if CheckTask(item1, item2, item3) then
		task:SetStatus(2);
    elseif task:GetStatus() == 2  then
        task:SetStatus(1);
	end
end

function OnGetItem(player, task, itemid, num)
	local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = task:GetVar("p3");
    if itemid == 360000002 then
        item1 = item1 + num;
        task:SetVar("p1", item1);
    end
    if itemid == 300000105 then
        item2 = item2 + num;
        task:SetVar("p2", item2);
    end

	if CheckTask(item1, item2, item3) then
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
	local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = task:GetVar("p3");
    if itemid == 360000002 then
        item1 = item1 - num;
        if item1 < 0 then
            item1 = 0;
        end
        task:SetVar("p1", item1);
    end
    if itemid == 300000105 then
        item2 = item2 - num;
        if item2 < 0 then
            item2 = 0;
        end
        task:SetVar("p2", item2);
    end

	if task:GetStatus() == 2 and (not CheckTask(item1, item2, item3)) then
			task:SetStatus(1);
	end
end

function CheckFinish(player, task)		---��������������
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    local item3 = task:GetVar("p3");
    if CheckTask(item1, item2, item3) then
		task:SetStatus(2);
	else
		task:SetStatus(1);
	end
end
