function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindSetTaskItemEvent(2, 20)	-- ��ע�ύ��Ʒ�¼���0��Ч,1��ɫ,2��ɫ,3��ɫ,4��ɫ,5��ɫ,6��ɫ�� (��С�ȼ�)
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


function OnSetTaskItem(player, task, item, itemId, itemNum)
	player:RemoveItemDirect(task:GetRewardsReason(), item);
	player:RemoveItem(task:GetRewardsReason(), itemId, itemNum);

	local num = task:GetVar("parameter");			---	��ȡ����
	num = num + 1;									---	�趨������ʽ
	task:SetVar("parameter", num);				---	���ñ�����ֵ
	task:SetStatus(2);
end


function CheckFinish(player, task)		---��������������
end
