function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	---��עװ��ǿ���¼���
	task:BindStrengthenEquipEvent(true, 10)
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

function OnStrengthenEquip(player, task, errorCode, oldStrengthen, newStrengthen)	--ǿ��
	if newStrengthen < 10 then
		return;
	end
	
	task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
end


function CheckFinish(player, task)		---��������������
end
