function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindStrengthenEquipEvent(true, 8)		--ǿ��(�Ƿ��ע�ɹ�, ǿ���ȼ�)
	-- task:BindGetItemQualityEvent(0, 8)			--������ȡװ��(װ��id, ǿ���ȼ�)
	task:BindGetItemStrengthenEvent(0, 8)			--������ȡװ��(װ��id, ǿ���ȼ�)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local strengthen = player:GetStrMaxLvAll()
	if strengthen >= 8 then
		task:SetVar("parameter",1);			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnStrengthenEquip(player, task, errorCode, oldStrengthen, newStrengthen)	--ǿ��
	if newStrengthen >= 8 then
		task:SetVar("parameter",1);			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
	end
end

function OnGetItem(player, task, itemId, itemNum)
	task:SetVar("parameter",1);			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
end

function OnRmItem(player, task, itemId, itemNum)
end

function CheckFinish(player, task)		---��������������
end
