function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindGetItemEvent(330000108)		---��ע��õ����¼��� ����ʿ֮��
    task:BindGetItemEvent(360000001)		---��ע��õ����¼��� ����ӡ��
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local item1 = player:GetItemNum(330000108);
    task:SetVar("p1", item1);
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);
	if item1 >= 60 and item2 >= 15 then
		task:SetStatus(2);
	end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
	local item1 = task:GetVar("p1");	---	�õ�����ֵ
    local item2 = task:GetVar("p2");
	if item1 >= 60 and item2 >= 15 then
		task:SetStatus(4);
        player:RemoveItem(task:GetRewardsReason(), 330000108, 60);        ---	ɾ������ֵ�ĵ���
		player:RemoveItem(task:GetRewardsReason(), 360000001, 15);        ---	ɾ������ֵ�ĵ���
		player:AcceptLegendTask(1008);									--��ȡ��������
	end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    if itemid == 330000108 then
        item1 = item1 + num;
        task:SetVar("p1",item1);				---	���ñ�����ֵ
    end
    if itemid == 360000001 then
        item2 = item2 + num;
        task:SetVar("p2",item2);				---	���ñ�����ֵ
    end

	if item1 >= 60 and item2 >= 15 then						---	�жϽ�������
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
    if itemid == 330000108 then
        item1 = item1 - num;
        if item1 < 0 then
            item1 = 0;
        end
        task:SetVar("p1",item1);				---	���ñ�����ֵ
    end
    if itemid == 360000001 then
        item2 = item2 - num;
        if item2 < 0 then
            item2 = 0;
        end
        task:SetVar("p2",item2);				---	���ñ�����ֵ
    end

	if(task:GetStatus() == 2 and (item1 < 60 or item2 < 15)) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
			task:SetStatus(1);
	end
end	

function CheckFinish(player, task)		---��������������
    local item1 = task:GetVar("p1");
    local item2 = task:GetVar("p2");
	if item1 >= 60 and item2 >= 15 then
		task:SetStatus(2);
	end	
end
