function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)�̳ǹ����¼� (p1:��ƷID)
	task:BindBuyMallItemEvent(0)
	
	--��(ע��)�̵깺���¼� (p1:�̵�ID, P2:��ƷID)
	task:BindShopBuyEvent(0, 0)
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

function OnBuyItem(player, task, itemNum)
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + itemNum;
	if num >= 1 then
		task:SetVar("parameter",1);
		task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
	else
		task:SetVar("parameter",num);
	end
end

function OnBuyMallItem(player, task, mallItemId, mallItemNum)
	OnBuyItem(player, task, mallItemNum);
end

function OnShopBuy(player, task, shopId, shopItemId, shopItemNum)
	OnBuyItem(player, task, shopItemNum);
end


function CheckFinish(player, task)		---��������������
end
