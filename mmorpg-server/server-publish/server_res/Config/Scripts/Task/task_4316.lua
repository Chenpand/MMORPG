function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindSetTaskItemByIdEvent(300000106,5)	-- ��ע�ύ��Ʒ�¼�����ƷID, ��Ʒ������
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

	task:SetVar("parameter", itemNum);				---	���ñ�����ֵ
	task:SetStatus(2);
end


function CheckFinish(player, task)		---��������������
end
