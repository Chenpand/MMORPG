function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindStrengthenEquipEvent(false, 0)		--ǿ��(�Ƿ��ע�ɹ�, ǿ���ȼ�)
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
    local num = task:GetVar("parameter");           --- ��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
    num = num + 1;
    task:SetVar("parameter",num);                   --- ���ñ�����ֵ
		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬

end


function CheckFinish(player, task)		---��������������
end
