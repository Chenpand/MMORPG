function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindGetItemEvent(360000001)		---��ע��õ����¼��� ����ӡ��
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	if player:CheckLegendTaskFinished(1003) and
	player:CheckLegendTaskFinished(1004) and
	player:CheckLegendTaskFinished(1005) and
	player:CheckLegendTaskFinished(1006) and
	player:CheckLegendTaskFinished(1007) then
		return true;
	end
	return false;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);
	if item2 >= 80 then
		task:SetStatus(2);
	end	
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
    local item2 = task:GetVar("p2");
	if item2 >= 80 then
		task:SetStatus(4);
		player:RemoveItem(task:GetRewardsReason(), 360000001, 80);        ---	ɾ������ֵ�ĵ���
	end
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnGetItem(player, task, itemid, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);

	if item2 >= 80 then						---	�жϽ�������
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
    local item2 = player:GetItemNum(360000001);
	task:SetVar("p2", item2);

	if(task:GetStatus() == 2 and item2 < 80) then		-- ���ɾ��һ�������ĵ��ߺ������������������������ø�����Ϊδ���״̬
		task:SetStatus(1);
	end
end	

function CheckFinish(player, task)		---��������������
    local item2 = task:GetVar("p2");
	if item2 >= 80 then
		task:SetStatus(2);
	else
		task:SetStatus(1);
	end	
end
